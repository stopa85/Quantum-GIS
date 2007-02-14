/***************************************************************************
                         qgsdiagramitem.cpp  -  description
                         ------------------
    begin                : January 2007
    copyright            : (C) 2007 by Marco Hugentobler
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

#include "qgsdiagramitem.h"

QgsDiagramItem::QgsDiagramItem(double lowerBound, double upperBound, int height, int width): mLowerBound(lowerBound), mUpperBound(upperBound), mHeight(height), mWidth(width)
{

}

QgsDiagramItem::~QgsDiagramItem()
{

}
