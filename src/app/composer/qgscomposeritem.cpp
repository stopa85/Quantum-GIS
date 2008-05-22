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
#include <QGraphicsSceneMouseEvent>

#include "qgscomposition.h"
#include "qgscomposeritem.h"

#include <iostream>

QgsComposerItem::QgsComposerItem(QGraphicsItem* parent): QGraphicsRectItem(0)
{
    mSelected = false;
    mPlotStyle = QgsComposition::Preview;
}

QgsComposerItem::QgsComposerItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent): QGraphicsRectItem(x, y, width, height, parent)
{
  mSelected = false;
  mPlotStyle = QgsComposition::Preview;
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
  qWarning("QgsComposerItem::mouseMoveEvent");
}

void QgsComposerItem::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
  qWarning("QgsComposerItem::mousePressEvent");
  setCursor(QCursor(evaluateCursor(event->pos())));
}

void QgsComposerItem::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
  qWarning("QgsComposerItem::mouseReleseEvent");
  setCursor(Qt::ArrowCursor);
}

Qt::CursorShape QgsComposerItem::evaluateCursor(const QPointF& itemCoordPos)
{
  return Qt::ClosedHandCursor;
}
