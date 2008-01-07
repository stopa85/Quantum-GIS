/***************************************************************************
    qgssymbolrenderer.h  -  Class which handles rendering of basic point
                            symbols, such as circles and diamonds.
                             -------------------
    begin                : Tue Dec 18 2007
    copyright            : (C) 2007 Steven Bell
    email                : botsnlinux at gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* $Id: $ */

#include "qgsbasicsymbolrenderer.h"

#include <QPainter>
#include <QString>
#include <QPolygonF>

QgsBasicSymbolRenderer::QgsBasicSymbolRenderer()
{
  mShape = CIRCLE;
}

QgsBasicSymbolRenderer::QgsBasicSymbolRenderer(QString shape)
{
  if(shape == "circle"){
    mShape = CIRCLE;
  }
  else if(shape == "square"){
    mShape = SQUARE;
  }
  else if(shape == "diamond"){
    mShape = DIAMOND;
  }
  else if(shape == "cross"){
    mShape = CROSS;
  }
  else if(shape == "cross2"){
    mShape = CROSS_2;
  }
  else if(shape == "triangle"){
    mShape = TRIANGLE;
  }
  else if(shape == "star"){
    mShape = STAR;
  }
}

QgsBasicSymbolRenderer::QgsBasicSymbolRenderer(SymbolShape shape)
{
  mShape = shape;
}

void QgsBasicSymbolRenderer::render(QPainter* p)
{
  //Don't have to set up the pen and brush, because that has been done
  //before we were called.
  QRectF symbolBoundingRect(-.5, -.5, 1, 1);
  if(mShape == CIRCLE){
      p->drawEllipse(symbolBoundingRect);
  }
  else if(mShape == SQUARE){
      p->drawRect(symbolBoundingRect);
  }
  else if(mShape == DIAMOND){
    QPolygonF pa(4);
    //Go around the diamond clockwise, starting at 12
    //note that the y-axis is flipped from a normal cartesian coordinate system
    //Is there a more effecient way to do this?
    pa[0] = QPointF(0, -.5);
    pa[1] = QPointF(.5, 0);
    pa[2] = QPointF(0, .5);
    pa[3] = QPointF(-.5, 0);
    p->drawConvexPolygon(pa);
  }
  else if(mShape == CROSS){
    p->drawLine(QLineF(0, .5, 0, -.5));
    p->drawLine(QLineF(-.5, 0, .5, 0));
  }
  else if(mShape == CROSS_2){
    p->drawLine(QLineF(-.5, .5, .5, -.5));
    p->drawLine(QLineF(-.5, -.5, .5, .5));
  }
  else if(mShape == TRIANGLE){
    QPolygonF pa(3);
    //Go around the triangle clockwise, starting at 12
    //note that the y-axis is flipped from a normal cartesian coordinate system
    //Is there a more effecient way to do this?
    pa[0] = QPointF(0, -.5);
    pa[1] = QPointF(-.5, .366);
    pa[2] = QPointF(.5, .366);
    pa << QPointF(0, -0.5);
    p->drawConvexPolygon(pa);
  }
  else if(mShape == STAR){
    QPolygonF pa(10);
    //Go around the triangle clockwise, starting at 12
    //note that the y-axis is flipped from a normal cartesian coordinate system
    //Is there a more effecient way to do this?
    
    //For a star, note that both the inner and outer sets of points form pentagons.
    //Each point in these pentagons is separated by 72 degrees.

  }
  else{ //draw a circle by default
    p->drawEllipse(symbolBoundingRect);
  }
}
