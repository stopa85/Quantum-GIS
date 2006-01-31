/***************************************************************************
    qgsmaptoolvertexedit.h  - tool for adding, moving, deleting vertices
    ---------------------
    begin                : January 2006
    copyright            : (C) 2006 by Martin Dobias
    email                : wonder.sk at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* $Id$ */

#ifndef QGSMAPTOOLVERTEXEDIT_H
#define QGSMAPTOOLVERTEXEDIT_H

#include "qgsmaptool.h"
#include "qgis.h"
#include "qgsgeometry.h"

class QgsRubberBand;

class QgsMapToolVertexEdit : public QgsMapTool
{
  public:
    QgsMapToolVertexEdit(QgsMapCanvas* canvas, enum QGis::MapTools tool);
    
    //! Overridden mouse move event
    virtual void canvasMoveEvent(QMouseEvent * e);
  
    //! Overridden mouse press event
    virtual void canvasPressEvent(QMouseEvent * e);
  
    //! Overridden mouse release event
    virtual void canvasReleaseEvent(QMouseEvent * e);

  private:
    
    //! current vertex edit tool
    enum Tool
    {
      AddVertex,
      MoveVertex,
      DeleteVertex
    } mTool;
    
    bool snapVertexWithContext(QgsPoint& point);
    
    //! The snapped-to segment before this vertex number (identifying the vertex that is being moved)
    QgsGeometryVertexIndex mSnappedAtVertex;

    //! The snapped-to segment before this vertex number (identifying the segment that a new vertex is being added to)
    QgsGeometryVertexIndex mSnappedBeforeVertex;

    //! The snapped-to feature ID
    int mSnappedAtFeatureId;

    //! The snapped-to geometry
    QgsGeometry mSnappedAtGeometry;
    
    QgsRubberBand* mRubberBand;
    
/*    
    //! Is the beginning point of a rubber band valid?  (If not, this segment of the rubber band will not be drawn)
    bool rubberStartPointIsValid;

    //! Mid point of a rubber band
    QPoint rubberMidPoint;

    //! End point of a rubber band
    QPoint rubberStopPoint;

    //! Is the end point of a rubber band valid?  (If not, this segment of the rubber band will not be drawn)
    bool rubberStopPointIsValid;

*/
};

#endif
