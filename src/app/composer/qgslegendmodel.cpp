/***************************************************************************
                         qgslegendmodel.cpp  -  description
                         ------------------
    begin                : June 2008
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

#include "qgslegendmodel.h"
#include "qgsmaplayer.h"
#include "qgsmaplayerregistry.h"
#include "qgsrenderer.h"
#include "qgssymbol.h"
#include "qgsvectorlayer.h"

QgsLegendModel::QgsLegendModel()
{

}

QgsLegendModel::~QgsLegendModel()
{

}

void QgsLegendModel::setLayerSet(const QStringList& layerIds)
{
  //in future check which layers have newly been added or deleted

  mLayerIds = layerIds;
  
  //for now clear the model and add the new entries
  clear();
  
  QStringList::const_iterator idIter = mLayerIds.constBegin();
  QgsMapLayer* currentLayer = 0;

  for(; idIter != mLayerIds.constEnd(); ++idIter)
    {
      currentLayer = QgsMapLayerRegistry::instance()->mapLayer(*idIter);

      //addItem for layer
      QStandardItem* layerItem = new QStandardItem(currentLayer->name());
      layerItem->setFlags(Qt::ItemIsEnabled);
      invisibleRootItem()->setChild (invisibleRootItem()->rowCount(), layerItem);

      switch(currentLayer->type())
	{
	case QgsMapLayer::VECTOR:
	  addVectorLayerItems(layerItem, currentLayer);
	  break;
	case QgsMapLayer::RASTER:
	  break;
	default:
	  break;
	}
    }
  
}

int QgsLegendModel::addVectorLayerItems(QStandardItem* layerItem, QgsMapLayer* vlayer)
{
  if(!layerItem || !vlayer)
    {
      return 1;
    }

  QgsVectorLayer* vectorLayer = dynamic_cast<QgsVectorLayer*>(vlayer);
  if(!vectorLayer)
    {
      return 2;
    }

  const QgsRenderer* vectorRenderer = vectorLayer->renderer();
  if(!vectorRenderer)
    {
      return 3;
    }

  const QList<QgsSymbol*> vectorSymbols = vectorRenderer->symbols();
  QList<QgsSymbol*>::const_iterator symbolIt = vectorSymbols.constBegin();

  QStandardItem* currentSymbolItem = 0;
  QStandardItem* currentLabelItem = 0;

  for(; symbolIt != vectorSymbols.constEnd(); ++symbolIt)
    {
      if(!(*symbolIt))
	{
	  continue;
	}

      //icon item
      switch((*symbolIt)->type())
	{
	case QGis::Point:
	  currentSymbolItem = new QStandardItem(QIcon(QPixmap::fromImage((*symbolIt)->getPointSymbolAsImage())), "PointSymbol");
	  break;
	case QGis::Line:
	  currentSymbolItem = new QStandardItem(QIcon(QPixmap::fromImage((*symbolIt)->getLineSymbolAsImage())), "PointSymbol");
	  break;
	case QGis::Polygon:
	  currentSymbolItem = new QStandardItem(QIcon(QPixmap::fromImage((*symbolIt)->getPolygonSymbolAsImage())), "PointSymbol");
	  break;
	default:
	  currentSymbolItem = 0;
	  break;
	}

      if(!currentSymbolItem)
	{
	  continue;
	}

      int currentRowCount = layerItem->rowCount();
      layerItem->setChild(currentRowCount, 0, currentSymbolItem);

      //label
      QString label;
      QString lowerValue = (*symbolIt)->lowerValue();
      QString upperValue = (*symbolIt)->upperValue();

      if(lowerValue == upperValue)
	{
	  label = lowerValue;
	}
      else
	{
	  label = lowerValue + " - " + upperValue;
	}

      currentLabelItem = new QStandardItem(label);
      layerItem->setChild(currentRowCount, 1, currentLabelItem);
      
    }
  
  return 0;
}

