/***************************************************************************
    qgssymbolrenderer.h  -  Abstract base class for the different classes
                            which render point symbols.
                             -------------------
    begin                : Thu Dec 27 2007
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
/* $Id$ */
#ifndef QGSSVGSYMBOLRENDERER_H
#define QGSSVGSYMBOLRENDERER_H

#include "qgssymbolrenderer.h"
#include <QSvgRenderer>

class QString;

class QgsSvgSymbolRenderer : public QgsSymbolRenderer
{
public:
  QgsSvgSymbolRenderer(QString file);

  void setFileName(QString file);
  void render(QPainter*); //implements pure virtual function from QgsSymbolRenderer
  
private:
  QSvgRenderer mSvgRenderer;
};

#endif /*QGSSVGSYMBOLRENDERER_H*/
