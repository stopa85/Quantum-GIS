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

void QgsBasicSymbolRenderer::render(QPainter* p)
{
  float size = 5;
  
  //Don't have to set up the pen and brush, because that has been done
  //before we were called.
  QRectF symbolBoundingRect(-size / 2, -size / 2, size, size);
  p->drawEllipse(symbolBoundingRect);
}
