/***************************************************************************
    qgssymbolrenderer.h  -  Abstract base class for the different classes
                            which render point symbols.
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
/* $Id$ */
#ifndef QGSSYMBOLRENDERER_H
#define QGSSYMBOLRENDERER_H

#include <QPainter> //was class QPainter

class QgsSymbolRenderer
{
public:
    QgsSymbolRenderer();
    virtual ~QgsSymbolRenderer();

    //appropriate stuff to set up the renderer?
    //That might all be in the implementation classes.

    virtual void render(QPainter*);
    void setSize(float size);
    float size(void);
    
protected:
    float mSize;

};//END class QgsSymbolRenderer
#endif /*QGSSYMBOLRENDERER_H*/
