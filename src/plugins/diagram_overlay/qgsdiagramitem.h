/***************************************************************************
                         qgsdiagramitem.h  -  description
                         ----------------
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

#ifndef QGSDIAGRAMITEM_H
#define QGSDIAGRAMITEM_H

/**A class to represent a classification entry for diagram symbolisation*/
class QgsDiagramItem
{
 public:
  QgsDiagramItem(double lowerBound=0.0, double upperBound=0.0, int height=0, int width=0);
  ~QgsDiagramItem();

  //getters
  double lowerBound() const {return mLowerBound;}
  double upperBound() const {return mUpperBound;}
  int height() const {return mHeight;}
  int width() const {return mWidth;}

  //setters
  void setLowerBound(double lb) {mLowerBound = lb;}
  void setUpperBound(double ub) {mUpperBound = ub;}
  void setHeight(int h) {mHeight = h;}
  void setWidth(int w) {mWidth = w;}

 protected:
  double mLowerBound;
  double mUpperBound;
  int mHeight;
  int mWidth;
};

#endif
