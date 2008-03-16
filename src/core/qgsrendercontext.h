/***************************************************************************
                              qgsrendercontext.h    
                              ------------------
  begin                : March 16, 2008
  copyright            : (C) 2008 by Marco Hugentobler
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

#ifndef QGSRENDERCONTEXT_H
#define QGSRENDERCONTEXT_H

#include "qgscoordinatetransform.h"
#include "qgsmaptopixel.h"
#include "qgsrect.h"

/**Contains information about the context of a rendering operation*/
class QgsRenderContext
{
 public:
  QgsRenderContext();
  ~QgsRenderContext();

  //getters
  const QgsCoordinateTransform* coordTransform() const {return mCoordTransform;}

  const QgsRect* extent() const {return &mExtent;}

  const QgsMapToPixel* mapToPixel() const {return &mMapToPixel;}

  double scaleFactor() const {return mScaleFactor;}
  
  bool renderingStopped() const {return mRenderingStopped;}

  bool forceVectorOutput() const {return mForceVectorOutput;}

  bool drawEditingInformation() const {return mDrawEditingInformation;}

  //setters

  /**Sets coordinate transformation. QgsRenderContext takes ownership of the pointer and takes \
   care of releasing the memory properly*/
  void setCoordTransform(QgsCoordinateTransform* t);
  void setMapToPixel(const QgsMapToPixel& mtp) {mMapToPixel = mtp;}
  void setExtent(const QgsRect& extent){mExtent = extent;}
  void setDrawEditingInformation(bool b){mDrawEditingInformation = b;}
  void setRenderingStopped(bool stopped){mRenderingStopped = stopped;}
  void setScaleFactor(double factor){mScaleFactor = factor;}

 private:

  //Copy constructor and assignement operator forbidden
   QgsRenderContext(const QgsRenderContext& other){}
   QgsRenderContext& operator=(const QgsRenderContext&){}

  /**For transformation between coordinate systems. Can be 0 if on-the-fly reprojection is not used*/
  QgsCoordinateTransform* mCoordTransform;

  /**True if vertex markers for editing should be drawn*/
  bool mDrawEditingInformation;

  QgsRect mExtent;

  /**If true then no rendered vector elements should be cached as image*/
  bool mForceVectorOutput;
  
  QgsMapToPixel mMapToPixel;
         
  /**True if the rendering has been canceled*/
  bool mRenderingStopped;
        
  /**Factor to scale line widths and point marker sizes*/
  double mScaleFactor;
};

#endif
