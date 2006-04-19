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

#include <QCoreApplication>
#include <QPainter>
#include <QSettings>


QgsLegendLayerFile::QgsLegendLayerFile(QTreeWidgetItem * theLegendItem, QString theString, QgsMapLayer* theLayer)
  : QgsLegendItem(theLegendItem, theString), mLayer(theLayer)
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
}

QgsLegendLayerFile::~QgsLegendLayerFile()
{
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
