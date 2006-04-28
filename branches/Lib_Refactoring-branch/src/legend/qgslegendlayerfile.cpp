/***************************************************************************
 *   Copyright (C) 2005 by Tim Sutton   *
 *   aps02ts@macbuntu   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "qgsapplication.h"
#include "qgslegend.h"
#include "qgslegendlayerfile.h"
#include "qgsmaplayer.h"
#include "qgsvectorlayer.h"
#include "qgsvectordataprovider.h"

// attribute table
#include "qgsattributetable.h"
#include "qgsattributetabledisplay.h"

#include "qgsencodingfiledialog.h"

#include <QApplication>
#include <QMessageBox>
#include <QPainter>
#include <QSettings>


QgsLegendLayerFile::QgsLegendLayerFile(QTreeWidgetItem * theLegendItem, QString theString, QgsMapLayer* theLayer)
  : QgsLegendItem(theLegendItem, theString), mLayer(theLayer), mTableDisplay(NULL)
{
  // Set the initial visibility flag for layers
  // This user option allows the user to turn off inital drawing of
  // layers when they are added to the map. This is useful when adding
  // many layers and the user wants to adjusty symbology, etc prior to
  // actually viewing the layer.
  QSettings settings;
  mVisible = settings.readBoolEntry("/qgis/new_layers_visible", 1);

  // not in overview by default
  mInOverview = FALSE;
  
  mType = LEGEND_LAYER_FILE;
  
  setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  setCheckState(0, Qt::Checked);
  setText(0, theString);

  // get notifications of changed selection - used to update attribute table
  connect(mLayer, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

  // get notifications of modified layer - used to close table as it's out of sync
  connect(mLayer, SIGNAL(wasModified(bool)), this, SLOT(closeTable(bool)));
}

QgsLegendLayerFile::~QgsLegendLayerFile()
{
  if (mTableDisplay)
  {
    mTableDisplay->close();
    delete mTableDisplay;
  }
}

QgsLegendItem::DRAG_ACTION QgsLegendLayerFile::accept(LEGEND_ITEM_TYPE type)
{
  return NO_ACTION;
}

QgsLegendItem::DRAG_ACTION QgsLegendLayerFile::accept(const QgsLegendItem* li) const
{
  if(li->type() == QgsLegendItem::LEGEND_LAYER_FILE)
    {
      if(li->parent() == this->parent())
	{
	  return REORDER;
	}
    }
  return NO_ACTION;
}


void QgsLegendLayerFile::updateLegendItem()
{
  QPixmap pix = legend()->pixmaps().mOriginalPixmap;
  
  if(mInOverview)
  {
    //add overview glasses to the pixmap
    QPainter p(&pix);
    p.drawPixmap(0,0, legend()->pixmaps().mInOverviewPixmap);
  }
  if(mLayer->isEditable())
  {
    //add editing icon to the pixmap
    QPainter p(&pix);
    p.drawPixmap(30,0, legend()->pixmaps().mEditablePixmap);
  }

  /*
  // TODO:
  if(mLayer->hasProjectionError())
  {
    //add overview glasses to the pixmap
    QPainter p(&pix);
    p.drawPixmap(60,0, legend()->pixmaps().mProjectionErrorPixmap);
  }
  */
  
  QIcon theIcon(pix);
  setIcon(0, theIcon);

}

void QgsLegendLayerFile::toggleCheckBox(bool state)
{
  //todo
}

QString QgsLegendLayerFile::nameFromLayer(QgsMapLayer* layer)
{
  QString sourcename = layer->source(); //todo: move this duplicated code into a new function
  if(sourcename.startsWith("host", false))
    {
      //this layer is a database layer
      //modify source string such that password is not visible
      sourcename = layer->name();
    }
  else
    {
      //modify source name such that only the file is visible
      sourcename = layer->source().section('/',-1,-1);
    }
  return sourcename;
}


void QgsLegendLayerFile::setVisible(bool visible)
{
  mVisible = visible;
}

bool QgsLegendLayerFile::isVisible()
{
  return mVisible;
}

void QgsLegendLayerFile::setInOverview(bool inOverview)
{
  mInOverview = inOverview;
}

bool QgsLegendLayerFile::isInOverview()
{
  return mInOverview;
}


void QgsLegendLayerFile::table()
{
  QgsVectorLayer* vlayer = dynamic_cast<QgsVectorLayer*>(mLayer);
  if (!vlayer)
    return;
  
  QgsAttributeAction& actions = *vlayer->actions();
  
  if (mTableDisplay)
  {
    
    mTableDisplay->raise();

    // Give the table the most recent copy of the actions for this layer.
    mTableDisplay->table()->setAttributeActions(actions);
  }
  else
  {
    // display the attribute table
    QApplication::setOverrideCursor(Qt::waitCursor);
    mTableDisplay = new QgsAttributeTableDisplay(vlayer);
    mTableDisplay->table()->fillTable(vlayer);
    mTableDisplay->table()->setSorting(true);

    connect(mTableDisplay, SIGNAL(deleted()), this, SLOT(invalidateTableDisplay()));

    mTableDisplay->setTitle(tr("Attribute table - ") + name());
    mTableDisplay->show();

    // Give the table the most recent copy of the actions for this layer.
    mTableDisplay->table()->setAttributeActions(actions);
    
    // select rows which should be selected
    selectionChanged();
    
    // etablish the necessary connections between the table and the vector layer
    connect(mTableDisplay->table(), SIGNAL(selected(int)), mLayer, SLOT(select(int)));
    connect(mTableDisplay->table(), SIGNAL(selectionRemoved()), mLayer, SLOT(removeSelection()));
    connect(mTableDisplay->table(), SIGNAL(repaintRequested()), mLayer, SLOT(triggerRepaint()));
    
    QApplication::restoreOverrideCursor();
  }

}

void QgsLegendLayerFile::invalidateTableDisplay()
{
  // from signal deleted() - table doesn't exist anymore, just erase our pointer
  mTableDisplay = 0;
}

void QgsLegendLayerFile::selectionChanged()
{
  if (!mTableDisplay)
    return;

  QgsVectorLayer* vlayer = dynamic_cast<QgsVectorLayer*>(mLayer);
  const std::set<int>& ids = vlayer->selectedFeaturesIds();
  mTableDisplay->table()->selectRowsWithId(ids);

}

void QgsLegendLayerFile::closeTable(bool onlyGeometryWasChanged)
{
  if (mTableDisplay)
  {
    mTableDisplay->close();
    delete mTableDisplay;
    mTableDisplay = NULL;
  }
}

void QgsLegendLayerFile::saveAsShapefile()
{
  if (mLayer->type() != QgsMapLayer::VECTOR)
    return;

  QgsVectorLayer* vlayer = dynamic_cast<QgsVectorLayer*>(mLayer);
  
  // get a name for the shapefile
  // Get a file to process, starting at the current directory
  QSettings settings;
  QString filter =  QString("Shapefiles (*.shp)");
  QString dirName = settings.readEntry("/UI/lastShapefileDir", ".");

  QgsEncodingFileDialog* openFileDialog = new QgsEncodingFileDialog(0,
      tr("Save layer as..."),
      dirName,
      filter,
      QString("UTF-8"));

  // allow for selection of more than one file
  //openFileDialog->setMode(QFileDialog::AnyFile);

  if (openFileDialog->exec() != QDialog::Accepted)
    return;
    
  QString enc = openFileDialog->encoding();
  QString shapefileName = openFileDialog->selectedFile();
  
  if (shapefileName.isNull())
    return;

  // add the extension if not present
  if (shapefileName.find(".shp") == -1)
  {
    shapefileName += ".shp";
  }
  
  QString error = vlayer->saveAsShapefile(shapefileName, enc);
  
  if (error == "DRIVER_NOT_FOUND")
  {
    QMessageBox::warning(0, "Driver not found", "ESRI Shapefile driver is not available");
  }
  else if (error == "ERROR_CREATE_SOURCE")
  {
    QMessageBox::warning(0, "Error creating shapefile",
                         "The shapefile could not be created (" +
                             shapefileName + ")");
  }
  else if (error == "ERROR_CREATE_LAYER")
  {
    QMessageBox::warning(0, "Error", "Layer creation failed");
  }
  else
  {
    QMessageBox::information( 0, "Saving done", "Export to Shapefile has been completed");
  }
}

void QgsLegendLayerFile::toggleEditing()
{
  QgsVectorLayer* vlayer = dynamic_cast<QgsVectorLayer*>(mLayer);
  if (!vlayer)
    return;
  
  if (!vlayer->isEditable())
  {
    vlayer->startEditing();
    if(!(vlayer->getDataProvider()->capabilities() & QgsVectorDataProvider::AddFeatures))
    {
      QMessageBox::information(0,tr("Start editing failed"),
                               tr("Provider cannot be opened for editing"),
                               QMessageBox::Ok);
    }
  }
  else
  {
    // if not modified, just stop editing
    if (!vlayer->isModified())
    {
      vlayer->rollBack();
    }

    // commit or roll back?
    int commit = QMessageBox::information(0,tr("Stop editing"),
                                          tr("Do you want to save the changes?"),
                                          tr("&Yes"),tr("&No"),QString::null,0,1);  

    if(commit==0)
    {
      if(!vlayer->commitChanges())
      {
        QMessageBox::information(0,tr("Error"),tr("Could not commit changes"),QMessageBox::Ok);
      }
    }
    else if(commit==1)
    {
      if(!vlayer->rollBack())
      {
        QMessageBox::information(0,tr("Error"),
                                 tr("Problems during roll back"),QMessageBox::Ok);
      }
    }
    vlayer->triggerRepaint();
    
  }

}
