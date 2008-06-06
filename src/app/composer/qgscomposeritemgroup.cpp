/***************************************************************************
                         qgscomposeritemgroup.cpp
                         ------------------------
    begin                : 2nd June 2008
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

#include "qgscomposeritemgroup.h"
#include "qgscomposition.h"
#include "qgscomposer.h" //probably use other approach to delete items because including this in a composer class is ugly
#include <QPen>
#include <QPainter>

QgsComposerItemGroup::QgsComposerItemGroup(QgsComposition* c, QGraphicsItem* parent): QgsComposerItem(parent), mComposition(c)
{
  setZValue(50);
  show();
}

QgsComposerItemGroup::~QgsComposerItemGroup()
{
  //todo: notify composition to delete child items too
  //QgsComposer::removeItem(QgsComposerItem* item)

  if(!mComposition)
    {
      return;
    }

  QgsComposer* composer = mComposition->composer();
  if(!composer)
    {
      return;
    }
  
  QSet<QgsComposerItem*>::iterator itemIt = mItems.begin();
  for(; itemIt != mItems.end(); ++itemIt)
    {
      composer->removeItem(*itemIt);
    }
}
 
void QgsComposerItemGroup::addItem(QgsComposerItem* item)
{
  if(!item)
    {
      return;
    }

  if(mItems.contains(item))
    {
      return;
    }
  mItems.insert(item);
  item->setSelected(false);
  item->setFlag(QGraphicsItem::ItemIsSelectable, false); //item in groups cannot be selected

  //update extent (which is in scene coordinates)
  double minXItem = item->transform().dx();
  double minYItem = item->transform().dy();
  double maxXItem = minXItem + item->rect().width();
  double maxYItem = minYItem + item->rect().height();

  if(mSceneBoundingRectangle.isEmpty()) //we add the first item
    {
      mSceneBoundingRectangle.setLeft(minXItem);
      mSceneBoundingRectangle.setTop(minYItem);
      mSceneBoundingRectangle.setRight(maxXItem);
      mSceneBoundingRectangle.setBottom(maxYItem);
    }

  else
    {
      if(minXItem < mSceneBoundingRectangle.left())
	{
	  mSceneBoundingRectangle.setLeft(minXItem);
	}
      if(minYItem < mSceneBoundingRectangle.top())
	{
	  mSceneBoundingRectangle.setTop(minYItem);
	}
      if(maxXItem > mSceneBoundingRectangle.right())
	{
	  mSceneBoundingRectangle.setRight(maxXItem);
	}
      if(maxYItem > mSceneBoundingRectangle.bottom())
	{
	  mSceneBoundingRectangle.setBottom(maxYItem);
	}
    }

  QgsComposerItem::setSceneRect(mSceneBoundingRectangle); //call method of superclass to avoid repositioning of items

}

void QgsComposerItemGroup::removeItems()
{
  QSet<QgsComposerItem*>::iterator item_it = mItems.begin();
  for(; item_it != mItems.end(); ++item_it)
    {
      (*item_it)->setFlag(QGraphicsItem::ItemIsSelectable, true); //enable item selection again
    }
  mItems.clear();
}

void QgsComposerItemGroup::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
  drawFrame(painter);
  if(isSelected())
    {
      drawSelectionBoxes(painter);
    }
}

void QgsComposerItemGroup::setSceneRect(const QRectF& rectangle)
{
  //call resize and transform for every item

  double transformX = rectangle.x() - transform().dx();
  double transformY = rectangle.y() - transform().dy();

  double dx = rectangle.width() - rect().width();
  double dy = rectangle.height() - rect().height();

  double itemWidthRatio, itemHeightRatio;
  
  double newItemWidth, newItemHeight;
  QRectF newItemRect;

  QSet<QgsComposerItem*>::iterator item_it = mItems.begin();
  for(; item_it != mItems.end(); ++item_it)
    { 
      itemWidthRatio = (*item_it)->rect().width()/ mSceneBoundingRectangle.width();
      itemHeightRatio = (*item_it)->rect().height()/ mSceneBoundingRectangle.height();

      newItemWidth = (*item_it)->rect().width() + dx * itemWidthRatio;
      newItemHeight = (*item_it)->rect().height() + dy * itemHeightRatio;
      newItemRect = QRectF((*item_it)->transform().dx() + transformX, (*item_it)->transform().dy() + transformY, newItemWidth, newItemHeight);  
      (*item_it)->setSceneRect(newItemRect);
    }

  QgsComposerItem::setSceneRect(rectangle);
}

void QgsComposerItemGroup::drawFrame(QPainter* p)
{
  if(mFrame && plotStyle() == QgsComposition::Preview)
    {
      QPen newPen(pen());
      newPen.setStyle(Qt::DashLine);
      newPen.setColor(QColor(128, 128, 128, 128));
      p->setPen(newPen);
      p->setRenderHint(QPainter::Antialiasing, true);
      p->drawRect (QRectF( 0, 0, rect().width(), rect().height()));
    }
}
