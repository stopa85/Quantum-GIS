

#ifndef QGSMAPTOOLVERTEXEDIT_H
#define QGSMAPTOOLVERTEXEDIT_H

#include "qgsmaptool.h"
#include "qgis.h"

class QgsMapToolVertexEdit : public QgsMapTool
{
  public:
    QgsMapToolVertexEdit(QgsMapCanvas* canvas, enum QGis::MapTools tool);
    
    //! Overridden mouse move event
    virtual void mouseMoveEvent(QMouseEvent * e);
  
    //! Overridden mouse press event
    virtual void mousePressEvent(QMouseEvent * e);
  
    //! Overridden mouse release event
    virtual void mouseReleaseEvent(QMouseEvent * e);

};

#endif
