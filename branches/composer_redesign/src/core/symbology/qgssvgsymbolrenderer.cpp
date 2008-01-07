/***************************************************************************
    qgssymbolrenderer.h  -  Class which handles rendering of svg point
                            symbols.
                             -------------------
    begin                : Tue Dec 27 2007
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

#include "qgssvgsymbolrenderer.h"

#include <QPainter>
#include <QString>

QgsSvgSymbolRenderer::QgsSvgSymbolRenderer(QString file)
{
  mSvgRenderer.load(file);
}

void QgsSvgSymbolRenderer::setFileName(QString file)
{
  mSvgRenderer.load(file);
}

void QgsSvgSymbolRenderer::render(QPainter* p)
{
  QRectF renderArea(-.5, -.5, 1, 1);
  mSvgRenderer.render(p, renderArea);
}
