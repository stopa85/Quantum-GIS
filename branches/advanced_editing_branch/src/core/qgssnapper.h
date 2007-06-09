/***************************************************************************
                              qgssnapper.h    
                              ------------
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

#ifndef QGSSNAPPER_H
#define QGSSNAPPER_H

#include "qgspoint.h"
#include <QList>

class QgsGeometry;
class QgsMapRender;
class QgsVectorLayer;

/**Describes the result of a snapping operation*/
struct QgsSnappingResult
{
  /**The layer coordinates of the snapping result*/
  QgsPoint snappedVertex;
  /**The vertex index of snappedVertex*/
  int snappedVertexNr;
  /**The layer coordinates of the vertex before snappedVertex*/
  QgsPoint beforeVertex;
  /**The layer coordinates of the vertex after snappedVertex*/
  QgsPoint afterVertex;
  /**A pointer to the snapped geometry*/
  QgsGeometry* snappedAtGeometry;
};



/**A class that allows advanced snapping operations on a set of vector layers*/
class QgsSnapper
{
 public:
  /**Snap to vertex or snap to segment*/
  enum SNAP_TO
    {
      SNAP_TO_VERTEX,
      SNAP_TO_SEGMENT
    };
  
  enum SNAP_MODE
    {
      /**Only one snapping result is returnd*/
      ONE_RESULT,
      /**One result, determined by first snap to segment and then snap to closest vertex of segment.
       This is usefull if only one polygon vertex of a border has to be shifted and it should be
      the polygon that is closer to the cursor*/
      ONE_RESULT_BY_SEGMENT,
      /**Several snapping results which have the same position are returned. This is usefull for topological
       editing*/
      SEVERAL_RESULTS_SAME_POSITION,
      /**All results within the given layer tolerances are returned*/
      ALL_RESULTS_WITHIN_GIVEN_TOLERANCES
    };

  QgsSnapper(QgsMapRender* mapRender);
  ~QgsSnapper();
  /**Does the snapping operation
   @param startPoint the start point for snapping (in pixel coordinates)
  @param snappingResult the list where the results are inserted
  @return 0 in case of success*/
  int snapPoint(const QgsPoint& startPoint, QList<QgsSnappingResult>& snappingResult);

  //todo: setters and getters
 private:
  /**Don't use the default constructor*/
  QgsSnapper();
  /**The maprender object contains information about the output coordinate system
   of the map and about the relationship between pixel space and map space*/
  QgsMapRender* mMapRender;
  /**Snap mode to apply*/
  QgsSnapper::SNAP_MODE mSnapMode;
  /**The layers to which snapping is applied*/
  QList<QgsVectorLayer*> mLayersToSnap;
  /**The snapping tolerances for the layers. The order must correspond to the layer list.
   Note that the tolerances are always in source coordinate systems of the layers*/
  QList<double> mSnappingTolerances;
  /**List if snap to segment of to vertex. The order must correspond to the layer list*/
  QList<QgsSnapper::SNAP_TO> mSnapToList;
};

#endif
