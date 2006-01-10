
#ifndef QGSMAPCANVASPROPERTIES_H
#define QGSMAPCANVASPROPERTIES_H

#ifndef QPAINTDEVICEMETRICS_H
#include <q3paintdevicemetrics.h>
#endif

#include "qgsgeometry.h"
#include "qgsmaptopixel.h"
#include "qgsvectorlayer.h"
#include "qgsproject.h"
//Added by qt3to4:
#include <QPixmap>


/**
 
   Implementation struct for QgsMapCanvas
 
  @note
 
  Changed to class from struct out of desperation to find workaround for g++ bug.
 
 */
class QgsMapCanvas::CanvasProperties
{
  public:

    CanvasProperties( int width, int height )
  : mapWindow( 0x0 ),
    coordXForm( 0x0 ),
    panSelectorDown( false ),
    dragging( false ),
    capturing( false ),
    drawing( false ),
    panning( false ),
    frozen( false ),
    dirty( true )
    {
      mapWindow = new QRect;
      coordXForm = new QgsMapToPixel;
    }

    CanvasProperties()
  : mapWindow( 0x0 ),
    coordXForm( 0x0 ),
    panSelectorDown( false ),
    dragging( false ),
    capturing( false ),
    drawing( false ),
    panning( false ),
    frozen( false ),
    dirty( true )
    {
      mapWindow = new QRect;
    }


    ~CanvasProperties()
    {
      delete mapWindow;
    } // ~CanvasProperties


  //! map containing the acetate objects by key (name)
    std::map< QString, QgsAcetateObject *> acetateObjects;

  //! Map window rectangle
  //std::auto_ptr<QRect> mapWindow;
    QRect * mapWindow;

  /** Pointer to the coordinate transform object used to transform
    coordinates from real world to device coordinates
   */
  //std::auto_ptr<QgsMapToPixel> coordXForm;
    QgsMapToPixel * coordXForm;

  /** The output spatial reference system that was used most
    recently. Obtained from a layer on this canvas
   */
    QgsSpatialRefSys previousOutputSRS;

  /**
     * \brief Currently selected map tool.
               * @see QGis::MapTools enum for valid values
   */
                  int mapTool;

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

  //! Flag to indicate a map canvas capture operation is taking place
              bool capturing;
  
  //! Vector containing the inital color for a layer
              std::vector < QColor > initialColor;

  //! Flag indicating a map refresh is in progress
              bool drawing;

  //! Flag indicating the map is being dragged in order to pan it
              bool panning;

  //! Amount of pixels we dragged since the pan attempt started
              QPoint pan_delta;

  //! Flag indicating if the map canvas is frozen.
              bool frozen;

  /*! \brief Flag to track the state of the Map canvas.
              *
              * The canvas is
              * flagged as dirty by any operation that changes the state of
              * the layers or the view extent. If the canvas is not dirty, paint
              * events are handled by bit-blitting the stored canvas bitmap to
              * the canvas. This improves performance by not reading the data source
              * when no real change has occurred
   */
                 bool dirty;


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
