/***************************************************************************
                         qgscomposerlegend.cpp  -  description
                         ---------------------
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

#include "qgscomposerlegend.h"
#include "qgsmaplayer.h"
#include "qgsmaplayerregistry.h"
#include "qgssymbol.h"
#include <QPainter>

QgsComposerLegend::QgsComposerLegend(QgsComposition* composition): QgsComposerItem(composition), mTitle(QObject::tr("Legend")), mBoxSpace(2), mLayerSpace(3), mSymbolSpace(2), mIconLabelSpace(2)
{
  QStringList layerIdList;
  QMap<QString,QgsMapLayer*> layerMap =  QgsMapLayerRegistry::instance()->mapLayers();
  QMap<QString,QgsMapLayer*>::const_iterator mapIt = layerMap.constBegin();

  for(; mapIt != layerMap.constEnd(); ++mapIt)
    {
      layerIdList.push_back(mapIt.key());
    }

  mLegendModel.setLayerSet(layerIdList);

  //default font size
  mTitleFont.setPointSizeF(4);
  mLayerFont.setPointSizeF(3);
  mItemFont.setPointSizeF(2);
  mSymbolWidth = 7;
  mSymbolHeight = 4;
  adjustBoxSize();
}

QgsComposerLegend::QgsComposerLegend(): QgsComposerItem(0)
{

}

QgsComposerLegend::~QgsComposerLegend()
{

}

void QgsComposerLegend::paint(QPainter* painter, const QStyleOptionGraphicsItem* itemStyle, QWidget* pWidget)
{
  paintAndDetermineSize(painter);
}

QSizeF QgsComposerLegend::paintAndDetermineSize(QPainter* painter)
{
  QSizeF size;
  double maxXCoord = 0;

  //go through model...
  QStandardItem* rootItem = mLegendModel.invisibleRootItem();
  if(!rootItem)
    {
      return size;
    }

  if(painter)
    {
      painter->save();
    }

  int numLayerItems = rootItem->rowCount();
  QStandardItem* currentLayerItem = 0;
  double currentYCoordinate = mBoxSpace;

  //font metrics
  QFontMetricsF titleFontMetrics(mTitleFont);
  QFontMetricsF layerFontMetrics(mLayerFont);

  //draw title
  currentYCoordinate += titleFontMetrics.height();
  if(painter)
    {
      painter->setFont(mTitleFont);
      painter->drawText(QPointF(mBoxSpace, currentYCoordinate), mTitle);
    }

  maxXCoord = 2 * mBoxSpace + titleFontMetrics.width(mTitle);

  //draw layer items
  for(int i = 0; i < numLayerItems; ++i)
    {
      currentLayerItem = rootItem->child(i);
      if(currentLayerItem)
	{
	  currentYCoordinate += mLayerSpace;
	  currentYCoordinate += layerFontMetrics.height();

	  //draw layer Item
	  if(painter)
	    {
	      painter->setFont(mLayerFont);
	      painter->drawText(QPointF(mBoxSpace, currentYCoordinate), currentLayerItem->text());
	    }

	  maxXCoord = std::max(maxXCoord, 2 * mBoxSpace + layerFontMetrics.width(currentLayerItem->text()));
	  
	  //and child items
	  drawLayerChildItems(painter, currentLayerItem, currentYCoordinate, maxXCoord);
	}
    }

  currentYCoordinate += mBoxSpace;

  if(painter)
    {
      painter->restore();

      //draw frame and selection boxes if necessary
      drawFrame(painter);
      if(isSelected())
	{
	  drawSelectionBoxes(painter);
	}
    }

  size.setHeight(currentYCoordinate);
  size.setWidth(maxXCoord);
  return size;
}

void QgsComposerLegend::adjustBoxSize()
{
  QSizeF size = paintAndDetermineSize(0);
  if(size.isValid())
    {
      setSceneRect(QRectF(transform().dx(), transform().dy(), size.width(), size.height()));
    }
}

void QgsComposerLegend::drawLayerChildItems(QPainter* p, QStandardItem* layerItem, double& currentYCoord, double& maxXCoord)
{
  if(!layerItem)
    {
      return;
    }

  QFontMetricsF itemFontMetrics(mItemFont);
  double itemHeight = std::max(mSymbolHeight, itemFontMetrics.height());

  QStandardItem* firstItem;
  QStandardItem* secondItem;

  int numChildren = layerItem->rowCount();

  if(p)
    {
      p->setFont(mItemFont);
    }

  for(int i = 0; i < numChildren; ++i)
    {
      currentYCoord += mSymbolSpace;
      double currentXCoord = mBoxSpace;
     
      firstItem = layerItem->child(i, 0);
      secondItem = layerItem->child(i, 1);

      if(secondItem) //an item with an icon
	{
	  QIcon symbolIcon = firstItem->icon();

	  //take QgsSymbol* from user data
	  QVariant symbolVariant = firstItem->data();
	  if(!symbolVariant.canConvert<void*>())
	    {
	      continue;
	    }
	  void* symbolData = symbolVariant.value<void*>();
	  QgsSymbol* symbol = (QgsSymbol*)(symbolData);
	  if(!symbol)
	    {
	      continue;
	    }

	  //draw symbol considering output device resolution
	  drawSymbol(p, symbol, currentYCoord + (itemHeight - mSymbolHeight) /2, currentXCoord);
	  currentXCoord += mIconLabelSpace;
	  
	  if(p)
	    {
	      p->drawText(QPointF(currentXCoord, currentYCoord + itemFontMetrics.height()), secondItem->text());
	    }

	  maxXCoord = std::max(maxXCoord, currentXCoord + itemFontMetrics.width(secondItem->text()) + mBoxSpace);
	}
      else //an item witout icon (e.g. name of classification field)
	{
	  if(p)
	    {
	      p->drawText(QPointF(currentXCoord, currentYCoord + itemFontMetrics.height()), firstItem->text());
	    }
	  maxXCoord = std::max(maxXCoord, currentXCoord + itemFontMetrics.width(firstItem->text()) + mBoxSpace);
	}
 
      currentYCoord += itemHeight;
    }
}

void QgsComposerLegend::drawSymbol(QPainter* p, QgsSymbol* s, double currentYCoord, double& currentXPosition) const
{
  if(!s)
    {
      return;
    }

  QGis::VectorType symbolType = s->type();
  switch(symbolType)
    {
    case QGis::Point:
      drawPointSymbol(p, s, currentYCoord, currentXPosition);
      break;
    case QGis::Line:
      drawLineSymbol(p, s, currentYCoord, currentXPosition);
      break;
    case QGis::Polygon:
      drawPolygonSymbol(p, s, currentYCoord, currentXPosition);
      break;
    }
}

void QgsComposerLegend::drawPointSymbol(QPainter* p, QgsSymbol* s, double currentYCoord, double& currentXPosition) const
{
  if(!s)
    {
      return;
    }

  QImage pointImage;
  double rasterScaleFactor = 1.0;
  if(p)
    {
      QPaintDevice* paintDevice = p->device();
      if(!paintDevice)
	{
	  return;
	}
      
      rasterScaleFactor = (paintDevice->logicalDpiX() + paintDevice->logicalDpiY()) / 2.0 / 25.4;
      double widthScale = (paintDevice->logicalDpiX() + paintDevice->logicalDpiY()) / 2 / 25.4;
    }
  
  //width scale is 1.0
  pointImage = s->getPointSymbolAsImage(1.0, false, Qt::yellow, 1.0, 0.0, rasterScaleFactor);

  if(p)
    {
      p->save();
      p->scale(1.0 / rasterScaleFactor, 1.0 / rasterScaleFactor);
      
      QPointF imageTopLeft(currentXPosition * rasterScaleFactor, currentYCoord * rasterScaleFactor);
      p->drawImage(imageTopLeft, pointImage);
      p->restore();
    }
  
  currentXPosition += pointImage.width();
}

void QgsComposerLegend::drawLineSymbol(QPainter* p, QgsSymbol* s, double currentYCoord, double& currentXPosition) const
{
  if(!s)
    {
      return;
    }

  double yCoord = currentYCoord + mSymbolHeight/2;

  if(p)
    {
      p->save();
      p->setPen(s->pen());
      p->drawLine(QPointF(currentXPosition, yCoord), QPointF(currentXPosition + mSymbolWidth, yCoord));
      p->restore();
    }

  currentXPosition += mSymbolWidth;
}

void QgsComposerLegend::drawPolygonSymbol(QPainter* p, QgsSymbol* s, double currentYCoord, double& currentXPosition) const
{
  if(!s)
    {
      return;
    }

  if(p)
    {
      p->setBrush(s->brush());
      p->setPen(s->pen());
      p->drawRect(QRectF(currentXPosition, currentYCoord, mSymbolWidth, mSymbolHeight));
    }

  currentXPosition += mSymbolWidth;
}
