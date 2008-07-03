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
  //go through model...
  QStandardItem* rootItem = mLegendModel.invisibleRootItem();
  if(!rootItem || !painter)
    {
      return;
    }

  painter->save();

  int numLayerItems = rootItem->rowCount();
  QStandardItem* currentLayerItem = 0;
  double currentYCoordinate = mBoxSpace;

  //font metrics
  QFontMetricsF titleFontMetrics(mTitleFont);
  QFontMetricsF layerFontMetrics(mLayerFont);

  //draw title
  currentYCoordinate += titleFontMetrics.height();
  painter->setFont(mTitleFont);
  painter->drawText(QPointF(mBoxSpace, currentYCoordinate), mTitle);

  //draw layer items
  for(int i = 0; i < numLayerItems; ++i)
    {
      currentLayerItem = rootItem->child(i);
      if(currentLayerItem)
	{
	  currentYCoordinate += mLayerSpace;
	  currentYCoordinate += layerFontMetrics.height();

	  //draw layer Item
	  painter->setFont(mLayerFont);
	  painter->drawText(QPointF(mBoxSpace, currentYCoordinate), currentLayerItem->text());
	  
	  //and child items
	  drawLayerChildItems(painter, currentLayerItem, currentYCoordinate);
	}
    }

  painter->restore();

  //draw frame and selection boxes if necessary
  drawFrame(painter);
  if(isSelected())
    {
      drawSelectionBoxes(painter);
    }
}

void QgsComposerLegend::adjustBoxSize()
{
  //todo...
  setSceneRect(QRectF(transform().dx(), transform().dy(), 10, 10));
}

void QgsComposerLegend::drawLayerChildItems(QPainter* p, QStandardItem* layerItem, double& currentYCoord)
{
  if(!layerItem || !p)
    {
      return;
    }

  QFontMetricsF itemFontMetrics(mItemFont);
  double itemHeight = std::max(mSymbolHeight, itemFontMetrics.height());

  QStandardItem* firstItem;
  QStandardItem* secondItem;

  int numChildren = layerItem->rowCount();
  p->setFont(mItemFont);

  for(int i = 0; i < numChildren; ++i)
    {
      currentYCoord += mSymbolSpace;
      double currentXCoord = mBoxSpace;
     
      firstItem = layerItem->child(i, 0);
      secondItem = layerItem->child(i, 1);

      if(secondItem) //an item with an icon
	{
	  QIcon symbolIcon = firstItem->icon();
	  symbolIcon.paint(p, QRect(currentXCoord, currentYCoord + (itemHeight - mSymbolHeight) /2, mSymbolWidth, mSymbolHeight));
	  currentXCoord += (mSymbolWidth + mIconLabelSpace);
	  p->drawText(QPointF(currentXCoord, currentYCoord + itemFontMetrics.height()), secondItem->text());
	}
      else //an item witout icon (e.g. name of classification field)
	{
	  p->drawText(QPointF(currentXCoord, currentYCoord + itemFontMetrics.height()), firstItem->text());
	}
 
      currentYCoord += itemHeight;
    }
}
