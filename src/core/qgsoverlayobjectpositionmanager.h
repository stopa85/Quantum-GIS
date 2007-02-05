/***************************************************************************
                         qgsoverlayobjectpositionmanager.h  -  description
                         ---------------------------------
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

#ifndef QGSOVERLAYOBJECTPOSITIONMANAGER_H
#define QGSOVERLAYOBJECTPOSITIONMANAGER_H

#include <list>

class QgsCoordinateTransform;
class QgsMapToPixel;
class QgsRect;
class QgsVectorLayer;
class QgsVectorOverlay;

/**An overlay object manager is capable to find optimal positions for vector overlay objects. It may consider
several overlay layers as well as the feature geometry and the current view extent. Different strategies for the placement may be implemented by subclasses*/
class QgsOverlayObjectPositionManager
{
 public:
  QgsOverlayObjectPositionManager(QgsVectorLayer* vl);
  void setOverlays(const std::list<QgsVectorOverlay*>& overlays){mOverlays = overlays;}
  virtual ~QgsOverlayObjectPositionManager();
  /**Does the placement of the overlay objects. The new positions are set using QgsOverlayObject::setPosition(QgsPoint)*/
  virtual void findOptimalObjectPositions(const QgsRect& viewExtent, const QgsMapToPixel * cXf, const QgsCoordinateTransform* ct) = 0;

 private:
  QgsOverlayObjectPositionManager();

 protected:
  QgsVectorLayer* mVectorLayer;
  std::list<QgsVectorOverlay*> mOverlays;
};

#endif
