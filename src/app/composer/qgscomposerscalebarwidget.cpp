/***************************************************************************
                            qgscomposerscalebarwidget.cpp
                            -----------------------------
    begin                : 11 June 2008
    copyright            : (C) 2008 by Marco Hugentobler
    email                : marco dot hugentobler at karto dot baug dot ethz dot ch
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgscomposerscalebarwidget.h"
#include "qgscomposermap.h"
#include "qgscomposerscalebar.h"
#include <QColorDialog>
#include <QFontDialog>
#include <QWidget>

QgsComposerScaleBarWidget::QgsComposerScaleBarWidget(QgsComposerScaleBar* scaleBar): QWidget(), mComposerScaleBar(scaleBar)
{
  setupUi(this);
  setGuiElements(); //set the GUI elements to the state of scaleBar
}

QgsComposerScaleBarWidget::~QgsComposerScaleBarWidget()
{

}

void QgsComposerScaleBarWidget::refreshMapComboBox()
{
  mMapComboBox->clear();

  if(mComposerScaleBar)
    {
      //insert available maps into mMapComboBox
      const QgsComposition* scaleBarComposition = mComposerScaleBar->composition();
      if(scaleBarComposition)
	{
	  QList<const QgsComposerMap*> availableMaps = scaleBarComposition->composerMapItems();
	  QList<const QgsComposerMap*>::const_iterator mapItemIt = availableMaps.constBegin();
	  for(; mapItemIt != availableMaps.constEnd(); ++mapItemIt)
	    {
	      mMapComboBox->addItem(tr("Map ") + QString::number((*mapItemIt)->id()));
	    } 
	}
    }
}

void QgsComposerScaleBarWidget::showEvent ( QShowEvent * event )
{
  refreshMapComboBox();
  QWidget::showEvent(event);
}

void QgsComposerScaleBarWidget::on_mMapComboBox_activated(const QString& text)
{
  if(!mComposerScaleBar)
    {
      return;
    }

  const QgsComposition* comp = mComposerScaleBar->composition();
  if(!comp)
    {
      return;
    }

  //extract id
  int id;
  bool conversionOk;
  QString idString = text.split(" ").at(1);
  id = idString.toInt(&conversionOk);

  if(!conversionOk)
    {
      return;
    }

  //get QgsComposerMap object from composition
  const QgsComposerMap* composerMap = comp->getComposerMapById(id);
  if(!composerMap)
    {
      return;
    }

  //set it to scale bar
  mComposerScaleBar->setComposerMap(composerMap);

  //update scale bar
  mComposerScaleBar->update();
}

void QgsComposerScaleBarWidget::setGuiElements()
{
  if(!mComposerScaleBar)
    {
      return;
    }

  blockSignals(true);
  mNumberOfSegmentsSpinBox->setValue(mComposerScaleBar->numSegments());
  mSegmentsLeftSpinBox->setValue(mComposerScaleBar->numSegmentsLeft());
  mSegmentSizeSpinBox->setValue(mComposerScaleBar->numUnitsPerSegment());
  mLineWidthSpinBox->setValue(mComposerScaleBar->pen().widthF());
  mHeightSpinBox->setValue(mComposerScaleBar->height());
  mMapUnitsPerBarUnitSpinBox->setValue(mComposerScaleBar->numMapUnitsPerScaleBarUnit());

  //map combo box
  if(mComposerScaleBar->composerMap())
    {
      QString mapText = tr("Map ") + mComposerScaleBar->composerMap()->id();
      int itemId = mMapComboBox->findText(mapText);
      if(itemId > 0)
	{
	  mMapComboBox->setCurrentItem(itemId);
	}
    }

  //Style //todo...
  
  
  blockSignals(false);
}

//slots

void QgsComposerScaleBarWidget::on_mLineWidthSpinBox_valueChanged(double d)
{
  if(!mComposerScaleBar)
    {
      return;
    }

  QPen newPen(QColor(0, 0, 0));
  newPen.setWidthF(d);
  mComposerScaleBar->setPen(newPen);
  mComposerScaleBar->update();
}

void QgsComposerScaleBarWidget::on_mSegmentSizeSpinBox_valueChanged(double d)
{
  if(!mComposerScaleBar)
    {
      return;
    }

  mComposerScaleBar->setNumUnitsPerSegment(d);
  mComposerScaleBar->update();
}

void QgsComposerScaleBarWidget::on_mSegmentsLeftSpinBox_valueChanged(int i)
{
  if(!mComposerScaleBar)
    {
      return;
    }

  mComposerScaleBar->setNumSegmentsLeft(i);
  mComposerScaleBar->update();
}

void QgsComposerScaleBarWidget::on_mNumberOfSegmentsSpinBox_valueChanged(int i)
{
  if(!mComposerScaleBar)
    {
      return;
    }

  mComposerScaleBar->setNumSegments(i);
  mComposerScaleBar->update();
}

void QgsComposerScaleBarWidget::on_mHeightSpinBox_valueChanged(int i)
{
if(!mComposerScaleBar)
   {
     return;
   }
 mComposerScaleBar->setHeight(i);
 mComposerScaleBar->update();
}

void QgsComposerScaleBarWidget::on_mFontButton_clicked()
{
  if(!mComposerScaleBar)
    {
      return;
    }

  bool dialogAccepted;
  QFont oldFont = mComposerScaleBar->font();
  QFont newFont = QFontDialog::getFont(&dialogAccepted, oldFont, 0);
  if(dialogAccepted)
    {
      mComposerScaleBar->setFont(newFont);
    }
  mComposerScaleBar->update();
}

void QgsComposerScaleBarWidget::on_mColorPushButton_clicked()
{
  if(!mComposerScaleBar)
    {
      return;
    }

  QColor oldColor = mComposerScaleBar->brush().color();
  QColor newColor = QColorDialog::getColor(oldColor, 0);
  
  if(!newColor.isValid()) //user canceled the dialog
    {
      return;
    }

  QBrush newBrush(newColor);
  mComposerScaleBar->setBrush(newBrush);
  mComposerScaleBar->update();
}

void QgsComposerScaleBarWidget::on_mUnitLabelLineEdit_textChanged(const QString& text)
{
  if(!mComposerScaleBar)
    {
      return;
    }

  mComposerScaleBar->setUnitLabeling(text);
  mComposerScaleBar->update();
}

void QgsComposerScaleBarWidget::on_mMapUnitsPerBarUnitSpinBox_valueChanged(double d)
{
  if(!mComposerScaleBar)
    {
      return;
    }
  
  mComposerScaleBar->setNumMapUnitsPerScaleBarUnit(d);
  mComposerScaleBar->update();
}
