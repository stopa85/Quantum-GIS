
#include "qgsmaptoolvertexedit.h"
#include "qgsmapcanvas.h"

QgsMapToolVertexEdit::QgsMapToolVertexEdit(QgsMapCanvas* canvas, QGis::MapTools tool)
  : QgsMapTool(canvas)
{
}


void QgsMapToolVertexEdit::canvasMoveEvent(QMouseEvent * e)
{
  // TODO: if left button pressed
  /*
  case QGis::AddVertex:
  case QGis::MoveVertex:

        // TODO: Redraw rubber band

        // TODO: Qt4 will have to do this a different way, using QRubberBand ...
#if QT_VERSION < 0x040000
  paint.begin(this);
  paint.setPen(pen);
  paint.setRasterOp(Qt::XorROP);

        // XOR-out the old line
  paint.drawLine(mCanvasProperties->rubberStartPoint, mCanvasProperties->rubberMidPoint);
  paint.drawLine(mCanvasProperties->rubberMidPoint, mCanvasProperties->rubberStopPoint);

  mCanvasProperties->rubberMidPoint = e->pos();

        // XOR-in the new line
  paint.drawLine(mCanvasProperties->rubberStartPoint, mCanvasProperties->rubberMidPoint);
  paint.drawLine(mCanvasProperties->rubberMidPoint, mCanvasProperties->rubberStopPoint);

  paint.end();
#endif

  break;

} // case
  */
}

void QgsMapToolVertexEdit::canvasPressEvent(QMouseEvent * e)
{
  /*
  case QGis::AddVertex:
  {
        // Find the closest line segment to the mouse position
        // Then set up the rubber band to its endpoints

        //QgsPoint segStart;
        //QgsPoint segStop;
  QgsGeometryVertexIndex beforeVertex;
  int atFeatureId;
  QgsGeometry atGeometry;
  double x1, y1;
  double x2, y2;



  QgsPoint point = getCoordinateTransform()->toMapCoordinates(e->x(), e->y());

  QgsVectorLayer *vlayer = dynamic_cast < QgsVectorLayer * >(mCurrentLayer);


#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::mousePressEvent: QGis::AddVertex." << std::endl;
#endif

        // TODO: Find nearest segment of the selected line, move that node to the mouse location
  if (!vlayer->snapSegmentWithContext(
  point,
  beforeVertex,
  atFeatureId,
  atGeometry,
  QgsProject::instance()->readDoubleEntry("Digitizing","/Tolerance",0)
              )
           )
  {
  QMessageBox::warning(0, "Error", "Could not snap segment. Have you set the tolerance?",
  QMessageBox::Ok, Qt::NoButton);
}
  else
  {

#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::mousePressEvent: QGis::AddVertex: Snapped to segment fid " 
  << atFeatureId 
            //                << " and beforeVertex " << beforeVertex
  << "." << std::endl;
#endif

          // Save where we snapped to
  mCanvasProperties->snappedBeforeVertex = beforeVertex;
  mCanvasProperties->snappedAtFeatureId  = atFeatureId;
  mCanvasProperties->snappedAtGeometry   = atGeometry;

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
  getCoordinateTransform()->transformInPlace(x1, y1);
  mCanvasProperties->rubberStartPoint.setX( static_cast<int>( round(x1) ) );
  mCanvasProperties->rubberStartPoint.setY( static_cast<int>( round(y1) ) );

  mCanvasProperties->rubberMidPoint = e->pos();

  getCoordinateTransform()->transformInPlace(x2, y2);
  mCanvasProperties->rubberStopPoint.setX( static_cast<int>( round(x2) ) );
  mCanvasProperties->rubberStopPoint.setY( static_cast<int>( round(y2) ) );


#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::mousePressEvent: QGis::AddVertex: Transformed to widget "
  << x1 << ", " << y1 << "; "
  << x2 << ", " << y2
  << "." << std::endl;
#endif


          // TODO: Qt4 will have to do this a different way, using QRubberBand ...
#if QT_VERSION < 0x040000
          // Draw initial rubber band
  paint.begin(this);
  paint.setPen(pen);
  paint.setRasterOp(Qt::XorROP);

  paint.drawLine(mCanvasProperties->rubberStartPoint, mCanvasProperties->rubberMidPoint);
  paint.drawLine(mCanvasProperties->rubberMidPoint, mCanvasProperties->rubberStopPoint);

  paint.end();
#endif
} // if snapSegmentWithContext

  break;
}

  case QGis::MoveVertex:
  {
#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::mousePressEvent: QGis::MoveVertex." << std::endl;
#endif

        // TODO: Find nearest node of the selected line, move that node to the mouse location

        // Find the closest line segment to the mouse position
        // Then set up the rubber band to its endpoints

  QgsGeometryVertexIndex atVertex;
  int atFeatureId;
  QgsGeometry atGeometry;
  double x1, y1;
  double x2, y2;

  QgsPoint point = getCoordinateTransform()->toMapCoordinates(e->x(), e->y());

  QgsVectorLayer *vlayer = dynamic_cast < QgsVectorLayer * >(mCurrentLayer);

#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::mousePressEvent: QGis::MoveVertex." << std::endl;
#endif

        // TODO: Find nearest segment of the selected line, move that node to the mouse location
  if (!vlayer->snapVertexWithContext(
  point,
  atVertex,
  atFeatureId,
  atGeometry,
  QgsProject::instance()->readDoubleEntry("Digitizing","/Tolerance",0)
              )
           )
  {
  QMessageBox::warning(0, "Error", "Could not snap vertex. Have you set the tolerance?",
  QMessageBox::Ok, Qt::NoButton);
}
  else
  {

#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::mousePressEvent: QGis::MoveVertex: Snapped to segment fid " 
  << atFeatureId 
            //                << " and beforeVertex " << beforeVertex
  << "." << std::endl;
#endif

          // Save where we snapped to
  mCanvasProperties->snappedAtVertex     = atVertex;
  mCanvasProperties->snappedAtFeatureId  = atFeatureId;
  mCanvasProperties->snappedAtGeometry   = atGeometry;

          // Get the startpoint of the rubber band, as the previous vertex to the snapped-to one.
  atVertex.decrement_back();
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
  getCoordinateTransform()->transformInPlace(x1, y1);
  mCanvasProperties->rubberStartPoint.setX( static_cast<int>( round(x1) ) );
  mCanvasProperties->rubberStartPoint.setY( static_cast<int>( round(y1) ) );
}

  mCanvasProperties->rubberMidPoint = e->pos();

  if (mCanvasProperties->rubberStopPointIsValid)
  {
  getCoordinateTransform()->transformInPlace(x2, y2);
  mCanvasProperties->rubberStopPoint.setX( static_cast<int>( round(x2) ) );
  mCanvasProperties->rubberStopPoint.setY( static_cast<int>( round(y2) ) );
}

#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::mousePressEvent: QGis::MoveVertex: Transformed to widget "
  << x1 << ", " << y1 << "; "
  << x2 << ", " << y2
  << "." << std::endl;
#endif

          // TODO: Qt4 will have to do this a different way, using QRubberBand ...
#if QT_VERSION < 0x040000
          // Draw initial rubber band
  paint.begin(this);
  paint.setPen(pen);
  paint.setRasterOp(Qt::XorROP);

  if (mCanvasProperties->rubberStartPointIsValid)
  {
  paint.drawLine(mCanvasProperties->rubberStartPoint, mCanvasProperties->rubberMidPoint);
}
  if (mCanvasProperties->rubberStopPointIsValid)
  {
  paint.drawLine(mCanvasProperties->rubberMidPoint, mCanvasProperties->rubberStopPoint);
}

  paint.end();
#endif
} // if snapVertexWithContext


  break;
}

  case QGis::DeleteVertex:
  {
#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::mousePressEvent: QGis::DeleteVertex." << std::endl;
#endif

        // TODO: Find nearest node of the selected line, show a big X symbol

  QgsGeometryVertexIndex atVertex;
  int atFeatureId;
  QgsGeometry atGeometry;
  double x1, y1;

  QgsPoint point = getCoordinateTransform()->toMapCoordinates(e->x(), e->y());

  QgsVectorLayer *vlayer = dynamic_cast < QgsVectorLayer * >(mCurrentLayer);

#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::mousePressEvent: QGis::DeleteVertex." << std::endl;
#endif

        // TODO: Find nearest segment of the selected line, move that node to the mouse location
  if (!vlayer->snapVertexWithContext(
  point,
  atVertex,
  atFeatureId,
  atGeometry,
  QgsProject::instance()->readDoubleEntry("Digitizing","/Tolerance",0)
              )
            // TODO: What if there is no snapped vertex?
           )
  {
  QMessageBox::warning(0, "Error", "Could not snap vertex. Have you set the tolerance?",
  QMessageBox::Ok, Qt::NoButton);
}
  else
  {

#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::mousePressEvent: QGis::DeleteVertex: Snapped to segment fid " 
  << atFeatureId 
            //                << " and beforeVertex " << beforeVertex
  << "." << std::endl;
#endif

          // Save where we snapped to
  mCanvasProperties->snappedAtVertex     = atVertex;
  mCanvasProperties->snappedAtFeatureId  = atFeatureId;
  mCanvasProperties->snappedAtGeometry   = atGeometry;

          // Get the point of the snapped-to vertex
  atGeometry.vertexAt(x1, y1, atVertex);

#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::mousePressEvent: QGis::DeleteVertex: Snapped to vertex "
  << x1 << ", " << y1
  << "." << std::endl;
#endif

          // Convert to canvas screen coordinates
  getCoordinateTransform()->transformInPlace(x1, y1);
  mCanvasProperties->rubberMidPoint.setX( static_cast<int>( round(x1) ) );
  mCanvasProperties->rubberMidPoint.setY( static_cast<int>( round(y1) ) );

#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::mousePressEvent: QGis::DeleteVertex: Transformed to widget "
  << x1 << ", " << y1
  << "." << std::endl;
#endif

          // TODO: Qt4 will have to do this a different way, using QRubberBand ...
#if QT_VERSION < 0x040000
          // Draw X symbol - people can feel free to pretty this up if they like
  paint.begin(this);
  paint.setPen(pen);
  paint.setRasterOp(Qt::XorROP);

          // TODO: Make the following a static member or something
  int crossSize = 10;

  paint.drawLine( mCanvasProperties->rubberMidPoint.x() - crossSize,
  mCanvasProperties->rubberMidPoint.y() - crossSize,
  mCanvasProperties->rubberMidPoint.x() + crossSize,
  mCanvasProperties->rubberMidPoint.y() + crossSize  );

  paint.drawLine( mCanvasProperties->rubberMidPoint.x() - crossSize,
  mCanvasProperties->rubberMidPoint.y() + crossSize,
  mCanvasProperties->rubberMidPoint.x() + crossSize,
  mCanvasProperties->rubberMidPoint.y() - crossSize  );

  paint.end();
#endif
} // if snapVertexWithContext


  break;
}
  */
}

void QgsMapToolVertexEdit::canvasReleaseEvent(QMouseEvent * e)
{
  /*
    // map tools that don't care if clicked or dragged
  switch (mMapTool)
  {
  case QGis::AddVertex:
  {
  QgsPoint point = getCoordinateTransform()->toMapCoordinates(e->x(), e->y());

  QgsVectorLayer *vlayer = dynamic_cast < QgsVectorLayer * >(mCurrentLayer);


#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::mouseReleaseEvent: QGis::AddVertex." << std::endl;
#endif

        // TODO: Find nearest line portion of the selected line, add a node at the mouse location

        // TODO: Qt4 will have to do this a different way, using QRubberBand ...
#if QT_VERSION < 0x040000
        // Undraw rubber band
  paint.begin(this);
  paint.setPen(pen);
  paint.setRasterOp(Qt::XorROP);

        // XOR-out the old line
  paint.drawLine(mCanvasProperties->rubberStartPoint, mCanvasProperties->rubberMidPoint);
  paint.drawLine(mCanvasProperties->rubberMidPoint, mCanvasProperties->rubberStopPoint);
  paint.end();
#endif

        // Add the new vertex

#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::mouseReleaseEvent: About to vlayer->insertVertexBefore." << std::endl;
#endif

  if (vlayer)
  {

          // only do the rest for provider with geometry modification support
          // TODO: Move this test earlier into the workflow, maybe by triggering just after the user selects "Add Vertex" or even by graying out the menu option.
  if (vlayer->getDataProvider()->capabilities() & QgsVectorDataProvider::ChangeGeometries)
  {
  if ( !vlayer->isEditable() )
  {
  QMessageBox::information(0,"Layer not editable",
  "Cannot edit the vector layer. Use 'Start editing' in the legend item menu",
  QMessageBox::Ok);
  break;
}

  vlayer->insertVertexBefore(
  point.x(), point.y(), 
  mCanvasProperties->snappedAtFeatureId,
  mCanvasProperties->snappedBeforeVertex);


#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::mouseReleaseEvent: Completed vlayer->insertVertexBefore." << std::endl;
#endif
}
}

        // TODO: Redraw?  
  break;
}  

  case QGis::MoveVertex:
  {
#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::mouseReleaseEvent: QGis::MoveVertex." << std::endl;
#endif
  QgsPoint point = getCoordinateTransform()->toMapCoordinates(e->x(), e->y());

  QgsVectorLayer *vlayer = dynamic_cast < QgsVectorLayer * >(mCurrentLayer);

        // TODO: Qt4 will have to do this a different way, using QRubberBand ...
#if QT_VERSION < 0x040000
        // Undraw rubber band
  paint.begin(this);
  paint.setPen(pen);
  paint.setRasterOp(Qt::XorROP);

        // XOR-out the old line
  paint.drawLine(mCanvasProperties->rubberStartPoint, mCanvasProperties->rubberMidPoint);
  paint.drawLine(mCanvasProperties->rubberMidPoint, mCanvasProperties->rubberStopPoint);
  paint.end();
#endif

        // Move the vertex

#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::mouseReleaseEvent: About to vlayer->moveVertexAt." << std::endl;
#endif

  if (vlayer)
  {
          // TODO: Move this test earlier into the workflow, maybe by triggering just after the user selects "Move Vertex" or even by graying out the menu option.
  if (vlayer->getDataProvider()->capabilities() & QgsVectorDataProvider::ChangeGeometries)
  {
  if ( !vlayer->isEditable() )
  {
  QMessageBox::information(0,"Layer not editable",
  "Cannot edit the vector layer. Use 'Start editing' in the legend item menu",
  QMessageBox::Ok);
  break;
}

  vlayer->moveVertexAt(
  point.x(), point.y(),
  mCanvasProperties->snappedAtFeatureId,
  mCanvasProperties->snappedAtVertex);

#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::mouseReleaseEvent: Completed vlayer->moveVertexAt." << std::endl;
#endif
}
}
        // TODO: Redraw?  

  break;
}  

  case QGis::DeleteVertex:
  {
#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::mouseReleaseEvent: QGis::DeleteVertex." << std::endl;
#endif
  QgsVectorLayer *vlayer = dynamic_cast < QgsVectorLayer * >(mCurrentLayer);

        // TODO: Qt4 will have to do this a different way, using QRubberBand ...
#if QT_VERSION < 0x040000
        // Undraw X symbol - people can feel free to pretty this up if they like
  paint.begin(this);
  paint.setPen(pen);
  paint.setRasterOp(Qt::XorROP);

        // TODO: Make the following a static member or something
  int crossSize = 10;

  paint.drawLine( mCanvasProperties->rubberMidPoint.x() - crossSize,
  mCanvasProperties->rubberMidPoint.y() - crossSize,
  mCanvasProperties->rubberMidPoint.x() + crossSize,
  mCanvasProperties->rubberMidPoint.y() + crossSize  );

  paint.drawLine( mCanvasProperties->rubberMidPoint.x() - crossSize,
  mCanvasProperties->rubberMidPoint.y() + crossSize,
  mCanvasProperties->rubberMidPoint.x() + crossSize,
  mCanvasProperties->rubberMidPoint.y() - crossSize  );

  paint.end();
#endif

  if (vlayer)
  {
          // TODO: Move this test earlier into the workflow, maybe by triggering just after the user selects "Delete Vertex" or even by graying out the menu option.
  if (vlayer->getDataProvider()->capabilities() & QgsVectorDataProvider::ChangeGeometries)
  {
  if ( !vlayer->isEditable() )
  {
  QMessageBox::information(0,"Layer not editable",
  "Cannot edit the vector layer. Use 'Start editing' in the legend item menu",
  QMessageBox::Ok);
  break;
}

  vlayer->deleteVertexAt(
  mCanvasProperties->snappedAtFeatureId,
  mCanvasProperties->snappedAtVertex);

#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::mouseReleaseEvent: Completed vlayer->deleteVertexAt." << std::endl;
#endif
}
}
        // TODO: Redraw?  

  break;
}
  */
}
