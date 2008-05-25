/***************************************************************************
                         qgscomposeritem.cpp
                             -------------------
    begin                : January 2005
    copyright            : (C) 2005 by Radim Blazek
    email                : blazek@itc.it
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QWidget>
#include <QDomNode>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include "qgscomposition.h"
#include "qgscomposeritem.h"

#include <iostream>

QgsComposerItem::QgsComposerItem(QGraphicsItem* parent): QGraphicsRectItem(0), mBoundingResizeRectangle(0) 
{
    mSelected = false;
    mPlotStyle = QgsComposition::Preview;
    setAcceptsHoverEvents(true);
}

QgsComposerItem::QgsComposerItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent): QGraphicsRectItem(x, y, width, height, parent), mBoundingResizeRectangle(0)
{
  mSelected = false;
  mPlotStyle = QgsComposition::Preview;
  setAcceptsHoverEvents(true);
}

QgsComposerItem::~QgsComposerItem()
{
}

void QgsComposerItem::setPlotStyle ( QgsComposition::PlotStyle d ) { mPlotStyle = d; }

QgsComposition::PlotStyle QgsComposerItem::plotStyle ( void ) { return mPlotStyle; }

void QgsComposerItem::setSelected( bool s ) 
{
    std::cout << "QgsComposerItem::setSelected" << std::endl; 
    mSelected = s; 
}

bool QgsComposerItem::selected( void ) { return mSelected; }

int QgsComposerItem::id(void) { return mId; }

void QgsComposerItem::showOptions ( QWidget * parent ) { }

QWidget *QgsComposerItem::options ( void ) { return 0; }

bool QgsComposerItem::writeSettings ( void )  { return true; }

bool QgsComposerItem::readSettings ( void )  { return true; }

bool QgsComposerItem::removeSettings ( void )  { return true; }
    
bool QgsComposerItem::writeXML( QDomNode & node, QDomDocument & doc, bool templ ) { return true; }

bool QgsComposerItem::readXML( QDomNode & node ) {  return true; }

void QgsComposerItem::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
  if(mBoundingResizeRectangle)
    {
      double diffX = event->lastPos().x() - mLastMouseEventPos.x();
      double diffY = event->lastPos().y() - mLastMouseEventPos.y();

      double mx, my, rx, ry;

      rectangleChange(diffX, diffY, mx, my, rx, ry);
      
      QRectF r = mBoundingResizeRectangle->rect();
      r.translate(mx, my);
      r.setWidth(r.width() + rx);
      r.setHeight(r.height() + ry);
      mBoundingResizeRectangle->setRect(r);
    }
  mLastMouseEventPos = event->lastPos();
}

void QgsComposerItem::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
  //set current position and type of mouse move action
  mMouseMoveStartPos = event->lastScenePos();
  mLastMouseEventPos = event->lastPos();
  mCurrentMouseMoveAction = mouseMoveActionForPosition(event->lastPos());
  setCursor(QCursor(cursorForPosition(event->pos())));

  //create and show bounding rectangle
  mBoundingResizeRectangle = new QGraphicsRectItem(QGraphicsRectItem::rect(), 0);
  scene()->addItem(mBoundingResizeRectangle);
  mBoundingResizeRectangle->moveBy(x(), y());
  mBoundingResizeRectangle->setBrush( Qt::NoBrush );
  mBoundingResizeRectangle->setPen( QPen(QColor(0,0,0), 0) );
  mBoundingResizeRectangle->setZValue(100);
  mBoundingResizeRectangle->show();

  //QGraphicsRectItem::setVisible(false);
}

void QgsComposerItem::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
  //delete frame rectangle
  if(mBoundingResizeRectangle)
    {
      scene()->removeItem(mBoundingResizeRectangle);
      delete mBoundingResizeRectangle;
      mBoundingResizeRectangle = 0;
    }

  //QGraphicsRectItem::setVisible(true);

  QPointF mouseMoveStopPoint = event->lastScenePos();
  double diffX = mouseMoveStopPoint.x() - mMouseMoveStartPos.x();
  double diffY = mouseMoveStopPoint.y() - mMouseMoveStartPos.y();

  //it was only a click
  if(abs(diffX) < std::numeric_limits<double>::min() && abs(diffY) < std::numeric_limits<double>::min())
    {
      return;
    }

  double mx, my, rx, ry;
  rectangleChange(diffX, diffY, mx, my, rx, ry);
  moveBy(mx, my);
  resize(rx, ry);

  //reset default action
  mCurrentMouseMoveAction = QgsComposerItem::moveItem;
  setCursor(Qt::ArrowCursor);
}

void QgsComposerItem::hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
{
  qWarning("QgsComposerItem::hoverEnterEvent");
}

void QgsComposerItem::hoverMoveEvent( QGraphicsSceneHoverEvent * event )
{
  qWarning("QgsComposerItem::hoverMoveEvent");
  setCursor(QCursor(cursorForPosition(event->pos())));
}

Qt::CursorShape QgsComposerItem::cursorForPosition(const QPointF& itemCoordPos)
{
  QgsComposerItem::mouseMoveAction mouseAction = mouseMoveActionForPosition(itemCoordPos);
  
  if(mouseAction == QgsComposerItem::moveItem)
    {
      return Qt::ClosedHandCursor;
    }
  else if(mouseAction == QgsComposerItem::resizeDLeftUp || mouseAction == QgsComposerItem::resizeDRightDown)
    {
      return Qt::SizeFDiagCursor;
    }
  else if(mouseAction == QgsComposerItem::resizeDLeftDown || mouseAction == QgsComposerItem::resizeDRightUp)
    {
      return Qt::SizeBDiagCursor;
    }
  else if(mouseAction == QgsComposerItem::resizeUp || mouseAction == QgsComposerItem::resizeDown)
    {
      return Qt::SizeVerCursor;
    }
  else //if(mouseAction == QgsComposerItem::resizeLeft || mouseAction == QgsComposerItem::resizeRight)
    {
      return Qt::SizeHorCursor;
    }
}

QgsComposerItem::mouseMoveAction QgsComposerItem::mouseMoveActionForPosition(const QPointF& itemCoordPos)
{
  bool nearLeftBorder = false;
  bool nearRightBorder = false;
  bool nearLowerBorder = false;
  bool nearUpperBorder = false;

  if(itemCoordPos.x() < 5)
    {
      nearLeftBorder = true;
    }
  if(itemCoordPos.y() < 5)
    {
      nearUpperBorder = true;
    }
  if(itemCoordPos.x() > (rect().right() - 5))
    {
      nearRightBorder = true;
    }
  if(itemCoordPos.y() > (rect().bottom() - 5))
    {
      nearLowerBorder = true;
    }

  if(nearLeftBorder && nearUpperBorder)
    {
      return QgsComposerItem::resizeDLeftUp;
    }
  else if(nearLeftBorder && nearLowerBorder)
    {
      return QgsComposerItem::resizeDLeftDown;
    }
  else if(nearRightBorder && nearUpperBorder)
    {
      return QgsComposerItem::resizeDRightUp;
    }
  else if(nearRightBorder && nearLowerBorder)
    {
      return QgsComposerItem::resizeDRightDown;
    }
  else if(nearLeftBorder)
    {
      return QgsComposerItem::resizeLeft;
    }
  else if(nearRightBorder)
    {
      return QgsComposerItem::resizeRight;
    }
  else if(nearUpperBorder)
    {
      return QgsComposerItem::resizeUp;
    }
  else if(nearLowerBorder)
    {
      return QgsComposerItem::resizeDown;
    }

  return QgsComposerItem::moveItem; //default
}


void QgsComposerItem::rectangleChange(double dx, double dy, double& mx, double& my, double& rx, double& ry) const
{
  switch(mCurrentMouseMoveAction)
    {
      //vertical resize
    case QgsComposerItem::resizeUp:
      mx = 0; my = dy; rx = 0; ry = -dy;
      break;

    case QgsComposerItem::resizeDown:
      mx = 0; my = 0; rx = 0; ry = dy;
      break;

      //horizontal resize
    case QgsComposerItem::resizeLeft:
      mx = dx, my = 0; rx = -dx; ry = 0;
      break;

    case QgsComposerItem::resizeRight:
      mx = 0; my = 0; rx = dx, ry = 0;
      break;

      //diagonal resize
    case QgsComposerItem::resizeDLeftUp:
      mx = dx, my = dy; rx = -dx; ry = -dy;
      break;

    case QgsComposerItem::resizeDRightDown:
      mx = 0; my = 0; rx = dx, ry = dy;
      break;

    case QgsComposerItem::resizeDRightUp:
      mx = 0; my = dy, rx = dx, ry = -dy;
      break;

    case QgsComposerItem::resizeDLeftDown:
      mx = dx, my = 0; rx = -dx; ry = dy;
      break;

    case QgsComposerItem::moveItem:
      mx = dx; my = dy;
      break;
    }
}
