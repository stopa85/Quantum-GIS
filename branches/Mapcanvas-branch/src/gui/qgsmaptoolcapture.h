/***************************************************************************
    qgsmaptoolcapture.h  -  map tool for capturing points, lines, polygons
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

#ifndef QGSMAPTOOLCAPTURE_H
#define QGSMAPTOOLCAPTURE_H

#include "qgsmaptool.h"
#include "qgis.h"

#include <QPoint>

class QgsMapToolCapture : public QgsMapTool
{
  public:
    QgsMapToolCapture(QgsMapCanvas* canvas, enum QGis::MapTools tool);

    //! Overridden mouse move event
    virtual void mouseMoveEvent(QMouseEvent * e);
  
    //! Overridden mouse press event
    virtual void mousePressEvent(QMouseEvent * e);
  
    //! Overridden mouse release event
    virtual void mouseReleaseEvent(QMouseEvent * e);
    
    /*  
    enum CaptureTool
    {
    CapturePoint,
    CaptureLine,
    CapturePolygon
}
    */
    
  protected:
    
    /** true if mCaptureList is used for line editing this is needed for the editing rubber band */
    bool mLineEditing;

    /** true if mCaptureList is used for polygon editing this is needed for the editing rubber band */
    bool mPolygonEditing;
    
    enum QGis::MapTools mTool;

    //! Flag to indicate a map canvas capture operation is taking place
    bool mCapturing;

    QPoint mRubberStartPoint;
    QPoint mRubberStopPoint;
};

#endif
