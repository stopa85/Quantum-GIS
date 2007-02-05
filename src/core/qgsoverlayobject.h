/***************************************************************************
                         qgsoverlayobject.h  -  description
                         ------------------------------
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

#ifndef QGSOVERLAYOBJECT_H
#define QGSOVERLAYOBJECT_H

#include "qgspoint.h"
class QgsFeature;

/**A class that holds information about the position of an overlay object, such as
the size of the bounding box (in screen corrdinages) and the position of the midpoint 
(in world coordinates). It also contains a pointer to the vector feature it belongs to*/
class QgsOverlayObject
{
 public:
  QgsOverlayObject(int height = 0, int width = 0, const QgsPoint* p = 0, bool fixed = false, unsigned char* wkb = 0, int wkbSize = 0, int geometryNr = 0);
  ~QgsOverlayObject();

  //copy constructor and operator= necessary because of mWKB
  QgsOverlayObject(const QgsOverlayObject& other);
  QgsOverlayObject& operator=(const QgsOverlayObject& other);

  //getters
  int height() const {return mHeight;}
  int width() const {return mWidth;}
  const QgsPoint& position() const {return mPosition;}
  bool fixed() const {return mFixed;}
  const unsigned char* wkb() const {return mWKB;}
  int wkbSize() const {return mWKBSize;}
  int geometryNr() const {return mGeometryNr;}
  
  //setters
  void setHeight(int height){mHeight = height;}
  void setWidth(int width){mWidth = width;}
  void setFixed(bool f){mFixed = f;}
  void setPosition(const QgsPoint& p){mPosition = p;}
  /**Sets the geometry of this feature. The class takes ownership and deletes it if necessary*/
  void setWkb(unsigned char* wkb);
  void setWkbSize(int size){mWKBSize = size;}
  void setGeometryNr(int nr){mGeometryNr = nr;}

 private:
  /**Height of the bounding box in pixels*/
  int mHeight;
  /**Width of the bounding box in pixels*/
  int mWidth;
  /**Position of the object in world coordinates*/
  QgsPoint mPosition;
  /**Flag if the position of the object may be changed*/
  bool mFixed;
  /**Pointer to the corresponding feature geometry*/
  unsigned char* mWKB;
  /**Size of geometry*/
  int mWKBSize;
  /**For multipoint/multiline/multipolygon: geometry number*/
  int mGeometryNr;
};

#endif
