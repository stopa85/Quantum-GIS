/***************************************************************************
                         qgsrenderer.h  -  description
                             -------------------
    begin                : Sat Jan 4 2003
    copyright            : (C) 2003 by Gary E.Sherman
    email                : sherman at mrcc.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QGSRENDERER_H
#define QGSRENDERER_H

class OGRFeature;
class QgsCoordinateTransform;
class QPainter;



/**Abstract base class for renderers. A renderer holds all the information necessary to draw the contents of a vector layer to a map canvas. The vector layer then passes each feature to paint to the renderer*/
class QgsRenderer
{
 public:
    virtual void renderFeature(QPainter* p, OGRFeature* f, QgsCoordinateTransform* t, int endian)=0;
};

#endif // QGSRENDERER_H
