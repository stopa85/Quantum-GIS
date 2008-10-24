/***************************************************************************
                         qgspaperitem.cpp
                       -------------------
    begin                : September 2008
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

#include "qgspaperitem.h"
#include <QPainter>

QgsPaperItem::QgsPaperItem(QgsComposition* c): QgsComposerItem(c, false)
{
  setFlag( QGraphicsItem::ItemIsSelectable, false );
  setZValue(0);
}

QgsPaperItem::QgsPaperItem( qreal x, qreal y, qreal width, qreal height, QgsComposition* composition ): QgsComposerItem(x, y, width, height, composition, false)
{
  setFlag( QGraphicsItem::ItemIsSelectable, false );
  setZValue(0);
}

QgsPaperItem::QgsPaperItem(): QgsComposerItem(0, false)
{

}

QgsPaperItem::~QgsPaperItem()
{
  
}

void QgsPaperItem::paint( QPainter* painter, const QStyleOptionGraphicsItem* itemStyle, QWidget* pWidget )
{
  if ( !painter )
  {
    return;
  }

  drawBackground( painter );

  //draw grid

  if(mComposition)
    {
      if(mComposition->snapToGridEnabled() && mComposition->plotStyle() ==  QgsComposition::Preview 
	 && mComposition->snapGridResolution() > 0)
	{
	  QPen pen(QColor(100, 100, 100));
	  pen.setWidthF(0.3);
	  painter->setPen(pen);

	  //draw vertical lines  
	  int gridMultiplyX = (int)(mComposition->snapGridOffsetX() / mComposition->snapGridResolution());  
	  double currentXCoord = mComposition->snapGridOffsetX() - gridMultiplyX * mComposition->snapGridResolution();
	  for(; currentXCoord <= rect().width(); currentXCoord += mComposition->snapGridResolution())
	    {
	      painter->drawLine(QPointF(currentXCoord, 0), QPointF(currentXCoord, rect().height()));
	    }
	  
	  //draw horizontal lines
	  int gridMultiplyY = (int)(mComposition->snapGridOffsetY() / mComposition->snapGridResolution());
	  double currentYCoord = mComposition->snapGridOffsetY() - gridMultiplyY * mComposition->snapGridResolution();
	  for(; currentYCoord <= rect().height(); currentYCoord += mComposition->snapGridResolution())
	    {
	      painter->drawLine(QPointF(0, currentYCoord), QPointF(rect().width(), currentYCoord));
	    }
	}
    }
}

bool QgsPaperItem::writeXML( QDomElement& elem, QDomDocument & doc )
{
  return true;
}

bool QgsPaperItem::readXML( const QDomElement& itemElem, const QDomDocument& doc )
{
  return true;
}
