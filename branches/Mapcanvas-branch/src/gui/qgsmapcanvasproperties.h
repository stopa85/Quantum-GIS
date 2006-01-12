
#ifndef QGSMAPCANVASPROPERTIES_H
#define QGSMAPCANVASPROPERTIES_H

#include "qgsgeometry.h"

/**
 
   Implementation struct for QgsMapCanvas
 
  @note
 
  @DEPRECATED: to be deleted, stuff from here should be moved elsewhere

 */
class QgsMapCanvas::CanvasProperties
{
  public:

    CanvasProperties()
  : panSelectorDown( false ),
    dragging( false )
    {
    }

  //! map containing the acetate objects by key (name)
    std::map< QString, QgsAcetateObject *> acetateObjects;

  //!Flag to indicate status of mouse button
              bool mouseButtonDown;

  //! Rubber band box for dynamic zoom
              QRect zoomBox;

  //! Last seen point of the mouse
              QPoint mouseLastXY;

  //! Beginning point of a rubber band
              QPoint rubberStartPoint;

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

  //! Flag to indicate the pan selector key is held down by user
              bool panSelectorDown;

  //! Flag to indicate a map canvas drag operation is taking place
              bool dragging;
  
  //! Vector containing the inital color for a layer
              std::vector < QColor > initialColor;

  //! Value use to calculate the search radius when identifying features
  // TODO - Do we need this?
             double radiusValue;

  private:

  /** not copyable
   */
    CanvasProperties( CanvasProperties const & rhs )
    {
    // XXX maybe should be NOP just like operator=() to be consistent
      std::cerr << __FILE__ << ":" << __LINE__
          << " should not be here since CanvasProperties shouldn't be copyable\n";
    } // CanvasProperties copy ctor


  /** not copyable
   */
    CanvasProperties & operator=( CanvasProperties const & rhs )
    {
      if ( this == &rhs )
      {
        return *this;
      }

      std::cerr << __FILE__ << ":" << __LINE__
          << " should not be here since CanvasProperties shouldn't be copyable\n";

      return *this;
    } // CanvasProperties assignment operator

}
; // struct QgsMapCanvas::CanvasProperties

#endif
