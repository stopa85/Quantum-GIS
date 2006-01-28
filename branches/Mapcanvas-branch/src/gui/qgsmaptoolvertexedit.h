

#ifndef QGSMAPTOOLVERTEXEDIT_H
#define QGSMAPTOOLVERTEXEDIT_H

#include "qgsmaptool.h"
#include "qgis.h"

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
/*    
    //! Is the beginning point of a rubber band valid?  (If not, this segment of the rubber band will not be drawn)
    bool rubberStartPointIsValid;

    //! Mid point of a rubber band
    QPoint rubberMidPoint;

    //! End point of a rubber band
    QPoint rubberStopPoint;

    //! Is the end point of a rubber band valid?  (If not, this segment of the rubber band will not be drawn)
    bool rubberStopPointIsValid;

    //! The snapped-to segment before this vertex number (identifying the vertex that is being moved)
    QgsGeometryVertexIndex snappedAtVertex;

    //! The snapped-to segment before this vertex number (identifying the segment that a new vertex is being added to)
    QgsGeometryVertexIndex snappedBeforeVertex;

    //! The snapped-to feature ID
    int snappedAtFeatureId;

    //! The snapped-to geometry
    QgsGeometry snappedAtGeometry;
*/
};

#endif
