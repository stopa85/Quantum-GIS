/***************************************************************************
                         qgsvectoroverlay.h  -  description
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

#ifndef QGSVECTOROVERLAY_H
#define QGSVECTOROVERLAY_H

#include <list>
#include <map>
#include "qgsoverlayobject.h"
#include "qgsvectorlayer.h"

class QPainter;
class QgsCoordinateTransform;
class QgsMapToPixel;
class QgsRect;
class QgsVectorLayer;

/**Base class for vector layer overlays (e.g. Diagrams, labels, etc.). For each object, the position of the bounding box is stored. The creation and drawing of the object contents is done by subclasses*/
class QgsVectorOverlay
{
 public:
  QgsVectorOverlay();
  virtual ~QgsVectorOverlay();
  void setAttributes(const QgsAttributeList& att){mAttributes = att;}
  virtual void createOverlayObjects(const QgsRect& viewExtent) = 0;
  /**Draws the overlay objects*/
  virtual void drawOverlayObjects(QPainter * p, const QgsRect& viewExtent, const QgsMapToPixel * cXf, \
const QgsCoordinateTransform* ct) const = 0;

 protected:
  /**The corresponding vector layer*/
  QgsVectorLayer* mVectorLayer;
  /**The attribute indexes needed by the overlay*/
  QgsAttributeList mAttributes;
  /**The positional information about the overlay objects*/
  std::multimap<int, QgsOverlayObject> mOverlayObjects;
};

#endif
