/***************************************************************************
                              qgssnapper.cpp    
                              --------------
  begin                : June 7, 2007
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

#include "qgssnapper.h"


QgsSnapper::QgsSnapper(QgsMapRender* mapRender): mMapRender(mapRender)
{

}

QgsSnapper::QgsSnapper()
{
  
}

QgsSnapper::~QgsSnapper()
{
  
}

int QgsSnapper::snapPoint(const QgsPoint& startPoint, QList<QgsSnappingResult>& snappingResult)
{
  snappingResult.clear();

  QList<QgsVectorLayer*>::const_iterator layerIt;
  for(layerIt = mLayersToSnap.constBegin(); layerIt != mLayersToSnap.constEnd(); ++layerIt)
    {
      //transform point into layer coordinates
      //do snap according to given snap mode and tolerance for the layer
      //transform start point and snap point into map coordinates to find out distance
      //depending on the snap_mode: store snap results or not
    }
  
  return 1; //soon
}
