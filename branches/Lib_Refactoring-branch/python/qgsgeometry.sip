
/** 
 * Represents a geometry with input and output in formats specified by 
 * (at least) the Open Geospatial Consortium (WKB / WKT), and containing
 * various functions for geoprocessing of the geometry.
 *
 * The geometry is represented internally by the OGC WKB format, though
 * perhaps this will be migrated to GEOS geometry in future.
 *
 * @author Brendan Morley
 */

class QgsGeometry
{
%TypeHeaderCode
#include <qgsgeometry.h>
%End

  public:

    //! Constructor
    QgsGeometry();
    
    /** copy constructor will prompt a deep copy of the object */
    QgsGeometry( const QgsGeometry & );
    

    //! Destructor
    ~QgsGeometry();

    typedef unsigned int size_t;

    
    /** 
       Set the geometry, feeding in the buffer containing OGC Well-Known Binary and the buffer's length.
       This class will take ownership of the buffer.
    */
    void setWkbAndOwnership(unsigned char * wkb, size_t length);
    
    /** 
       Returns the buffer containing this geometry in WKB format.
       You may wish to use in conjunction with wkbSize().
    */
    unsigned char * wkbBuffer() const;
    
    /** 
       Returns the size of the WKB in wkbBuffer().
    */
    size_t wkbSize() const;
    
    /** 
       Returns the QString containing this geometry in WKT format.
    */
    const QString & wkt() const; 

    /**
       Set the geometry, feeding in a geometry in GEOS format.
    */
    // TODO: unsupported class... would be possible to use PyGEOS?
    //void setGeos(geos::Geometry* geos);

    /**
       Returns the vertex closest to the given point
    */
    QgsPoint closestVertex(const QgsPoint& point) const;

    /** Insert a new vertex before the given vertex index,
     *  ring and item (first number is index 0)
     *  If the requested vertex number (beforeVertex.back()) is greater
     *  than the last actual vertex on the requested ring and item,
     *  it is assumed that the vertex is to be appended instead of inserted.
     *  Returns FALSE if atVertex does not correspond to a valid vertex
     *  on this geometry (including if this geometry is a Point).
     *  It is up to the caller to distinguish between
     *  these error conditions.  (Or maybe we add another method to this
     *  object to help make the distinction?)
     */
    bool insertVertexBefore(double x, double y, QgsGeometryVertexIndex beforeVertex);

    /** Moves the vertex at the given position number,
     *  ring and item (first number is index 0)
     *  to the given coordinates.
     *  Returns FALSE if atVertex does not correspond to a valid vertex
     *  on this geometry (including if this geometry is a Point).
     *  It is up to the caller to distinguish between
     *  these error conditions.  (Or maybe we add another method to this
     *  object to help make the distinction?)
     */
    bool moveVertexAt(double x, double y, QgsGeometryVertexIndex atVertex);

    /** Deletes the vertex at the given position number,
     *  ring and item (first number is index 0)
     *  Returns FALSE if atVertex does not correspond to a valid vertex
     *  on this geometry (including if this geometry is a Point),
     *  or if the number of remaining verticies in the linestring
     *  would be less than two.
     *  It is up to the caller to distinguish between
     *  these error conditions.  (Or maybe we add another method to this
     *  object to help make the distinction?)
     */
    bool deleteVertexAt(QgsGeometryVertexIndex atVertex);

    /**
     *  Modifies x and y to indicate the location of
     *  the vertex at the given position number,
     *  ring and item (first number is index 0)
     *  to the given coordinates
     *
     *  Returns TRUE if atVertex is a valid position on
     *  the geometry, otherwise FALSE.
     *
     *  If FALSE, x and y are not modified.
     */
    bool vertexAt(double &x, double &y, QgsGeometryVertexIndex atVertex) const;

    /**
        Returns, in atVertex, the closest vertex in this geometry to the given point.
        The squared cartesian distance is also returned in sqrDist.
     */
    QgsPoint closestVertexWithContext(QgsPoint& point,
                                      QgsGeometryVertexIndex& atVertex,
                                      double& sqrDist);

    /**
        Returns, in beforeVertex, the closest segment in this geometry to the given point.
        The squared cartesian distance is also returned in sqrDist.
     */
    QgsPoint closestSegmentWithContext(QgsPoint& point,
                                       QgsGeometryVertexIndex& beforeVertex,
                                       double& sqrDist);

    /**Returns the bounding box of this feature*/
    QgsRect boundingBox() const;

    /**Test for intersection with a rectangle (uses GEOS)*/
    bool intersects(QgsRect* r) const;

    /**Creates a geos geometry from this features geometry. Note, that the returned object needs to be deleted*/
    // TODO: unsupported class... would be possible to use PyGEOS?
    //geos::Geometry* geosGeometry() const;


}; // class QgsGeometry

