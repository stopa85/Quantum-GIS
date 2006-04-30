
//typedef std::map<int, std::map<QString,QString> > changed_attr_map;
//typedef std::set<int> feature_ids;

/*! \class QgsVectorLayer
 * \brief Vector layer backed by a data source provider
 */

class QgsVectorLayer : QgsMapLayer
{
%TypeHeaderCode
#include <qgsvectorlayer.h>
%End
  
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
  bool setDataProvider( const QString & provider );
  
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

  // TODO: QgsLabel and QgsAttributeAction not yet wrapped
  //QgsLabel *label();
  //QgsAttributeAction* actions() { return mActions; }

  
  /** Select features found within the search rectangle (in layer's coordinates) */
  void select(QgsRect * rect, bool lock);

  /** Select feature by its ID, optionally emit signal selectionChanged() */
  void select(int featureId, bool emitSignal = TRUE);
  
  /** Clear selection */
  void removeSelection(bool emitSignal = TRUE);
  
  /** Select not selected features and deselect selected ones */
  void invertSelection();

  /** Get a copy of the user-selected features */  
  // TODO: wrap
  //std::vector<QgsFeature>* selectedFeatures();
  
  /** Return reference to identifiers of selected features */
  // TODO: wrap
  //const feature_ids& selectedFeaturesIds() const;
  
  /** Change selection to the new set of features */
  // TODO: wrap
  //void setSelectedFeatures(feature_ids& ids);

  /** Returns the bounding box of the selected features. If there is no selection, QgsRect(0,0,0,0) is returned */
  QgsRect boundingBoxOfSelected();

  
  /** Insert a copy of the given features into the layer */
  // TODO: wrap
  //bool addFeatures(std::vector<QgsFeature*>* features, bool makeSelected = TRUE);

  /** Copies the symbology settings from another layer. Returns true in case of success */
  bool copySymbologySettings(const QgsMapLayer& other);

  /** Returns true if this layer can be in the same symbology group with another layer */
  bool isSymbologyCompatible(const QgsMapLayer& other) const;
  
  /** Read property of int featureType. */
  // TODO: wrap
  //const int & featureType() const;
  
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
  // TODO: wrap
  //virtual const std::vector<QgsField> & fields() const;

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
  bool labelOn( );

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
  // TODO: wrap
  //bool commitAttributeChanges(const std::set<QString>& deleted,
  //          const std::map<QString,QString>& added,
  //          std::map<int,std::map<QString,QString> >& changed);

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
  // TODO: wrap
  //std::vector<QgsFeature*>& addedFeatures();

  /** returns array of deleted feature IDs */
  // TODO: wrap
  //feature_ids& deletedFeatureIds();
 
  /** returns array of features with changed attributes */
  // TODO: wrap
  //changed_attr_map& changedAttributes();

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
  QgsVectorLayer( const QgsVectorLayer& rhs );

};

