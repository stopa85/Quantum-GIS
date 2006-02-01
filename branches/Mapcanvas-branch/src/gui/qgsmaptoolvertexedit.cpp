/***************************************************************************
    qgsmaptoolvertexedit.cpp  - tool for adding, moving, deleting vertices
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

#include "qgsmaptoolvertexedit.h"
#include "qgsmapcanvas.h"
#include "qgsrubberband.h"
#include "qgsvectorlayer.h"
#include "qgsvectordataprovider.h"
#include "qgsmaptopixel.h"
#include "qgsproject.h"
#include <QMessageBox>

QgsMapToolVertexEdit::QgsMapToolVertexEdit(QgsMapCanvas* canvas, QGis::MapTools tool)
  : QgsMapTool(canvas)
{
  if (tool == QGis::AddVertex)
    mTool = AddVertex;
  else if (tool == QGis::MoveVertex)
    mTool = MoveVertex;
  else if (tool == QGis::DeleteVertex)
    mTool = DeleteVertex;
}


void QgsMapToolVertexEdit::canvasMoveEvent(QMouseEvent * e)
{
  if (e->button() == Qt::LeftButton && mRubberBand &&
      (mTool == AddVertex || mTool == MoveVertex))
  {
  
#ifdef QGISDEBUG
        qWarning("Moving rubber band for moveVertex");
#endif
        mRubberBand->movePoint(1, e->pos());
  }

}


void QgsMapToolVertexEdit::canvasPressEvent(QMouseEvent * e)
{
  //QgsVectorLayer* vlayer = dynamic_cast<QgsVectorLayer*>(mCanvas->currentLayer());
  
  QgsPoint point = mCanvas->getCoordinateTransform()->toMapCoordinates(e->x(), e->y());
  
  double x1, y1;
  double x2, y2;
  
  if (mTool == AddVertex)
  {
    // Find the closest line segment to the mouse position
    // Then set up the rubber band to its endpoints

#ifdef QGISDEBUG
        std::cout << "QgsMapCanvas::mousePressEvent: QGis::AddVertex." << std::endl;
#endif

    // TODO: Find nearest segment of the selected line, move that node to the mouse location
    if (!snapVertexWithContext(point))
      return;
/*
    // Get the endpoint of the snapped-to segment
    atGeometry.vertexAt(x2, y2, beforeVertex);

    // Get the startpoint of the snapped-to segment
    beforeVertex.decrement_back();
    atGeometry.vertexAt(x1, y1, beforeVertex);


#ifdef QGISDEBUG
    std::cout << "QgsMapCanvas::mousePressEvent: QGis::AddVertex: Snapped to segment "
      << x1 << ", " << y1 << "; "
      << x2 << ", " << y2
      << "." << std::endl;
#endif

    // Convert to canvas screen coordinates for rubber band
    mCanvasProperties->coordXForm->transformInPlace(x1, y1);
    mCanvasProperties->rubberStartPoint.setX( static_cast<int>( round(x1) ) );
    mCanvasProperties->rubberStartPoint.setY( static_cast<int>( round(y1) ) );

    mCanvasProperties->rubberMidPoint = e->pos();

    mCanvasProperties->coordXForm->transformInPlace(x2, y2);
    mCanvasProperties->rubberStopPoint.setX( static_cast<int>( round(x2) ) );
    mCanvasProperties->rubberStopPoint.setY( static_cast<int>( round(y2) ) );

#ifdef QGISDEBUG
    std::cout << "QgsMapCanvas::mousePressEvent: QGis::AddVertex: Transformed to widget "
      << x1 << ", " << y1 << "; " << x2 << ", " << y2 << "." << std::endl;
#endif*/

  }
  else if (mTool == MoveVertex)
  {
#ifdef QGISDEBUG
    std::cout << "QgsMapCanvas::mousePressEvent: QGis::MoveVertex." << std::endl;
#endif

    // Find the closest line segment to the mouse position
    // Then set up the rubber band to its endpoints

    // TODO: Find nearest segment of the selected line, move that node to the mouse location
    
    if (!snapVertexWithContext(point))
      return;

    // Get the startpoint of the rubber band, as the previous vertex to the snapped-to one.
/*    atVertex.decrement_back();
    mCanvasProperties->rubberStartPointIsValid = atGeometry.vertexAt(x1, y1, atVertex);

    // Get the endpoint of the rubber band, as the following vertex to the snapped-to one.
    atVertex.increment_back();
    atVertex.increment_back();
    mCanvasProperties->rubberStopPointIsValid = atGeometry.vertexAt(x2, y2, atVertex);

#ifdef QGISDEBUG
    std::cout << "QgsMapCanvas::mousePressEvent: QGis::MoveVertex: Snapped to vertex "
      << "(valid = " << mCanvasProperties->rubberStartPointIsValid << ") " << x1 << ", " << y1 << "; "
      << "(valid = " << mCanvasProperties->rubberStopPointIsValid  << ") " << x2 << ", " << y2
      << "." << std::endl;
#endif

    // Convert to canvas screen coordinates for rubber band
    if (mCanvasProperties->rubberStartPointIsValid)
    {
      mCanvasProperties->coordXForm->transformInPlace(x1, y1);
      mCanvasProperties->rubberStartPoint.setX( static_cast<int>( round(x1) ) );
      mCanvasProperties->rubberStartPoint.setY( static_cast<int>( round(y1) ) );
    }

    mCanvasProperties->rubberMidPoint = e->pos();

    if (mCanvasProperties->rubberStopPointIsValid)
    {
      mCanvasProperties->coordXForm->transformInPlace(x2, y2);
      mCanvasProperties->rubberStopPoint.setX( static_cast<int>( round(x2) ) );
      mCanvasProperties->rubberStopPoint.setY( static_cast<int>( round(y2) ) );
  }

#ifdef QGISDEBUG
    std::cout << "QgsMapCanvas::mousePressEvent: QGis::MoveVertex: Transformed to widget "
      << x1 << ", " << y1 << "; "
      << x2 << ", " << y2
      << "." << std::endl;
#endif*/

#ifdef QGISDEBUG
    qWarning("Creating rubber band for moveVertex");
#endif

    // FIXME: will this work?
    mSnappedAtGeometry.vertexAt(x1, y1, mSnappedAtVertex);
    x2 = x1; y2 = y1;

    mRubberBand = new QgsRubberBand(mCanvas, /*mPolygonEditing*/ FALSE);
    mRubberBand->addPoint(QPoint(static_cast<int>(round(x1)), static_cast<int>(round(y1))));
    mRubberBand->addPoint(e->pos());
    mRubberBand->addPoint(QPoint(static_cast<int>(round(x2)), static_cast<int>(round(y2))));
    QgsProject* project = QgsProject::instance();
    QColor color( project->readNumEntry("Digitizing", "/LineColorRedPart", 255),
                  project->readNumEntry("Digitizing", "/LineColorGreenPart", 0),
                  project->readNumEntry("Digitizing", "/LineColorBluePart", 0));
    mRubberBand->setColor(color);
    mRubberBand->setWidth(project->readNumEntry("Digitizing", "/LineWidth", 1));
    mRubberBand->show();
      
  }
  else if (mTool == DeleteVertex)
  {
#ifdef QGISDEBUG
    std::cout << "QgsMapCanvas::mousePressEvent: QGis::DeleteVertex." << std::endl;
#endif

    // TODO: Find nearest node of the selected line, show a big X symbol
  
    // TODO: Find nearest segment of the selected line, move that node to the mouse location
        
    if (!snapVertexWithContext(point))
      return;
/*      
    // Get the point of the snapped-to vertex
    atGeometry.vertexAt(x1, y1, atVertex);
    
#ifdef QGISDEBUG
    std::cout << "QgsMapCanvas::mousePressEvent: QGis::DeleteVertex: Snapped to vertex "
      << x1 << ", " << y1 << "." << std::endl;
#endif

    // Convert to canvas screen coordinates
    mCanvasProperties->coordXForm->transformInPlace(x1, y1);
    mCanvasProperties->rubberMidPoint.setX( static_cast<int>( round(x1) ) );
    mCanvasProperties->rubberMidPoint.setY( static_cast<int>( round(y1) ) );

#ifdef QGISDEBUG
    std::cout << "QgsMapCanvas::mousePressEvent: QGis::DeleteVertex: Transformed to widget "
      << x1 << ", " << y1 << "." << std::endl;
#endif*/
  
  }
  
}

bool QgsMapToolVertexEdit::snapVertexWithContext(QgsPoint& point)
{
  QgsVectorLayer* vlayer = dynamic_cast<QgsVectorLayer*>(mCanvas->currentLayer());
  double tolerance = QgsProject::instance()->readDoubleEntry("Digitizing","/Tolerance",0);
  
  QgsGeometryVertexIndex atVertex;
  int atFeatureId;
  QgsGeometry atGeometry;
  
  if (!vlayer)
    return FALSE;
  
  if (!vlayer->snapVertexWithContext(point, atVertex, atFeatureId, atGeometry, tolerance))
  {
    QMessageBox::warning(0, "Error", "Could not snap vertex. Have you set the tolerance?",
                         QMessageBox::Ok, Qt::NoButton);
    return FALSE;
  }
  else
  {
#ifdef QGISDEBUG
      std::cout << "QgsMapToolVertexEdit: Snapped to segment fid " << atFeatureId << "." << std::endl;
#endif
    
    // Save where we snapped to
    mSnappedAtVertex     = atVertex;
    mSnappedAtFeatureId  = atFeatureId;
    mSnappedAtGeometry   = atGeometry;
    return TRUE;
  }
}


void QgsMapToolVertexEdit::canvasReleaseEvent(QMouseEvent * e)
{
  QgsVectorLayer* vlayer = dynamic_cast<QgsVectorLayer*>(mCanvas->currentLayer());
  
  if (!vlayer)
  {
    QMessageBox::information(0,"Not a vector layer","The current layer is not a vector layer",QMessageBox::Ok);
    return;
  }
  
  if (vlayer->getDataProvider()->capabilities() & QgsVectorDataProvider::ChangeGeometries)
  {
    QMessageBox::information(0,"Change geometry",
                             "Data provider of the current layer doesn't allow changing geometries",
                             QMessageBox::Ok);
    return;
  }
  
  if (!vlayer->isEditable())
  {
    QMessageBox::information(0,"Layer not editable",
                              "Cannot edit the vector layer. Use 'Start editing' in the legend item menu",
                              QMessageBox::Ok);
    return;
  }
  
  QgsPoint point = mCanvas->getCoordinateTransform()->toMapCoordinates(e->x(), e->y());
  
  if (mTool == AddVertex)
  {

#ifdef QGISDEBUG
    std::cout << "QgsMapToolVertexEdit::canvasReleaseEvent: AddVertex." << std::endl;
#endif

    // TODO: Find nearest line portion of the selected line, add a node at the mouse location
  
    // Add the new vertex
    vlayer->insertVertexBefore(point.x(), point.y(), mSnappedAtFeatureId, mSnappedBeforeVertex);
        
  }
  else if (mTool == MoveVertex)
  {
#ifdef QGISDEBUG
    std::cout << "QgsMapToolVertexEdit::canvasReleaseEvent: MoveVertex." << std::endl;
#endif

    delete mRubberBand;
    mRubberBand = 0;
  
    vlayer->moveVertexAt(point.x(), point.y(), mSnappedAtFeatureId, mSnappedAtVertex);
  }
  else if (mTool == DeleteVertex)
  {
#ifdef QGISDEBUG
    std::cout << "QgsMapToolVertexEdit::canvasReleaseEvent: DeleteVertex." << std::endl;
#endif

    // delete vertex
    vlayer->deleteVertexAt(mSnappedAtFeatureId, mSnappedAtVertex);
  }
}
