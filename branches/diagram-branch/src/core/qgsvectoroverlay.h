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
  QgsVectorOverlay(QgsVectorLayer* vl);
  virtual ~QgsVectorOverlay();
  void setAttributes(const QgsAttributeList& att){mAttributes = att;}
  virtual void createOverlayObjects(const QgsRect& viewExtent) = 0;
  /**Draws the overlay objects*/
  virtual void drawOverlayObjects(QPainter * p, const QgsRect& viewExtent, QgsMapToPixel * cXf, QgsCoordinateTransform* ct) const = 0;
  /**Returns the name of the overlay layer*/
  virtual QString name() const = 0;
  /**Gives direct access to the overlay objects. The classes derived from QgsOverlayObjectPositionManager 
need to manipulate these objects directly*/
  std::multimap<int, QgsOverlayObject*>* overlayObjects(){return &mOverlayObjects;}

 protected:
  /**The corresponding vector layer*/
  QgsVectorLayer* mVectorLayer;
  /**The attribute indexes needed by the overlay*/
  QgsAttributeList mAttributes;
  /**The positional information about the overlay objects*/
  std::multimap<int, QgsOverlayObject*> mOverlayObjects;
  //todo: have a std::multimap<int, QgsOverlayObject> mFixedObjects
  virtual int getOverlayObjectSize(int& width, int& height, double value, const QgsFeature& f) const = 0;
  //wkb helper methods
  /**Splits the WKB of multitypes into several single types. The calling method takes ownership of the created buffers. In case of single types, the wkb buffer is just copied*/
  int splitWkb(std::list<unsigned char*>& wkbGeometries, std::list<int> wkbSizes, QgsFeature& f) const;
  int splitWkbMultiPolygon(std::list<unsigned char*>& wkbGeometries, std::list<int> wkbSizes, QgsFeature& f) const;
  int splitWkbMultiLine(std::list<unsigned char*>& wkbGeometries, std::list<int> wkbSizes, QgsFeature& f) const;
  int splitWkbMultiPoint(std::list<unsigned char*>& wkbGeometries, std::list<int> wkbSizes, QgsFeature& f) const;
};

#endif
