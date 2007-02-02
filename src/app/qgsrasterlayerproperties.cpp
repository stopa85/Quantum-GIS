/***************************************************************************
  qgsrasterlayerproperties.cpp  -  description
  -------------------
begin                : 1/1/2004
copyright            : (C) 2004 Tim Sutton
email                : tim@linfiniti.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "qgsapplication.h"
#include "qgscoordinatetransform.h"
#include "qgsrasterlayerproperties.h"
#include "qgslayerprojectionselector.h"
#include "qgsproject.h"
#include "qgsrasterbandstats.h"
#include "qgsrasterlayer.h"
#include "qgsrasterpyramid.h"
#include "qgscontexthelp.h"

#include <QTableWidgetItem>
#include <QHeaderView>

#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QPolygon>
#include <iostream>

const char * const ident = 
  "$Id$";


QgsRasterLayerProperties::QgsRasterLayerProperties(QgsMapLayer *lyr, QWidget *parent, Qt::WFlags fl)
: QDialog(parent, fl), 
  rasterLayer( dynamic_cast<QgsRasterLayer*>(lyr) )
{
  ignoreSpinBoxEvent = false; //Short circuit signal loop between min max field and stdDev spin box

  if (rasterLayer->getDataProvider() == 0)
  {
    // This is a GDAL-based layer
    rasterLayerIsGdal = TRUE;
    rasterLayerIsWms = FALSE;
  }
  else if (rasterLayer->getDataProvider()->name() == "wms")
  {
    // This is a WMS-based layer
    rasterLayerIsWms = TRUE;
    rasterLayerIsGdal = FALSE;
  }

  setupUi(this);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  connect(buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
  connect(this, SIGNAL(accepted()), this, SLOT(apply()));
  connect(sliderTransparency, SIGNAL(valueChanged(int)), this, SLOT(sliderTransparency_valueChanged(int)));

  //clear either stdDev or min max entries depending which is changed
  connect(sboxStdDevSingleBand, SIGNAL(valueChanged(double)), this, SLOT(sboxStdDevSingleBand_valueChanged(double)));
  connect(sboxStdDevThreeBand, SIGNAL(valueChanged(double)), this, SLOT(sboxStdDevThreeBand_valueChanged(double)));
  connect(leGrayMin, SIGNAL(textEdited(QString)), this, SLOT(userDefinedMinMax_textEdited(QString)));
  connect(leGrayMax, SIGNAL(textEdited(QString)), this, SLOT(userDefinedMinMax_textEdited(QString)));
  connect(leRedMin, SIGNAL(textEdited(QString)), this, SLOT(userDefinedMinMax_textEdited(QString)));
  connect(leRedMax, SIGNAL(textEdited(QString)), this, SLOT(userDefinedMinMax_textEdited(QString)));
  connect(leGreenMin, SIGNAL(textEdited(QString)), this, SLOT(userDefinedMinMax_textEdited(QString)));
  connect(leGreenMax, SIGNAL(textEdited(QString)), this, SLOT(userDefinedMinMax_textEdited(QString)));
  connect(leBlueMin, SIGNAL(textEdited(QString)), this, SLOT(userDefinedMinMax_textEdited(QString)));
  connect(leBlueMax, SIGNAL(textEdited(QString)), this, SLOT(userDefinedMinMax_textEdited(QString)));

  // set up the scale based layer visibility stuff....
  chkUseScaleDependentRendering->setChecked(lyr->scaleBasedVisibility());
  spinMinimumScale->setValue((int)lyr->minScale());
  spinMaximumScale->setValue((int)lyr->maxScale());

  // build GUI components
  cboColorMap->insertItem(tr("Grayscale"));
  cboColorMap->insertItem(tr("Pseudocolor"));
  cboColorMap->insertItem(tr("Freak Out"));

  //add items to the color stretch combo box
  cboxColorScalingAlgorithm->insertItem(tr("No Stretch"));
  cboxColorScalingAlgorithm->insertItem(tr("Stretch To MinMax"));
  cboxColorScalingAlgorithm->insertItem(tr("Stretch And Clip To MinMax"));
  cboxColorScalingAlgorithm->insertItem(tr("Clip To MinMax"));

  //
  // Set up the combo boxes that contain band lists using the qstring list generated above
  //

  if (rasterLayer->getRasterLayerType()
          == QgsRasterLayer::PALETTE) //paletted layers have hard coded color entries
  {
    cboRed->insertItem("Red");
    cboGreen->insertItem("Red");
    cboBlue->insertItem("Red");

    cboRed->insertItem("Green");
    cboGreen->insertItem("Green");
    cboBlue->insertItem("Green");

    cboRed->insertItem("Blue");
    cboGreen->insertItem("Blue");
    cboBlue->insertItem("Blue");

    cboRed->insertItem(tr("Not Set"));
    cboGreen->insertItem(tr("Not Set"));
    cboBlue->insertItem(tr("Not Set"));

    cboGray->insertItem("Red");
    cboGray->insertItem("Green");
    cboGray->insertItem("Blue");
    cboGray->insertItem(tr("Not Set"));

    lstHistogramLabels->insertItem(tr("Palette"));
  }
  else                   // all other layer types use band name entries only
  {
#ifdef QGISDEBUG
    std::cout << "Populating combos for non paletted layer" << std::endl;
#endif

    //
    // Get a list of band names
    //
    QStringList myBandNameQStringList;

    int myBandCountInt = rasterLayer->getBandCount();
#ifdef QGISDEBIG
    std::cout << "Looping though " << myBandCountInt << " image layers to get their names " << std::endl;
#endif
    for (int myIteratorInt = 1;
            myIteratorInt <= myBandCountInt;
            ++myIteratorInt)
    {
      //find out the name of this band
      QString myRasterBandNameQString = rasterLayer->getRasterBandName(myIteratorInt) ;

      //
      //add the band to the histogram tab
      //
      QPixmap myPixmap(10,10);

      if (myBandCountInt==1) //draw single band images with black
      {
        myPixmap.fill( Qt::black );
      }
      else if (myIteratorInt==1)
      {
        myPixmap.fill( Qt::red );
      }
      else if (myIteratorInt==2)
      {
        myPixmap.fill( Qt::green );
      }
      else if (myIteratorInt==3)
      {
        myPixmap.fill( Qt::blue );
      }
      else if (myIteratorInt==4)
      {
        myPixmap.fill( Qt::magenta );
      }
      else if (myIteratorInt==5)
      {
        myPixmap.fill( Qt::darkRed );
      }
      else if (myIteratorInt==6)
      {
        myPixmap.fill( Qt::darkGreen );
      }
      else if (myIteratorInt==7)
      {
        myPixmap.fill( Qt::darkBlue );
      }
      else
      {
        myPixmap.fill( Qt::gray );
      }
      lstHistogramLabels->insertItem(myPixmap,myRasterBandNameQString);
      //keep a list of band names for later use
      //! @note band names should not be translated!
      myBandNameQStringList.append(myRasterBandNameQString);
    }

    //select all histogram layers list items by default
    for (int myIteratorInt = 1;
          myIteratorInt <= myBandCountInt;
          ++myIteratorInt)
    {
      Q3ListBoxItem *myItem = lstHistogramLabels->item( myIteratorInt-1 );
      lstHistogramLabels->setSelected( myItem,true);
    }

    for (QStringList::Iterator myIterator = myBandNameQStringList.begin(); 
            myIterator != myBandNameQStringList.end(); 
            ++myIterator)
    {
      QString myQString = *myIterator;
#ifdef QGISDEBUG

      std::cout << "Inserting : " << myQString.toLocal8Bit().data() << std::endl;
#endif

      cboGray->insertItem(myQString);
      cboRed->insertItem(myQString);
      cboGreen->insertItem(myQString);
      cboBlue->insertItem(myQString);
      cboTransparent->insertItem(myQString);
    }
    cboRed->insertItem(tr("Not Set"));
    cboGreen->insertItem(tr("Not Set"));
    cboBlue->insertItem(tr("Not Set"));
    cboTransparent->insertItem(tr("Not Set"));
    if (cboGray->count() != 1)
      cboGray->insertItem(tr("Not Set"));
  }

  QString myThemePath = QgsApplication::themePath();
  QPixmap myPyramidPixmap(myThemePath + "/mIconPyramid.png");
  QPixmap myNoPyramidPixmap(myThemePath + "/mIconNoPyramid.png");

  // Only do pyramids if dealing directly with GDAL.
  if (rasterLayerIsGdal)
  {
    QgsRasterLayer::RasterPyramidList myPyramidList = rasterLayer->buildRasterPyramidList();
    QgsRasterLayer::RasterPyramidList::iterator myRasterPyramidIterator;
  
    for ( myRasterPyramidIterator=myPyramidList.begin();
            myRasterPyramidIterator != myPyramidList.end();
            ++myRasterPyramidIterator )
    {
      if ((*myRasterPyramidIterator).existsFlag==true)
      {
        lbxPyramidResolutions->insertItem(myPyramidPixmap,
                QString::number((*myRasterPyramidIterator).xDimInt) + QString(" x ") + 
                QString::number((*myRasterPyramidIterator).yDimInt)); 
      }
      else
      {
        lbxPyramidResolutions->insertItem(myNoPyramidPixmap,
                QString::number((*myRasterPyramidIterator).xDimInt) + QString(" x ") + 
                QString::number((*myRasterPyramidIterator).yDimInt)); 
      }
    }
  }
  else if (rasterLayerIsWms)
  {
    // disable Pyramids tab completely
    tabBar->setTabEnabled(tabBar->indexOf(tabPagePyramids), FALSE);

    // disable Histogram tab completely
    tabBar->setTabEnabled(tabBar->indexOf(tabPageHistogram), FALSE);
  }

  leSpatialRefSys->setText(rasterLayer->srs().proj4String());

  // update based on lyr's current state
  sync();
} // QgsRasterLayerProperties ctor


QgsRasterLayerProperties::~QgsRasterLayerProperties()
{
}

/*
 *
 * PUBLIC METHODS
 *
 */
void QgsRasterLayerProperties::apply()
{
  /*
   * Symbology Tab
   */
  //set the appropriate render style
  if (rbtnSingleBand->isChecked())
  {
    //
    // Grayscale
    //
    if (rasterLayer->rasterLayerType == QgsRasterLayer::GRAY_OR_UNDEFINED)
    {

      if (cboColorMap->currentText() != tr("Grayscale"))
      {
#ifdef QGISDEBUG
        std::cout << "Setting Raster Drawing Style to :: SINGLE_BAND_PSEUDO_COLOR" << std::endl;
#endif

        rasterLayer->setDrawingStyle(QgsRasterLayer::SINGLE_BAND_PSEUDO_COLOR);
      }
      else
      {
#ifdef QGISDEBUG
        std::cout << "Setting Raster Drawing Style to :: SINGLE_BAND_GRAY" << std::endl;
#endif

        rasterLayer->setDrawingStyle(QgsRasterLayer::SINGLE_BAND_GRAY);
      }
    }
    //
    // Paletted Image
    //
    else if (rasterLayer->rasterLayerType == QgsRasterLayer::PALETTE)
    {
      if (cboColorMap->currentText() != tr("Grayscale"))
      {
#ifdef QGISDEBUG
        std::cout << "Setting Raster Drawing Style to :: PALETTED_SINGLE_BAND_PSEUDO_COLOR" << std::endl;
#endif

        rasterLayer->setDrawingStyle(QgsRasterLayer::PALETTED_SINGLE_BAND_PSEUDO_COLOR);
      }
      else
      {
#ifdef QGISDEBUG
        std::cout << "Setting Raster Drawing Style to :: PALETTED_SINGLE_BAND_GRAY" << std::endl;
#endif
#ifdef QGISDEBUG

        std::cout << "Combo value : " << cboGray->currentText().toLocal8Bit().data() << " GrayBand Mapping : " << rasterLayer->
            getGrayBandName().toLocal8Bit().data() << std::endl;
#endif

        rasterLayer->setDrawingStyle(QgsRasterLayer::PALETTED_SINGLE_BAND_GRAY);
      }
    }
    //
    // Mutltiband
    //
    else if (rasterLayer->rasterLayerType == QgsRasterLayer::MULTIBAND)
    {
      if (cboColorMap->currentText() != tr("Grayscale"))
      {
#ifdef QGISDEBUG
        std::cout << "Setting Raster Drawing Style to ::MULTI_BAND_SINGLE_BAND_PSEUDO_COLOR " << std::endl;
#endif

        rasterLayer->setDrawingStyle(QgsRasterLayer::MULTI_BAND_SINGLE_BAND_PSEUDO_COLOR);
      }
      else
      {
#ifdef QGISDEBUG
        std::cout << "Setting Raster Drawing Style to :: MULTI_BAND_SINGLE_BAND_GRAY" << std::endl;
        std::cout << "Combo value : " << cboGray->currentText().toLocal8Bit().data() << " GrayBand Mapping : " << rasterLayer->
            getGrayBandName().toLocal8Bit().data() << std::endl;
#endif

        rasterLayer->setDrawingStyle(QgsRasterLayer::MULTI_BAND_SINGLE_BAND_GRAY);

      }
    }
  }                           //end of grayscale box enabled and rbtnsingleband checked
  else                          //assume that rbtnThreeBand is checked and render in rgb color
  {
    //set the grayscale color table type if the groupbox is enabled

    if (rasterLayer->rasterLayerType == QgsRasterLayer::PALETTE)
    {
#ifdef QGISDEBUG
      std::cout << "Setting Raster Drawing Style to :: PALETTED_MULTI_BAND_COLOR" << std::endl;
#endif

      rasterLayer->setDrawingStyle(QgsRasterLayer::PALETTED_MULTI_BAND_COLOR);
    }
    else if (rasterLayer->rasterLayerType == QgsRasterLayer::MULTIBAND)
    {

#ifdef QGISDEBUG
      std::cout << "Setting Raster Drawing Style to :: MULTI_BAND_COLOR" << std::endl;
#endif

      rasterLayer->setDrawingStyle(QgsRasterLayer::MULTI_BAND_COLOR);
    }

  }

  //set whether the layer histogram should be inverted
  if (cboxInvertColorMap->isChecked())
  {
    rasterLayer->setInvertHistogramFlag(true);
  }
  else
  {
    rasterLayer->setInvertHistogramFlag(false);
  }

  //set transparency
  rasterLayer->setTransparency(static_cast < unsigned int >(255 - sliderTransparency->value()));

  //now set the color -> band mapping combos to the correct values
  rasterLayer->setRedBandName(cboRed->currentText());
  rasterLayer->setGreenBandName(cboGreen->currentText());
  rasterLayer->setBlueBandName(cboBlue->currentText());
  rasterLayer->setGrayBandName(cboGray->currentText());
  rasterLayer->setTransparentBandName(cboTransparent->currentText());

  //set the appropriate color ramping type
  if (cboColorMap->currentText() == tr("Pseudocolor"))
  {
    rasterLayer->setColorRampingType(QgsRasterLayer::BLUE_GREEN_RED);  
  }
  else if (cboColorMap->currentText() == tr("Freak Out"))
  {
    rasterLayer->setColorRampingType(QgsRasterLayer::FREAK_OUT);  
  }

  //set the std deviations to be plotted and check for user defined Min Max values
  if(rbtnThreeBand->isChecked())
  {
    rasterLayer->setStdDevsToPlot(sboxStdDevThreeBand->value());
    //Set min max based on user defined values if all are set and stdDev is 0.0
    if(0.0 == rasterLayer->getStdDevsToPlot() && validUserDefinedMinMax())
    {
      rasterLayer->setMinRedDouble(leRedMin->text().toDouble());
      rasterLayer->setMaxRedDouble(leRedMax->text().toDouble());
      rasterLayer->setMinGreenDouble(leGreenMin->text().toDouble());
      rasterLayer->setMaxGreenDouble(leGreenMax->text().toDouble());
      rasterLayer->setMinBlueDouble(leBlueMin->text().toDouble());
      rasterLayer->setMaxBlueDouble(leBlueMax->text().toDouble());
      rasterLayer->setUserDefinedColorMinMax(true);
    }
    else
    {
      rasterLayer->setUserDefinedColorMinMax(false);
    }
  }
  else
  {
    rasterLayer->setStdDevsToPlot(sboxStdDevSingleBand->value());
    //Set min max based on user defined values if all are set and stdDev is 0.0
    if(0.0 == rasterLayer->getStdDevsToPlot() && validUserDefinedMinMax())
    {
      rasterLayer->setMinGrayDouble(leGrayMin->text().toDouble());
      rasterLayer->setMaxGrayDouble(leGrayMax->text().toDouble());
      rasterLayer->setUserDefinedGrayMinMax(true);
    }
    else
    {
      rasterLayer->setUserDefinedGrayMinMax(false);
    }
  }

  //set the color scaling algorithm
  if(cboxColorScalingAlgorithm->currentText() == tr("Stretch To MinMax"))
  {
    rasterLayer->setColorScalingAlgorithm(QgsRasterLayer::STRETCH_TO_MINMAX);
  }
  else if(cboxColorScalingAlgorithm->currentText() == tr("Stretch And Clip To MinMax"))
  {
    rasterLayer->setColorScalingAlgorithm(QgsRasterLayer::STRETCH_AND_CLIP_TO_MINMAX);
  }
  else if(cboxColorScalingAlgorithm->currentText() == tr("Clip To MinMax"))
  {
    rasterLayer->setColorScalingAlgorithm(QgsRasterLayer::CLIP_TO_MINMAX);
  }
  else
  {
    rasterLayer->setColorScalingAlgorithm(QgsRasterLayer::NO_STRETCH);
  }

  /*
   * Transparent Pixel Tab
   */
  //set NoDataValue
  bool myDoubleOk;
  double myNoDataValue = leNoDataValue->text().toDouble(&myDoubleOk);
  if(myDoubleOk)
  {
    rasterLayer->setNoDataValue(myNoDataValue);
  }
  else
  {
    //TODO:check to see if band has stats and get noDataValue from there.
    leNoDataValue->setText("-9999.0");
    rasterLayer->setNoDataValue(-9999.0);
  }

  //Walk through each row in table and test value. If not valid set to 0.0 and continue building transparency list
  if(rbtnThreeBand->isChecked())
  {
    double myTransparentValueDouble;
    QgsRasterLayer::TransparentColorPixel myTransparentPixel;
    QList<QgsRasterLayer::TransparentColorPixel> myTransparentColorPixelList;
    for(int myListRunner = 0; myListRunner < tableTransparency->rowCount(); myListRunner++)
    {
      if(!tableTransparency->item(myListRunner, 0))
      {
        myTransparentPixel.red = 0.0;
        QTableWidgetItem* newItem = new QTableWidgetItem("0.0");
        tableTransparency->setItem(myListRunner, 0, newItem);
      }
      else 
      {
        myTransparentValueDouble = tableTransparency->item(myListRunner, 0)->text().toDouble(&myDoubleOk);
        if(myDoubleOk)
        {
          myTransparentPixel.red = myTransparentValueDouble;
        }
        else 
        {
        myTransparentPixel.red = 0.0;
        tableTransparency->item(myListRunner, 0)->setText("0.0");
        }
      }

      if(!tableTransparency->item(myListRunner, 1))
      {
        myTransparentPixel.green = 0.0;
        QTableWidgetItem* newItem = new QTableWidgetItem("0.0");
        tableTransparency->setItem(myListRunner, 1, newItem);
      }
      else 
      {
        myTransparentValueDouble = tableTransparency->item(myListRunner, 1)->text().toDouble(&myDoubleOk);
        if(myDoubleOk)
        {
          myTransparentPixel.green = myTransparentValueDouble;
        }
        else 
        {
         myTransparentPixel.green = 0.0;
         tableTransparency->item(myListRunner, 1)->setText("0.0");
        }
      }

      if(!tableTransparency->item(myListRunner, 2))
      {
        myTransparentPixel.blue = 0.0;
        QTableWidgetItem* newItem = new QTableWidgetItem("0.0");
        tableTransparency->setItem(myListRunner, 2, newItem);
      }
      else 
      {
        myTransparentValueDouble = tableTransparency->item(myListRunner, 2)->text().toDouble(&myDoubleOk);
        if(myDoubleOk)
        {
          myTransparentPixel.blue = myTransparentValueDouble;
        }
        else 
        {
         myTransparentPixel.blue = 0.0;
         tableTransparency->item(myListRunner, 2)->setText("0.0");
        }
      }
      myTransparentColorPixelList.append(myTransparentPixel);
    }

    rasterLayer->setTransparentColorPixelList(myTransparentColorPixelList);
  }
  else
  {
    double myTransparentValueDouble;
    double myTransparentPixel;
    QList<double> myTransparentGrayPixelList;
    for(int myListRunner = 0; myListRunner < tableTransparency->rowCount(); myListRunner++)
    {
      if(!tableTransparency->item(myListRunner, 0))
      {
        myTransparentPixel = 0.0;
        QTableWidgetItem* newItem = new QTableWidgetItem("0.0");
        tableTransparency->setItem(myListRunner, 0, newItem);
      }
      else 
      {
        myTransparentValueDouble = tableTransparency->item(myListRunner, 0)->text().toDouble(&myDoubleOk);
        if(myDoubleOk)
        {
          myTransparentPixel = myTransparentValueDouble;
        }
        else 
        {
          myTransparentPixel = 0.0;
          tableTransparency->item(myListRunner, 0)->setText("0.0");
        }
      }
      myTransparentGrayPixelList.append(myTransparentPixel);
    }

    rasterLayer->setTransparentGrayPixelList(myTransparentGrayPixelList);
  }

  

  /*
   * General Tab
   */
  rasterLayer->setLayerName(leDisplayName->text());

  //see if the user would like debug overlays
  if (cboxShowDebugInfo->isChecked() == true)
  {
    rasterLayer->setShowDebugOverlayFlag(true);
  }
  else
  {
    rasterLayer->setShowDebugOverlayFlag(false);
  }

  // set up the scale based layer visibility stuff....
  rasterLayer->setScaleBasedVisibility(chkUseScaleDependentRendering->isChecked());
  rasterLayer->setMinScale(spinMinimumScale->value());
  rasterLayer->setMaxScale(spinMaximumScale->value());

  //update the legend pixmap
  pixmapLegend->setPixmap(rasterLayer->getLegendQPixmap());
  pixmapLegend->setScaledContents(true);
  pixmapLegend->repaint(false);

  //get the thumbnail for the layer
  QPixmap myQPixmap = QPixmap(pixmapThumbnail->width(),pixmapThumbnail->height());
  rasterLayer->drawThumbnail(&myQPixmap);
  pixmapThumbnail->setPixmap(myQPixmap);

  // update symbology
  emit refreshLegend(rasterLayer->getLayerID());

  //make sure the layer is redrawn
  rasterLayer->triggerRepaint();

  //Becuase Min Max values can be set during the redraw if a strech is requested we need to resync after apply
  if(QgsRasterLayer::NO_STRETCH != rasterLayer->getColorScalingAlgorithm())
  {
    //sync();

    //set the stdDevs and min max values
    if(rbtnThreeBand->isChecked())
    {
      sboxStdDevThreeBand->setValue(rasterLayer->getStdDevsToPlot());
      sboxStdDevSingleBand->setValue(0.0);
      leRedMin->setText(QString::number(rasterLayer->getMinRedDouble()));
      leRedMax->setText(QString::number(rasterLayer->getMaxRedDouble()));
      leGreenMin->setText(QString::number(rasterLayer->getMinGreenDouble()));
      leGreenMax->setText(QString::number(rasterLayer->getMaxGreenDouble()));
      leBlueMin->setText(QString::number(rasterLayer->getMinBlueDouble()));
      leBlueMax->setText(QString::number(rasterLayer->getMaxBlueDouble()));
    }
    else
    {
      sboxStdDevThreeBand->setValue(0.0);
      sboxStdDevSingleBand->setValue(rasterLayer->getStdDevsToPlot());
      leGrayMin->setText(QString::number(rasterLayer->getMinGrayDouble()));
      leGrayMax->setText(QString::number(rasterLayer->getMaxGrayDouble()));
    }
  }
}//apply

/**
  @note moved from ctor

  Previously this dialog was created anew with each right-click pop-up menu
  invokation.  Changed so that the dialog always exists after first
  invocation, and is just re-synchronized with its layer's state when
  re-shown.

*/
void QgsRasterLayerProperties::sync()
{
  cboxShowDebugInfo->hide();

  /*
   * Symbology Tab
   */
  //decide whether user can change rgb settings
  switch (rasterLayer->getDrawingStyle())
  {
      case QgsRasterLayer::SINGLE_BAND_GRAY:
          rbtnThreeBand->setEnabled(false);
          rbtnSingleBand->setEnabled(true);
          rbtnSingleBand->toggle();
          break;
      case QgsRasterLayer::SINGLE_BAND_PSEUDO_COLOR:
          rbtnThreeBand->setEnabled(false);
          rbtnSingleBand->setEnabled(true);
          rbtnSingleBand->toggle();
          break;
      case QgsRasterLayer::PALETTED_SINGLE_BAND_GRAY:
          rbtnThreeBand->setEnabled(true);
          rbtnSingleBand->setEnabled(true);
          rbtnSingleBand->toggle();
          break;
      case QgsRasterLayer::PALETTED_SINGLE_BAND_PSEUDO_COLOR:
          rbtnThreeBand->setEnabled(true);
          rbtnSingleBand->setEnabled(true);
          rbtnSingleBand->toggle();
          break;
      case QgsRasterLayer::PALETTED_MULTI_BAND_COLOR:
          rbtnThreeBand->setEnabled(true);
          rbtnSingleBand->setEnabled(true);
          rbtnThreeBand->toggle();
          break;
      case QgsRasterLayer::MULTI_BAND_SINGLE_BAND_GRAY:
          rbtnThreeBand->setEnabled(true);
          rbtnSingleBand->setEnabled(true);
          rbtnSingleBand->toggle();
          break;
      case QgsRasterLayer::MULTI_BAND_SINGLE_BAND_PSEUDO_COLOR:
          rbtnThreeBand->setEnabled(true);
          rbtnSingleBand->setEnabled(true);
          rbtnSingleBand->toggle();
          break;
      case QgsRasterLayer::MULTI_BAND_COLOR:
          rbtnThreeBand->setEnabled(true);
          rbtnSingleBand->setEnabled(true);
          rbtnThreeBand->toggle();
          break;
      default:
          break;
  }

  if (rasterLayer->getRasterLayerType() == QgsRasterLayer::MULTIBAND)
  {
    //multiband images can also be rendered as single band (using only one of the bands)
    txtSymologyNotes->
        setText(tr
                ("<h3>Multiband Image Notes</h3><p>This is a multiband image. You can choose to render it as grayscale or color (RGB). For color images, you can associate bands to colors arbitarily. For example, if you have a seven band landsat image, you may choose to render it as:</p><ul><li>Visible Blue (0.45 to 0.52 microns) - not mapped</li><li>Visible Green (0.52 to 0.60 microns) - not mapped</li></li>Visible Red (0.63 to 0.69 microns) - mapped to red in image</li><li>Near Infrared (0.76 to 0.90 microns) - mapped to green in image</li><li>Mid Infrared (1.55 to 1.75 microns) - not mapped</li><li>Thermal Infrared (10.4 to 12.5 microns) - not mapped</li><li>Mid Infrared (2.08 to 2.35 microns) - mapped to blue in image</li></ul>"));
  }
  else if (rasterLayer->getRasterLayerType() == QgsRasterLayer::PALETTE)
  {
    //paletted images (e.g. tif) can only be rendered as three band rgb images
    txtSymologyNotes->
        setText(tr
                ("<h3>Paletted Image Notes</h3> <p>This image uses a fixed color palette. You can remap these colors in different combinations e.g.</p><ul><li>Red - blue in image</li><li>Green - blue in image</li><li>Blue - green in image</li></ul>"));
  }
  else                        //only grayscale settings allowed
  {
    //grayscale images can only be rendered as singleband
    txtSymologyNotes->
        setText(tr
                ("<h3>Grayscale Image Notes</h3> <p>You can remap these grayscale colors to a pseudocolor image using an automatically generated color ramp.</p>"));
  }

  //
  // Populate the various controls on the form
  //
  if (rasterLayer->getDrawingStyle() == QgsRasterLayer::SINGLE_BAND_PSEUDO_COLOR ||
          rasterLayer->getDrawingStyle() == QgsRasterLayer::PALETTED_SINGLE_BAND_PSEUDO_COLOR ||
          rasterLayer->getDrawingStyle() == QgsRasterLayer::MULTI_BAND_SINGLE_BAND_PSEUDO_COLOR)
  {
    if(rasterLayer->getColorRampingType()==QgsRasterLayer::BLUE_GREEN_RED)
    {
      cboColorMap->setCurrentText(tr("Pseudocolor"));
    }
    else
    {
      cboColorMap->setCurrentText(tr("Freak Out"));
    }

  }
  else
  {
    cboColorMap->setCurrentText(tr("Grayscale"));
  }

  //set whether the layer histogram should be inverted
  if (rasterLayer->getInvertHistogramFlag())
  {
    cboxInvertColorMap->setChecked(true);
  }
  else
  {
    cboxInvertColorMap->setChecked(false);
  }

  //set the transparency slider
  sliderTransparency->setValue(255 - rasterLayer->getTransparency());
  //update the transparency percentage label
  sliderTransparency_valueChanged(255 - rasterLayer->getTransparency());

  //set the stdDevs and min max values
  if(rbtnThreeBand->isChecked())
  {
    sboxStdDevThreeBand->setValue(rasterLayer->getStdDevsToPlot());
    sboxStdDevSingleBand->setValue(0.0);
    leRedMin->setText(QString::number(rasterLayer->getMinRedDouble()));
    leRedMax->setText(QString::number(rasterLayer->getMaxRedDouble()));
    leGreenMin->setText(QString::number(rasterLayer->getMinGreenDouble()));
    leGreenMax->setText(QString::number(rasterLayer->getMaxGreenDouble()));
    leBlueMin->setText(QString::number(rasterLayer->getMinBlueDouble()));
    leBlueMax->setText(QString::number(rasterLayer->getMaxBlueDouble()));
  }
  else
  {
    sboxStdDevThreeBand->setValue(0.0);
    sboxStdDevSingleBand->setValue(rasterLayer->getStdDevsToPlot());
    leGrayMin->setText(QString::number(rasterLayer->getMinGrayDouble()));
    leGrayMax->setText(QString::number(rasterLayer->getMaxGrayDouble()));
  }

  //now set the combos to the correct values
  cboRed->setCurrentText(rasterLayer->getRedBandName());
  cboGreen->setCurrentText(rasterLayer->getGreenBandName());
  cboBlue->setCurrentText(rasterLayer->getBlueBandName());
  cboGray->setCurrentText(rasterLayer->getGrayBandName());
  cboTransparent->setCurrentText(rasterLayer->getTransparentBandName());

  //set color scaling algorithm
  if(QgsRasterLayer::STRETCH_TO_MINMAX == rasterLayer->getColorScalingAlgorithm())
  {
    cboxColorScalingAlgorithm->setCurrentText(tr("Stretch To MinMax"));
  }
  else if(QgsRasterLayer::STRETCH_AND_CLIP_TO_MINMAX == rasterLayer->getColorScalingAlgorithm())
  {
    cboxColorScalingAlgorithm->setCurrentText(tr("Stretch And Clip To MinMax"));
  }
  else if(QgsRasterLayer::CLIP_TO_MINMAX == rasterLayer->getColorScalingAlgorithm())
  {
    cboxColorScalingAlgorithm->setCurrentText(tr("Clip To MinMax"));
  }
  else
  {
    cboxColorScalingAlgorithm->setCurrentText(tr("No Scaling"));
  }

  /*
   * Transparent Pixel Tab
   */
  //add current NoDataValue to NoDataValue line edit 
  leNoDataValue->setText(QString::number(rasterLayer->getNoDataValue()));

  //Clear existsing color transparency list
  //NOTE: May want to just tableTransparency->clearContents() and fill back in after checking to be sure list and table are the same size
  for(int myTableRunner = tableTransparency->rowCount() - 1; myTableRunner >= 0; myTableRunner--)
  {
    tableTransparency->removeRow(myTableRunner);
  }

  //populate color transparency list
  QList<QgsRasterLayer::TransparentColorPixel> myTransparentColorPixelList = rasterLayer->getTransparentColorPixelList();
  for(int myListRunner = 0; myListRunner < myTransparentColorPixelList.count(); myListRunner++)
  {
    tableTransparency->insertRow(myListRunner);
    QTableWidgetItem* myRedItem = new QTableWidgetItem(QString::number(myTransparentColorPixelList[myListRunner].red));
    QTableWidgetItem* myGreenItem = new QTableWidgetItem(QString::number(myTransparentColorPixelList[myListRunner].green));
    QTableWidgetItem* myBlueItem = new QTableWidgetItem(QString::number(myTransparentColorPixelList[myListRunner].blue));

    tableTransparency->setItem(myListRunner, 0, myRedItem);
    tableTransparency->setItem(myListRunner, 1, myGreenItem);
    tableTransparency->setItem(myListRunner, 2, myBlueItem);
  }

  //Clear existing gray gransparency list
  for(int myTableRunner = tableTransparency->rowCount() - 1; myTableRunner >=0; myTableRunner--)
  {
    tableTransparency->removeRow(myTableRunner);
  }

  //populate gray transparency list
  QList<double> myTransparentGrayPixelList = rasterLayer->getTransparentGrayPixelList();
  for(int myListRunner = 0; myListRunner < myTransparentGrayPixelList.count(); myListRunner++)
  {
    tableTransparency->insertRow(myListRunner);
    QTableWidgetItem* myGrayItem = new QTableWidgetItem(QString::number(myTransparentGrayPixelList[myListRunner]));

    tableTransparency->setItem(myListRunner, 0, myGrayItem);
  }

  /*
   * General Tab
   */
  cboxShowDebugInfo->hide();

  //these properties (layername and label) are provided by the qgsmaplayer superclass
  leLayerSource->setText(rasterLayer->source());
  leDisplayName->setText(rasterLayer->name());

  //update the debug checkbox
  cboxShowDebugInfo->setChecked(rasterLayer->getShowDebugOverlayFlag());

  //display the raster dimensions and no data value
  if (rasterLayerIsGdal)
  {
    lblColumns->setText(tr("Columns: ") + QString::number(rasterLayer->getRasterXDim()));
    lblRows->setText(tr("Rows: ") + QString::number(rasterLayer->getRasterYDim()));
    lblNoData->setText(tr("No-Data Value: ") + QString::number(rasterLayer->getNoDataValue()));
  }
  else if (rasterLayerIsWms)
  {
    // TODO: Account for fixedWidth and fixedHeight WMS layers
    lblColumns->setText(tr("Columns: ") + tr("n/a"));
    lblRows->setText(tr("Rows: ") + tr("n/a"));
    lblNoData->setText(tr("No-Data Value: ") + tr("n/a"));
  }

  //get the thumbnail for the layer
  QPixmap myQPixmap = QPixmap(pixmapThumbnail->width(),pixmapThumbnail->height());
  rasterLayer->drawThumbnail(&myQPixmap);
  pixmapThumbnail->setPixmap(myQPixmap);

  //update the legend pixmap on this dialog
  pixmapLegend->setPixmap(rasterLayer->getLegendQPixmap());
  pixmapLegend->setScaledContents(true);
  pixmapLegend->repaint(false);

  //set the palette pixmap
  pixmapPalette->setPixmap(rasterLayer->getPaletteAsPixmap());
  pixmapPalette->setScaledContents(true);
  pixmapPalette->repaint(false);

  /*
   * Metadata Tab
   */
  //populate the metadata tab's text browser widget with gdal metadata info
  txtbMetadata->setText(rasterLayer->getMetadata());

} // QgsRasterLayerProperties::sync()

bool QgsRasterLayerProperties::validUserDefinedMinMax()
{
  if(rbtnThreeBand->isChecked())
  {
    bool myDoubleOk;
    leRedMin->text().toDouble(&myDoubleOk);
    if(myDoubleOk)
    {
      leRedMax->text().toDouble(&myDoubleOk);
      if(myDoubleOk) 
      {
        leGreenMin->text().toDouble(&myDoubleOk);
        if(myDoubleOk)
        {
          leGreenMax->text().toDouble(&myDoubleOk);
          if(myDoubleOk)
          {
            leBlueMin->text().toDouble(&myDoubleOk);
            if(myDoubleOk)
            {
              leBlueMax->text().toDouble(&myDoubleOk);
              if(myDoubleOk)
              {
                return true;
              }
            }
          }
        }
      }
    }
  }
  else
  {
    bool myDoubleOk;
    leGrayMin->text().toDouble(&myDoubleOk);
    if(myDoubleOk)
    {
      leGrayMax->text().toDouble(&myDoubleOk);
      if(myDoubleOk)
      {
        return true;
      }
    }
  }
  
  return false;
}


/*
 *
 * PUBLIC AND PRIVATE SLOTS
 *
 */
void QgsRasterLayerProperties::on_buttonBox_helpRequested()
{
  QgsContextHelp::run(context_id);
}

void QgsRasterLayerProperties::on_buttonBuildPyramids_clicked()
{

  //
  // Go through the list marking any files that are selected in the listview
  // as true so that we can generate pyramids for them.
  //
  QgsRasterLayer::RasterPyramidList myPyramidList = rasterLayer->buildRasterPyramidList();
  for ( unsigned int myCounterInt = 0; myCounterInt < lbxPyramidResolutions->count(); myCounterInt++ )
  {
    Q3ListBoxItem *myItem = lbxPyramidResolutions->item( myCounterInt );
    if ( myItem->isSelected() )
    {
      //mark to be pyramided
      myPyramidList[myCounterInt].existsFlag=true;
    }
  }
  //
  // Ask raster layer to build the pyramids
  //
  
  // let the user know we're going to possibly be taking a while
  QApplication::setOverrideCursor(Qt::WaitCursor);
  QString res = rasterLayer->buildPyramids(myPyramidList,cboResamplingMethod->currentText());
  QApplication::restoreOverrideCursor();
  if (!res.isNull())
  {
    if (res == "ERROR_WRITE_ACCESS")
    {
      QMessageBox::warning(this, tr("Write access denied"),
                           tr("Write access denied. Adjust the file permissions and try again.\n\n") );
    }
    else if (res == "ERROR_WRITE_FORMAT")
    {
      QMessageBox::warning(this, tr("Building pyramids failed."),
                           tr("The file was not writeable. Some formats can not be written to, only read. You can also try to check the permissions and then try again.") );
    }
    else if (res == "FAILED_NOT_SUPPORTED")
    {
      QMessageBox::warning(this, tr("Building pyramids failed."),
                           tr("Building pyramid overviews is not supported on this type of raster.") );
    }
  }

  
  //
  // repopulate the pyramids list
  //
  lbxPyramidResolutions->clear();
  QString myThemePath = QgsApplication::themePath();
  QPixmap myPyramidPixmap(myThemePath + "/mIconPyramid.png");
  QPixmap myNoPyramidPixmap(myThemePath + "/mIconNoPyramid.png");

  QgsRasterLayer::RasterPyramidList::iterator myRasterPyramidIterator;
  for ( myRasterPyramidIterator=myPyramidList.begin();
          myRasterPyramidIterator != myPyramidList.end();
          ++myRasterPyramidIterator )
  {
    if ((*myRasterPyramidIterator).existsFlag==true)
    {
      lbxPyramidResolutions->insertItem(myPyramidPixmap,
              QString::number((*myRasterPyramidIterator).xDimInt) + QString(" x ") + 
              QString::number((*myRasterPyramidIterator).yDimInt)); 
    }
    else
    {
      lbxPyramidResolutions->insertItem(myNoPyramidPixmap,
              QString::number((*myRasterPyramidIterator).xDimInt) + QString(" x ") + 
              QString::number((*myRasterPyramidIterator).yDimInt)); 
    }
  }
  //update the legend pixmap
  pixmapLegend->setPixmap(rasterLayer->getLegendQPixmap());
  pixmapLegend->setScaledContents(true);
  pixmapLegend->repaint(false);
  //populate the metadata tab's text browser widget with gdal metadata info
  txtbMetadata->setText(rasterLayer->getMetadata());
}

void QgsRasterLayerProperties::on_pbnAddValuesFromDisplay_clicked()
{
  QMessageBox::warning(this, "Function Not Available", "This functionality will be added soon");
}

void QgsRasterLayerProperties::on_pbnAddValuesManually_clicked()
{
  if(rbtnThreeBand->isChecked())
  {
    tableTransparency->insertRow(tableTransparency->rowCount());
  }
  else
  {
    tableTransparency->insertRow(tableTransparency->rowCount());
  }
}

void QgsRasterLayerProperties::on_pbnChangeSpatialRefSys_clicked()
{

    QgsLayerProjectionSelector * mySelector = new QgsLayerProjectionSelector(this);
    mySelector->setSelectedSRSID(rasterLayer->srs().srsid());
    if(mySelector->exec())
    {
      QgsSpatialRefSys srs(mySelector->getCurrentSRSID(), QgsSpatialRefSys::QGIS_SRSID);
      rasterLayer->setSrs(srs);
    }
    else
    {
      QApplication::restoreOverrideCursor();
    }
    delete mySelector;

    leSpatialRefSys->setText(rasterLayer->srs().proj4String());
}

void QgsRasterLayerProperties::on_pbnDefaultValues_clicked()
{
  rasterLayer->resetNoDataValue();

  //Clear existsing color transparency list
  for(int myTableRunner = tableTransparency->rowCount() - 1; myTableRunner > 0; myTableRunner--)
  {
    tableTransparency->removeRow(myTableRunner);
  }

  tableTransparency->item(0, 0)->setText(QString::number(rasterLayer->getNoDataValue()));
  tableTransparency->item(0, 1)->setText(QString::number(rasterLayer->getNoDataValue()));
  tableTransparency->item(0, 2)->setText(QString::number(rasterLayer->getNoDataValue()));

  //Clear existing gray gransparency list
  for(int myTableRunner = tableTransparency->rowCount() - 1; myTableRunner > 0; myTableRunner--)
  {
    tableTransparency->removeRow(myTableRunner);
  }

  tableTransparency->item(0, 0)->setText(QString::number(rasterLayer->getNoDataValue()));

  leNoDataValue->setText(QString::number(rasterLayer->getNoDataValue()));
}

void QgsRasterLayerProperties::on_pbnExportTransparentPixelValues_clicked()
{
  QString myFilename = QFileDialog::getSaveFileName(this, tr("Save file"), "/", tr("Textfile (*.txt)"));
  if(!myFilename.isEmpty())
  {
    if(!myFilename.endsWith(".txt", Qt::CaseInsensitive))
    {
      myFilename = myFilename + ".txt";
   }

   QFile myOutputFile(myFilename);
   if (myOutputFile.open(QFile::WriteOnly))
   {
     QTextStream myOutputStream(&myOutputFile);
     myOutputStream << "# " << tr("QGIS Generated Transparent Pixel Value Export File") << "\n";
     if(rbtnThreeBand->isChecked())
      {
       myOutputStream << "#\n#\n# " << tr("Red") << "\t" << tr("Green") << "\t" << tr("Blue");
       for(int myTableRunner = 0; myTableRunner < tableTransparency->rowCount(); myTableRunner++)
       {
         myOutputStream << "\n" << tableTransparency->item(myTableRunner, 0)->text() << "\t" << tableTransparency->item(myTableRunner, 1)->text() << "\t" << tableTransparency->item(myTableRunner, 2)->text();
       }
     }
     else
     {
       myOutputStream << "#\n#\n# " << tr("Gray");
       for(int myTableRunner = 0; myTableRunner < tableTransparency->rowCount(); myTableRunner++)
       {
         myOutputStream << "\n" << tableTransparency->item(myTableRunner, 0)->text();
       }
      }
    }
    else
    {
      QMessageBox::warning(this, tr("Write access denied"), tr("Write access denied. Adjust the file permissions and try again.\n\n") );
    }
  }
}

void QgsRasterLayerProperties::on_pbnHistRefresh_clicked()
{
#ifdef QGISDEBUG
  std::cout << "QgsRasterLayerProperties::on_pbnHistRefresh_clicked" << std::endl;
#endif
  int myBandCountInt = rasterLayer->getBandCount();
  //
  // Find out how many bands are selected and short circuit out clearing the image
  // if needed
  int mySelectionCount=0;
  for (int myIteratorInt = 1;
          myIteratorInt <= myBandCountInt;
          ++myIteratorInt)
  {
    QgsRasterBandStats myRasterBandStats = rasterLayer->getRasterBandStats(myIteratorInt);
    Q3ListBoxItem *myItem = lstHistogramLabels->item( myIteratorInt-1 );
    if ( myItem->isSelected() )
    {
      mySelectionCount++;
    }
  }
  if (mySelectionCount==0)
  {
    int myImageWidth = pixHistogram->width();
    int myImageHeight =  pixHistogram->height();
    QPixmap myPixmap(myImageWidth,myImageHeight);
    myPixmap.fill(Qt::white);
    pixHistogram->setPixmap(myPixmap);
  }

  // Explanation:
  // We use the gdal histogram creation routine is called for each selected  
  // layer. Currently the hist is hardcoded
  // to create 256 bins. Each bin stores the total number of cells that 
  // fit into the range defined by that bin.
  //
  // The graph routine below determines the greatest number of pixesl in any given 
  // bin in all selected layers, and the min. It then draws a scaled line between min 
  // and max - scaled to image height. 1 line drawn per selected band
  //
  const int BINCOUNT = spinHistBinCount->value();
  enum GRAPH_TYPE { BAR_CHART, LINE_CHART } myGraphType;
  if (radHistTypeBar->isOn()) myGraphType=BAR_CHART; else myGraphType=LINE_CHART;
  bool myIgnoreOutOfRangeFlag = chkHistIgnoreOutOfRange->isChecked();
  bool myThoroughBandScanFlag = chkHistAllowApproximation->isChecked();

#ifdef QGISDEBUG
  long myCellCount = rasterLayer->getRasterXDim() * rasterLayer->getRasterYDim();
#endif

#ifdef QGISDEBUG
  std::cout << "Computing histogram minima and maxima" << std::endl;
#endif
  //somtimes there are more bins than needed
  //we find out the last on that actully has data in it
  //so we can discard the rest adn the x-axis scales correctly
  int myLastBinWithData=0;
  //
  // First scan through to get max and min cell counts from among selected layers' histograms
  //
  double myYAxisMax=0;
  double myYAxisMin=0;
  int myXAxisMin=0;
  int myXAxisMax=0;
  for (int myIteratorInt = 1;
          myIteratorInt <= myBandCountInt;
          ++myIteratorInt)
  {
    QgsRasterBandStats myRasterBandStats = rasterLayer->getRasterBandStats(myIteratorInt);
    //calculate the x axis min max
    if (myRasterBandStats.minValDouble < myXAxisMin || myIteratorInt==1)
    {
      myXAxisMin=static_cast < unsigned int >(myRasterBandStats.minValDouble);
    }
    if (myRasterBandStats.maxValDouble < myXAxisMax || myIteratorInt==1)
    {
      myXAxisMax=static_cast < unsigned int >(myRasterBandStats.maxValDouble);
    }
    Q3ListBoxItem *myItem = lstHistogramLabels->item( myIteratorInt-1 );
    if ( myItem->isSelected() )
    {
#ifdef QGISDEBUG
      std::cout << "Ensuring hist is populated for this layer" << std::endl;
#endif
      rasterLayer->populateHistogram(myIteratorInt,BINCOUNT,myIgnoreOutOfRangeFlag,myThoroughBandScanFlag); 

#ifdef QGISDEBUG
      std::cout << "...done..." << myRasterBandStats.histogramVector->size() << " bins filled" << std::endl;
#endif
      for (int myBin = 0; myBin <BINCOUNT; myBin++)
      {
        int myBinValue = myRasterBandStats.histogramVector->at(myBin);
        if (myBinValue > 0 && myBin > myLastBinWithData)
        {
          myLastBinWithData = myBin;
        }
#ifdef QGISDEBUG
        std::cout << "Testing if " << myBinValue << " is less than " << myYAxisMin  << "or greater then " <<myYAxisMax  <<  std::endl;
#endif
        if ( myBin==0)
        {
          myYAxisMin = myBinValue;
          myYAxisMax = myBinValue;
        }

        if (myBinValue  > myYAxisMax)
        {
          myYAxisMax = myBinValue;
        }
        if ( myBinValue < myYAxisMin)
        {
          myYAxisMin = myBinValue;
        }
      }
    }
  }
#ifdef QGISDEBUG
  std::cout << "max " << myYAxisMax << std::endl;
  std::cout << "min " << myYAxisMin << std::endl;
#endif


  //create the image onto which graph and axes will be drawn
  int myImageWidth = pixHistogram->width();
  int myImageHeight =  pixHistogram->height();
  QPixmap myPixmap(myImageWidth,myImageHeight);
  myPixmap.fill(Qt::white);

  // TODO: Confirm that removing the "const QWidget * copyAttributes" 2nd parameter,
  // in order to make things work in Qt4, doesn't break things in Qt3.
  //QPainter myPainter(&myPixmap, this);
  QPainter myPainter(&myPixmap);

  //determine labels sizes and draw them
  QFont myQFont("arial", 8, QFont::Normal);
  QFontMetrics myFontMetrics( myQFont );
  myPainter.setFont(myQFont);
  myPainter.setPen(Qt::black);
  QString myYMaxLabel= QString::number(static_cast < unsigned int >(myYAxisMax));
  QString myXMinLabel= QString::number(myXAxisMin);
  QString myXMaxLabel= QString::number(myXAxisMax);
  //calculate the gutters
  int myYGutterWidth=0;
  if (myFontMetrics.width(myXMinLabel) < myFontMetrics.width(myYMaxLabel))
  {
    myYGutterWidth = myFontMetrics.width(myYMaxLabel )+2; //add 2 so we can have 1 pix whitespace either side of label
  }
  else
  {
    myYGutterWidth = myFontMetrics.width(myXMinLabel )+2; //add 2 so we can have 1 pix whitespace either side of label
  }
  int myXGutterHeight = myFontMetrics.height()+2;
  int myXGutterWidth = myFontMetrics.width(myXMaxLabel)+1;//1 pix whtispace from right edge of image

  //
  // Now calculate the graph drawable area after the axis labels have been taken
  // into account
  //
  int myGraphImageWidth =myImageWidth-myYGutterWidth; 
  int myGraphImageHeight = myImageHeight-myXGutterHeight; 

  //find out how wide to draw bars when in bar chart mode
  int myBarWidth = static_cast<int>((((double)myGraphImageWidth)/((double)BINCOUNT)));


  //
  //now draw actual graphs
  //
  if (rasterLayer->getRasterLayerType()
          == QgsRasterLayer::PALETTE) //paletted layers have hard coded color entries
  {
    QPolygon myPointArray(myLastBinWithData);
    QgsColorTable *myColorTable=rasterLayer->colorTable(1);
#ifdef QGISDEBUG
    std::cout << "Making paletted image histogram....computing band stats" << std::endl;
    std::cout << "myLastBinWithData = " << myLastBinWithData << std::endl;
#endif

    QgsRasterBandStats myRasterBandStats = rasterLayer->getRasterBandStats(1);
    for (int myBin = 0; myBin < myLastBinWithData; myBin++)
    {
      double myBinValue = myRasterBandStats.histogramVector->at(myBin);
#ifdef QGISDEBUG
      std::cout << "myBin = " << myBin << " myBinValue = " << myBinValue << std::endl;
#endif
      //NOTE: Int division is 0 if the numerator is smaller than the denominator.
      //hence the casts
      int myX = static_cast<int>((((double)myGraphImageWidth)/((double)myLastBinWithData))*myBin);
      //height varies according to freq. and scaled to greatet value in all layers
      int myY=0;
      if (myYAxisMax!=0)
      {  
        myY=static_cast<int>(((double)myBinValue/(double)myYAxisMax)*myGraphImageHeight);
      }
      
      //see wehter to draw something each loop or to save up drawing for after iteration
      if (myGraphType==BAR_CHART)
      {
	//determin which color to draw the bar
	int c1, c2, c3;
	// Take middle of the interval for color
	// TODO: this is not precise
	double myInterval = (myXAxisMax - myXAxisMin) / myLastBinWithData;
	double myMiddle = myXAxisMin + myBin * myInterval + myInterval/2;

#ifdef QGISDEBUG
	std::cout << "myMiddle = " << myMiddle << std::endl;
#endif
	
	bool found = myColorTable->color ( myMiddle, &c1, &c2, &c3 );
	if ( !found ) {
	    std::cout << "Color not found" << std::endl;
	    c1 = c2 = c3 = 180; // grey
	}
	  
#ifdef QGISDEBUG
	std::cout << "c1 = " << c1 << " c2 = " << c2 << " c3 = " << c3 << std::endl;
#endif
	  
        //draw the bar
        //QBrush myBrush(QColor(c1,c2,c3));
	myPainter.setBrush(QColor(c1,c2,c3));
        myPainter.setPen(QColor(c1,c2,c3));
#ifdef QGISDEBUG
        std::cout << "myX = " << myX << " myY = " << myY << std::endl;
        std::cout << "rect: " << myX+myYGutterWidth << ", " << myImageHeight-(myY+myXGutterHeight)
	          << ", " << myBarWidth << ", " << myY << std::endl;
#endif
        myPainter.drawRect(myX+myYGutterWidth,myImageHeight-(myY+myXGutterHeight),myBarWidth,myY);
      }
      //store this point in our line too
      myY = myGraphImageHeight - myY;
      myPointArray.setPoint(myBin, myX+myYGutterWidth, myY-myXGutterHeight);
    }
    //draw a line on the graph along the bar peaks; 
    if (myGraphType==LINE_CHART)
    {
      myPainter.setPen( Qt::black );
      myPainter.drawPolyline(myPointArray);
    }
  }
  else
  {

    for (int myIteratorInt = 1;
            myIteratorInt <= myBandCountInt;
            ++myIteratorInt)
    {
      QgsRasterBandStats myRasterBandStats = rasterLayer->getRasterBandStats(myIteratorInt);
      Q3ListBoxItem *myItem = lstHistogramLabels->item( myIteratorInt-1 );
      if ( myItem->isSelected() )
      {

        QPolygon myPointArray(myLastBinWithData);
        for (int myBin = 0; myBin <myLastBinWithData; myBin++)
        {
          double myBinValue = myRasterBandStats.histogramVector->at(myBin);
          //NOTE: Int division is 0 if the numerator is smaller than the denominator.
          //hence the casts
          int myX = static_cast<int>((((double)myGraphImageWidth)/((double)myLastBinWithData))*myBin);
          //height varies according to freq. and scaled to greatet value in all layers
          int myY = static_cast<int>(((double)myBinValue/(double)myYAxisMax)*myGraphImageHeight);
          //adjust for image origin being top left
#ifdef QGISDEBUG
          std::cout << "-------------" << std::endl;
          std::cout << "int myY = (myBinValue/myCellCount)*myGraphImageHeight" << std::endl;
          std::cout << "int myY = (" << myBinValue << "/" << myCellCount << ")*" << myGraphImageHeight << std::endl;
          std::cout << "Band " << myIteratorInt << ", bin " << myBin << ", Hist Value : " << myBinValue << ", Scaled Value : " << myY << std::endl;
          std::cout << "myY = myGraphImageHeight - myY" << std::endl;
          std::cout << "myY = " << myGraphImageHeight << "-" << myY << std::endl;
#endif
          if (myGraphType==BAR_CHART)
          {
            //draw the bar
            if (myBandCountInt==1) //draw single band images with black
            {
              myPainter.setPen( Qt::black );
            }
            else if (myIteratorInt==1)
            {
              myPainter.setPen( Qt::red );
            }
            else if (myIteratorInt==2)
            {
              myPainter.setPen( Qt::green );
            }
            else if (myIteratorInt==3)
            {
              myPainter.setPen( Qt::blue );
            }
            else if (myIteratorInt==4)
            {
              myPainter.setPen( Qt::magenta );
            }
            else if (myIteratorInt==5)
            {
              myPainter.setPen( Qt::darkRed );
            }
            else if (myIteratorInt==6)
            {
              myPainter.setPen( Qt::darkGreen );
            }
            else if (myIteratorInt==7)
            {
              myPainter.setPen( Qt::darkBlue );
            }
            else
            {
              myPainter.setPen( Qt::gray );
            }
#ifdef QGISDEBUG
            //  std::cout << "myPainter.fillRect(QRect(" << myX << "," << myY << "," << myBarWidth << "," <<myY << ") , myBrush );" << std::endl;
#endif
            myPainter.drawRect(myX+myYGutterWidth,myImageHeight-(myY+myXGutterHeight),myBarWidth,myY);
          }
          else //line graph
          {
            myY = myGraphImageHeight - myY;
            myPointArray.setPoint(myBin, myX+myYGutterWidth, myY);
          }
        }
        if (myGraphType==LINE_CHART)
        {
          if (myBandCountInt==1) //draw single band images with black
          {
            myPainter.setPen( Qt::black );
          }
          else if (myIteratorInt==1)
          {
            myPainter.setPen( Qt::red );
          }
          else if (myIteratorInt==2)
          {
            myPainter.setPen( Qt::green );
          }
          else if (myIteratorInt==3)
          {
            myPainter.setPen( Qt::blue );
          }
          else if (myIteratorInt==4)
          {
            myPainter.setPen( Qt::magenta );
          }
          else if (myIteratorInt==5)
          {
            myPainter.setPen( Qt::darkRed );
          }
          else if (myIteratorInt==6)
          {
            myPainter.setPen( Qt::darkGreen );
          }
          else if (myIteratorInt==7)
          {
            myPainter.setPen( Qt::darkBlue );
          }
          else
          {
            myPainter.setPen( Qt::gray );
          }
          myPainter.drawPolyline(myPointArray);
        }
      }
    }
  }

  //
  // Now draw interval markers on the x axis
  //
  int myXDivisions = myGraphImageWidth/10;
  myPainter.setPen( Qt::gray );
  for (int i=0;i<myXDivisions;++i)
  {
    QPolygon myPointArray(4);
    myPointArray.setPoint(0,(i*myXDivisions)+myYGutterWidth , myImageHeight-myXGutterHeight);
    myPointArray.setPoint(1,(i*myXDivisions)+myYGutterWidth , myImageHeight-(myXGutterHeight-5));
    myPointArray.setPoint(2,(i*myXDivisions)+myYGutterWidth , myImageHeight-myXGutterHeight);
    myPointArray.setPoint(3,((i+1)*myXDivisions)+myYGutterWidth , myImageHeight-myXGutterHeight);
    myPainter.drawPolyline(myPointArray);
  }
  //
  // Now draw interval markers on the y axis
  //
  int myYDivisions = myGraphImageHeight/10;
  myPainter.setPen( Qt::gray );
  for (int i=myYDivisions;i>0;--i)
  {

    QPolygon myPointArray(4);
    int myYOrigin = myImageHeight-myXGutterHeight;
    myPointArray.setPoint(0,myYGutterWidth,myYOrigin-(i*myYDivisions ));
    myPointArray.setPoint(1,myYGutterWidth-5,myYOrigin-(i*myYDivisions ));
    myPointArray.setPoint(2,myYGutterWidth,myYOrigin-(i*myYDivisions ));
    myPointArray.setPoint(3,myYGutterWidth,myYOrigin-((i-1)*myYDivisions ));
    myPainter.drawPolyline(myPointArray);
  }

  //now draw the axis labels onto the graph
  myPainter.drawText(1, 12, myYMaxLabel);
  myPainter.drawText(1, myImageHeight-myXGutterHeight, QString::number(static_cast < unsigned int >(myYAxisMin)));
  myPainter.drawText(myYGutterWidth,myImageHeight-1 , myXMinLabel);
  myPainter.drawText( myImageWidth-myXGutterWidth,myImageHeight-1, myXMaxLabel );

  //
  // Finish up
  //
  myPainter.end();
  pixHistogram->setPixmap(myPixmap);
}

void QgsRasterLayerProperties::on_pbnImportTransparentPixelValues_clicked()
{
  int myLineCounter = 0;
  bool myImportError = false;
  QString myBadLines;
  QString myFilename = QFileDialog::getOpenFileName(this, tr("Open file"), "/", tr("Textfile (*.txt)"));
  QFile myInputFile(myFilename);
  if (myInputFile.open(QFile::ReadOnly))
  {
    QTextStream myInputStream(&myInputFile);
    QString myInputLine;
    if(rbtnThreeBand->isChecked())
    {
      for(int myTableRunner = tableTransparency->rowCount() - 1; myTableRunner >= 0; myTableRunner--)
      {
        tableTransparency->removeRow(myTableRunner);
      }

      while(!myInputStream.atEnd())
      {
        myLineCounter++;
        myInputLine = myInputStream.readLine();
        if(!myInputLine.isEmpty())
        {
          if(!myInputLine.simplified().startsWith("#"))
          {
            QStringList myTokens = myInputLine.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            if(myTokens.count() != 3)
            {
              myImportError = true;
              myBadLines = myBadLines + QString::number(myLineCounter) + ":\t[" + myInputLine + "]\n";
            }
            else
            {
              tableTransparency->insertRow(tableTransparency->rowCount());
              tableTransparency->setItem(tableTransparency->rowCount() - 1, 0, new QTableWidgetItem(myTokens[0]));
              tableTransparency->setItem(tableTransparency->rowCount() - 1, 1, new QTableWidgetItem(myTokens[1]));
              tableTransparency->setItem(tableTransparency->rowCount() - 1, 2, new QTableWidgetItem(myTokens[2]));
            }
          }
        }
      }
    }
    else
    {
      for(int myTableRunner = tableTransparency->rowCount() - 1; myTableRunner >= 0; myTableRunner--)
      {
        tableTransparency->removeRow(myTableRunner);
      }

      while(!myInputStream.atEnd())
      {
        myLineCounter++;
        myInputLine = myInputStream.readLine();
        if(!myInputLine.isEmpty())
        {
          if(!myInputLine.simplified().startsWith("#"))
          {
            QStringList myTokens = myInputLine.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            if(myTokens.count() != 1)
            {
              myImportError = true;
              myBadLines = myBadLines + QString::number(myLineCounter) + ":\t[" + myInputLine + "]\n";
            }
            else
            {
              tableTransparency->insertRow(tableTransparency->rowCount());
              tableTransparency->setItem(tableTransparency->rowCount() - 1, 0, new QTableWidgetItem(myTokens[0]));
            }
          }
        }
      }
    }

    if(myImportError)
    {
      QMessageBox::warning(this, tr("Import Error"), tr("The following lines contained errors\n\n") + myBadLines );
    }
  }
  else if(!myFilename.isEmpty())
  {
    QMessageBox::warning(this, tr("Read access denied"), tr("Read access denied. Adjust the file permissions and try again.\n\n") );
  }
}

void QgsRasterLayerProperties::on_pbnRemoveSelectedRow_clicked() 
{
  if(rbtnThreeBand->isChecked())
  {
    if(1 < tableTransparency->rowCount())
    {
      tableTransparency->removeRow(tableTransparency->currentRow());
    }
  }
  else
  {
    if(1 < tableTransparency->rowCount())
    {
      tableTransparency->removeRow(tableTransparency->currentRow());
    }
  }
}

void QgsRasterLayerProperties::on_rbtnSingleBand_toggled(bool b)
{
  if(b)
  {
    stackedStretchParameters->setCurrentIndex(1);
    tableTransparency->clear();
    tableTransparency->setColumnCount(1);
    tableTransparency->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Gray")));
    // If no band is selected but there are multiple bands, selcet the first as the default
    if(cboGray->currentText() == tr("Not Set") && 1 < cboGray->count())
    {
      cboGray->setCurrentIndex(0);
    }
  }
}

void QgsRasterLayerProperties::on_rbtnThreeBand_toggled(bool b)
{
  if(b)
  {
    stackedStretchParameters->setCurrentIndex(0);
    tableTransparency->clear();
    tableTransparency->setColumnCount(3);
    tableTransparency->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Red")));
    tableTransparency->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Green")));
    tableTransparency->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Blue")));
  }
}

void QgsRasterLayerProperties::sboxStdDevSingleBand_valueChanged(double theValue) 
{
  if(!ignoreSpinBoxEvent)
  {
    leGrayMin->setText("");
    leGrayMax->setText("");
  }
  else 
    ignoreSpinBoxEvent = false;
}

void QgsRasterLayerProperties::sboxStdDevThreeBand_valueChanged(double theValue) 
{
  if(!ignoreSpinBoxEvent)
  {
    leRedMin->setText("");
    leRedMax->setText("");
    leGreenMin->setText("");
    leGreenMax->setText("");
    leBlueMin->setText("");
    leBlueMax->setText("");
  }
  else
    ignoreSpinBoxEvent = false;
}

void QgsRasterLayerProperties::sliderTransparency_valueChanged(int theValue)
{
  //set the transparency percentage label to a suitable value
  int myInt = static_cast < int >((theValue / 255.0) * 100);  //255.0 to prevent integer division
  lblTransparencyPercent->setText(QString::number(myInt) + "%");
}//sliderTransparency_valueChanged

void QgsRasterLayerProperties::userDefinedMinMax_textEdited(QString theString)
{
  /*
   * If all min max values are set and valid, then reset stdDev to 0.0
   */
  if(rbtnThreeBand->isChecked())
  {
    if(validUserDefinedMinMax() && sboxStdDevThreeBand->value() != 0.0)
    {
      ignoreSpinBoxEvent = true;
      sboxStdDevThreeBand->setValue(0.0);
    }
  }
  else
  {
    if(validUserDefinedMinMax() && sboxStdDevSingleBand->value() != 0.0)
    {
      ignoreSpinBoxEvent = true;
      sboxStdDevSingleBand->setValue(0.0);
    }
  }
}


