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
#include "qgsrasterlayer.h"
#include "qgsrenderer.h"
#include "qgssymbol.h"
#include "qgsvectorlayer.h"

QgsLegendModel::QgsLegendModel(): QStandardItemModel()
{
}

QgsLegendModel::~QgsLegendModel()
{
  QSet<QgsSymbol*>::iterator it = mSymbols.begin();
  for(; it != mSymbols.end(); ++it)
    {
      delete *it;
    }
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
      layerItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
     
      invisibleRootItem()->setChild (invisibleRootItem()->rowCount(), layerItem);

      switch(currentLayer->type())
	{
	case QgsMapLayer::VECTOR:
	  addVectorLayerItems(layerItem, currentLayer);
	  break;
	case QgsMapLayer::RASTER:
	  addRasterLayerItem(layerItem, currentLayer);
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

       //label
      QString label;
      QString lowerValue = (*symbolIt)->lowerValue();
      QString upperValue = (*symbolIt)->upperValue();

      if(lowerValue == upperValue || upperValue.isEmpty())
	{
	  label = lowerValue;
	}
      else
	{
	  label = lowerValue + " - " + upperValue;
	}

      //icon item
      switch((*symbolIt)->type())
	{
	case QGis::Point:
	  currentSymbolItem = new QStandardItem(QIcon(QPixmap::fromImage((*symbolIt)->getPointSymbolAsImage())), label);
	  break;
	case QGis::Line:
	  currentSymbolItem = new QStandardItem(QIcon(QPixmap::fromImage((*symbolIt)->getLineSymbolAsImage())), label);
	  break;
	case QGis::Polygon:
	  currentSymbolItem = new QStandardItem(QIcon(QPixmap::fromImage((*symbolIt)->getPolygonSymbolAsImage())), label);
	  break;
	default:
	  currentSymbolItem = 0;
	  break;
	}

      //Pass deep copy of QgsSymbol as user data. Cast to void* necessary such that QMetaType handles it
      QgsSymbol* symbolCopy = new QgsSymbol(**symbolIt);
      currentSymbolItem->setData(QVariant::fromValue((void*)symbolCopy));
      insertSymbol(symbolCopy);

      if(!currentSymbolItem)
	{
	  continue;
	}

      int currentRowCount = layerItem->rowCount();
      currentSymbolItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

      layerItem->setChild(layerItem->rowCount(), 0, currentSymbolItem);
      
    }
  
  return 0;
}

int QgsLegendModel::addRasterLayerItem(QStandardItem* layerItem, QgsMapLayer* rlayer)
{
  if(!layerItem || !rlayer)
    {
      return 1;
    }

  QgsRasterLayer* rasterLayer = dynamic_cast<QgsRasterLayer*>(rlayer);
  if(!rasterLayer)
    {
      return 2;
    }

  QStandardItem* currentSymbolItem = new QStandardItem(QIcon(rasterLayer->getLegendQPixmap(true)), "");

  int currentRowCount = layerItem->rowCount();
  layerItem->setChild(currentRowCount, 0, currentSymbolItem);

  return 0;
}

void QgsLegendModel::insertSymbol(QgsSymbol* s)
{
  QSet<QgsSymbol*>::iterator it = mSymbols.find(s);
  if(it != mSymbols.end())
    {
      delete (*it); //very unlikely
    }
  else
    {
      mSymbols.insert(s);
    }
}
  
void QgsLegendModel::removeSymbol(QgsSymbol* s)
{
  mSymbols.remove(s);
}
