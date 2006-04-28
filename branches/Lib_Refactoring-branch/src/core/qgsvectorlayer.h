/***************************************************************************
                          qgsvectorlayer.h  -  description
                             -------------------
    begin                : Oct 29, 2003
    copyright            : (C) 2003 by Gary E.Sherman
    email                : sherman at mrcc.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* $Id$ */

#ifndef QGSVECTORLAYER_H
#define QGSVECTORLAYER_H

#include <map>
#include <set>
#include <vector>

#include "qgis.h"
#include "qgsmaplayer.h"

class QPainter;
class QImage;

class QgsAttributeAction;
class QgsField;
class QgsFeature;
class QgsGeometry;
class QgsGeometryVertexIndex;
class QgsMapToPixel;
class QgsLabel;
class QgsRect;
class QgsRenderer;
class QgsVectorDataProvider;

typedef std::map<int, std::map<QString,QString> > changed_attr_map;
typedef std::set<int> feature_ids;

/*! \class QgsVectorLayer
 * \brief Vector layer backed by a data source provider
 */

class QgsVectorLayer : public QgsMapLayer
{
  Q_OBJECT;

public:

  /** Constructor */
  QgsVectorLayer(QString baseName = 0, QString path = 0, QString providerLib = 0);

  /** Destructor */
  virtual ~QgsVectorLayer();

  /** Returns the permanent storage type for this layer as a friendly name. */
  QString storageType() const;

  /** Capabilities for this layer in a friendly format. */
  QString capabilitiesString() const;

  /** Set the primary display field to be used in the identify results dialog */
  void setDisplayField(QString fldName=0);

  /** Returns the primary display field name used in the identify results dialog */
  const QString displayField() const;

  /** bind layer to a specific data provider
     @param provider should be "postgres", "ogr", or ??
     @todo XXX should this return bool?  Throw exceptions?
  */
  bool setDataProvider( QString const & provider );
  
  /** Returns the data provider */
  QgsVectorDataProvider* getDataProvider();

  /** Returns the data provider in a const-correct manner */
  const QgsVectorDataProvider* getDataProvider() const;

  /** Sets the textencoding of the data provider */
  void setProviderEncoding(const QString& encoding);

  /** Query data provider to find out the WKT projection string for this layer.
   *  This implements the virtual method of the same name defined in QgsMapLayer
   */
  QString getProjectionWKT();

  /**
   * Gets the SRID of the layer by querying the underlying data provider
   * @return The SRID if the provider is able to provide it, otherwise 0
   */
  int getProjectionSrid();
  
  /** Setup the coordinate system tranformation for the layer */
  void setCoordinateSystem();

  QgsLabel *label();

  QgsAttributeAction* actions() { return mActions; }

  
  
  /** Select features found within the search rectangle (in layer's coordinates) */
  void select(QgsRect * rect, bool lock);

  /** Select feature by its ID, optionally emit signal selectionChanged() */
  void select(int featureId, bool emitSignal = TRUE);
  
  /** Clear selection */
  void removeSelection(bool emitSignal = TRUE);
  
  /** Select not selected features and deselect selected ones */
  void invertSelection();

  /** Get a copy of the user-selected features */  
  std::vector<QgsFeature>* selectedFeatures();
  
  /** Return reference to identifiers of selected features */
  const feature_ids& selectedFeaturesIds() const;
  
  /** Change selection to the new set of features */
  void setSelectedFeatures(feature_ids& ids);

  /** Returns the bounding box of the selected features. If there is no selection, QgsRect(0,0,0,0) is returned */
  QgsRect boundingBoxOfSelected();

  
  /** Insert a copy of the given features into the layer */
  bool addFeatures(std::vector<QgsFeature*>* features, bool makeSelected = TRUE);

  /** Copies the symbology settings from another layer. Returns true in case of success */
  bool copySymbologySettings(const QgsMapLayer& other);

  /** Returns true if this layer can be in the same symbology group with another layer */
  bool isSymbologyCompatible(const QgsMapLayer& other) const;
  
  /** Read property of int featureType. */
  const int & featureType() const;
  
  /** Returns a pointer to the renderer */
  const QgsRenderer* renderer() const;

  /** Sets the renderer. If a renderer is already present, it is deleted */
  void setRenderer(QgsRenderer * r);

  /** Returns point, line or polygon */
  QGis::VectorType vectorType() const;

  /** Return the provider type for this layer */
  QString providerType() const;

  /** reads vector layer specific state from project file DOM node.
   *  @note Called by QgsMapLayer::readXML().
   */
  virtual bool readXML_( QDomNode & layer_node );

  /** write vector layer specific state to project file DOM node.
   *  @note Called by QgsMapLayer::writeXML().
   */
  virtual bool writeXML_( QDomNode & layer_node, QDomDocument & doc );


  /** Get the first feature resulting from a select operation
  * @param selected selected feeatures only
  * @return QgsFeature
  */
  virtual QgsFeature * getFirstFeature(bool fetchAttributes=false, bool selected=false) const;

  /** Get the next feature resulting from a select operation
  * @param selected selected feeatures only
  * @return QgsFeature
  */
  virtual QgsFeature * getNextFeature(bool fetchAttributes=false, bool selected=false) const;

  /** Get the next feature using new method
   * TODO - make this pure virtual once it works and change existing providers
   *        to use this method of fetching features
   */
  virtual bool getNextFeature(QgsFeature &feature, bool fetchAttributes=false) const;

  /**
   * Number of features in the layer. This is necessary if features are
   * added/deleted or the layer has been subsetted. If the data provider
   * chooses not to support this feature, the total number of features
   * can be returned.
   * @return long containing number of features
   */
  virtual long featureCount() const;

  /** Update the feature count 
   * @return long containing the number of features in the datasource
   */
  virtual long updateFeatureCount() const;

  /** Update the extents for the layer. This is necessary if features are
   *  added/deleted or the layer has been subsetted.
   */
  virtual void updateExtents();

  /**
   * Set the string (typically sql) used to define a subset of the layer
   * @param subset The subset string. This may be the where clause of a sql statement
   *               or other defintion string specific to the underlying dataprovider
   *               and data store.
   */
  virtual void setSubsetString(QString subset);

  /**
   * Get the string (typically sql) used to define a subset of the layer
   * @return The subset string or QString::null if not implemented by the provider
   */
  virtual QString subsetString();

  /**
   * Number of attribute fields for a feature in the layer
   */
  virtual int fieldCount() const;

  /**
    Return a list of field names for this layer
   @return vector of field names
  */
  virtual std::vector<QgsField> const & fields() const;

  /** Adds a feature
      @param lastFeatureInBatch  If True, will also go to the effort of e.g. updating the extents.
      @return                    Irue in case of success and False in case of error
   */
  bool addFeature(QgsFeature* f, bool alsoUpdateExtent = TRUE);
  
  
  /** Insert a new vertex before the given vertex number,
   *  in the given ring, item (first number is index 0), and feature
   *  Not meaningful for Point geometries
   */
  bool insertVertexBefore(double x, double y, int atFeatureId,
                          QgsGeometryVertexIndex beforeVertex);

  /** Moves the vertex at the given position number,
   *  ring and item (first number is index 0), and feature
   *  to the given coordinates
   */
  bool moveVertexAt(double x, double y, int atFeatureId,
                    QgsGeometryVertexIndex atVertex);

  /** Deletes the vertex at the given position number,
   *  ring and item (first number is index 0), and feature
   */
  bool deleteVertexAt(int atFeatureId,
                      QgsGeometryVertexIndex atVertex);

  /** Deletes the selected features
   *  @return true in case of success and false otherwise
   */
  bool deleteSelectedFeatures();

  /** Returns the default value for the attribute @c attr for the feature @c f. */
  QString getDefaultValue(const QString& attr, QgsFeature* f);

  /** Set labels on */
  void setLabelOn( bool on );

  /** Label is on */
  bool labelOn( void );

  /** Returns true if the provider is in editing mode */
  virtual bool isEditable() const;

  /** Returns true if the provider has been modified since the last commit */
  virtual bool isModified() const;

  /**Snaps a point to the closest vertex if there is one within the snapping tolerance
     @param point       The point which is set to the position of a vertex if there is one within the snapping tolerance.
     If there is no point within this tolerance, point is left unchanged.
     @param tolerance   The snapping tolerance
     @return true if the position of point has been changed, and false otherwise */
  bool snapPoint(QgsPoint& point, double tolerance);

  /**Snaps a point to the closest vertex if there is one within the snapping tolerance
     @param atVertex          Set to a vertex index of the snapped-to vertex
     @param snappedFeatureId  Set to the feature ID that where the snapped-to vertex belongs to.
     @param snappedGeometry   Set to the geometry that the snapped-to vertex belongs to.
     @param tolerance         The snapping tolerance
     @return true if the position of the points have been changed, and false otherwise (or not implemented by the provider) 
     
     TODO: Handle returning multiple verticies if they are coincident
   */
  bool snapVertexWithContext(QgsPoint& point,
                             QgsGeometryVertexIndex& atVertex,
                             int& snappedFeatureId,
                             QgsGeometry& snappedGeometry,
                             double tolerance);

  /**Snaps a point to the closest line segment if there is one within the snapping tolerance (mSnappingTolerance)
     @param beforeVertex      Set to a value where the snapped-to segment is before this vertex index
     @param snappedFeatureId  Set to the feature ID that where the snapped-to segment belongs to.
     @param snappedGeometry   Set to the geometry that the snapped-to segment belongs to.
     @param tolerance         The snapping tolerance
     @return true if the position of the points have been changed, and false otherwise (or not implemented by the provider) 
     
     TODO: Handle returning multiple lineFeatures if they are coincident
   */
  bool snapSegmentWithContext(QgsPoint& point,
                              QgsGeometryVertexIndex& beforeVertex,
                              int& snappedFeatureId, 
                              QgsGeometry& snappedGeometry,
                              double tolerance);

  /**Commits edited attributes. Depending on the feature id,
     the changes are written to not commited features or redirected to
     the data provider*/
  bool commitAttributeChanges(const std::set<QString>& deleted,
            const std::map<QString,QString>& added,
            std::map<int,std::map<QString,QString> >& changed);

  /** \brief Draws the layer using coordinate transformation
   *  \param widthScale line width scale
   *  \param symbolScale symbol scale
   */
  void draw(QPainter * p, QgsRect * viewExtent, QgsMapToPixel * cXf, QgsCoordinateTransform* ct, double widthScale, double symbolScale);

  /** \brief Draws the layer labels using coordinate transformation
   *  \param scale size scale, applied to all values in pixels
   */
  void drawLabels(QPainter * p, QgsRect * viewExtent, QgsMapToPixel * cXf, QgsCoordinateTransform* ct, double scale);

  /** returns array of added features */
  std::vector<QgsFeature*>& addedFeatures();

  /** returns array of deleted feature IDs */
  feature_ids& deletedFeatureIds();
 
  /** returns array of features with changed attributes */
  changed_attr_map& changedAttributes();

  /** Sets whether some features are modified or not */
  void setModified(bool modified = TRUE, bool onlyGeometryWasModified = FALSE);
  
  /** Save as shapefile */
  QString saveAsShapefile(QString path, QString encoding);

  /** Make layer editable */
  bool startEditing();
  
  /** Stop editing and write the changes to the provider */
  bool commitChanges();

  /** Stop editing and discard the edits */
  bool rollBack();

public slots:

  void triggerRepaint();

signals:

  /** This signal is emited when selection was changed */
  void selectionChanged();
  
  /** This signal is emitted when modifications has been done on layer */
  void wasModified(bool onlyGeometry);

  /** This signal is emitted when drawing features to tell current progress */
  void drawingProgress(int current, int total);

private:                       // Private methods

  /** vector layers are not copyable */
  QgsVectorLayer( QgsVectorLayer const & rhs );

  /** vector layers are not copyable */
  QgsVectorLayer & operator=( QgsVectorLayer const & rhs );

  /** Draws features. May cause projections exceptions to be generated
   *  (i.e., code that calls this function needs to catch them
   */
  void drawFeature(QPainter* p, QgsFeature* fet, QgsMapToPixel * cXf, QgsCoordinateTransform* ct,
                   QImage* marker, double markerScaleFactor);

  /** Draws the layer labels using coordinate transformation */
  void drawLabels(QPainter * p, QgsRect * viewExtent, QgsMapToPixel * cXf, QgsCoordinateTransform* ct);

  /** Convenience function to transform the given point */
  void transformPoint(double& x, double& y,
                      QgsMapToPixel* mtp, QgsCoordinateTransform* ct);
  
  void transformPoints(std::vector<double>& x, std::vector<double>& y, std::vector<double>& z,
                       QgsMapToPixel* mtp, QgsCoordinateTransform* ct);

  /** Draw the linestring as given in the WKB format. Returns a pointer
   * to the byte after the end of the line string binary data stream (WKB).
   */
  unsigned char* drawLineString(unsigned char* WKBlinestring, QPainter* p,
                                QgsMapToPixel* mtp, QgsCoordinateTransform* ct);

  /** Draw the polygon as given in the WKB format. Returns a pointer to
   *  the byte after the end of the polygon binary data stream (WKB).
   */
  unsigned char* drawPolygon(unsigned char* WKBpolygon, QPainter* p, 
                             QgsMapToPixel* mtp, QgsCoordinateTransform* ct);

  /** Draws the layer using coordinate transformation
   *  @return FALSE if an error occurred during drawing
   */
  bool draw(QPainter * p, QgsRect * viewExtent, QgsMapToPixel * cXf, QgsCoordinateTransform* ct);

  /** Goes through all features and finds a free id (e.g. to give it temporarily to a not-commited feature) */
  int findFreeId();


private:                       // Private attributes

  /** Update threshold for drawing features as they are read. A value of zero indicates
   *  that no features will be drawn until all have been read
   */
  int mUpdateThreshold;

  /** Pointer to data provider derived from the abastract base class QgsDataProvider */
  QgsVectorDataProvider *mDataProvider;

  /** index of the primary label field */
  QString mDisplayField;

  /** Data provider key */
  QString mProviderKey;

  /** The user-defined actions that are accessed from the Identify Results dialog box */
  QgsAttributeAction* mActions;

  /** Flag indicating whether the layer is in editing mode or not */
  bool mEditable;
  
  /** Flag indicating whether the layer has been modified since the last commit */
  bool mModified;
  
  /** cache of the committed geometries retrieved for the current display */
  std::map<int, QgsGeometry*> mCachedGeometries;
  
  /** Set holding the feature IDs that are activated */
  feature_ids mSelected;
  
  /** Deleted feature IDs which are not commited */
  feature_ids mDeleted;
  
  /** New features which are not commited */
  std::vector<QgsFeature*> mAddedFeatures;
  
  /** Changed attributes which are not commited */
  changed_attr_map mChangedAttributes;
  
  /** Changed geometries which are not commited. */
  std::map<int, QgsGeometry> mChangedGeometries;
  
  /** Geometry type as defined in enum WKBTYPE (qgis.h) */
  int mGeometryType;
  
  /** Renderer object which holds the information about how to display the features */
  QgsRenderer *mRenderer;

  /** Label */
  QgsLabel *mLabel;

  /** Display labels */
  bool mLabelOn;

};

#endif
