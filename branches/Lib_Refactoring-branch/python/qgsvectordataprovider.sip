
/** Base class for vector data providers
 */
 
class QgsVectorDataProvider : QgsDataProvider /Abstract/
{
%TypeHeaderCode
#include <qgsvectordataprovider.h>
%End

    // XXX no  signals or slots, so not needed Q_OBJECT

    public:

      // If you add to this, please also add to capabilitiesString()
      enum Capability
      {
        NoCapabilities =           0,
        AddFeatures =              1,
        DeleteFeatures =           2,
        ChangeAttributeValues =    4,
        AddAttributes =            8,    // TODO: what is this exactly?
        DeleteAttributes =        16,
        CreateSpatialIndex =      64,
        SelectAtId =             128,
        ChangeGeometries =       256
      };

      QgsVectorDataProvider();

      QgsVectorDataProvider( const QString & uri );

      virtual ~QgsVectorDataProvider();

      /**
       *   Returns the permanent storage type for this layer as a friendly name.
       */
      virtual QString storageType();

      /**
       * Select features based on a bounding rectangle. Features can be retrieved 
       * with calls to getFirstFeature and getNextFeature. Request for features 
       * for use in drawing the map canvas should set useIntersect to false.
       * @param mbr QgsRect containing the extent to use in selecting features
       * @param useIntersect If true, use the intersects function to select features
       * rather than the PostGIS && operator that selects based on bounding box
       * overlap.
       *
       */
      virtual void select(QgsRect *mbr, bool useIntersect=false)=0;
      /**
       * Update the feature count based on current spatial filter. If not
       * overridden in the data provider this function returns -1
       */
      virtual long updateFeatureCount();
      
      /** 
       * Gets the feature at the given feature ID.
       * @return  QgsFeature
       */
      virtual QgsFeature * getFeatureAtId(int featureId);
      
      /** 
       * Get the first feature resulting from a select operation
       * @return QgsFeature
       */
      virtual QgsFeature * getFirstFeature(bool fetchAttributes = false) = 0;

      /** 
       * Get the next feature resutling from a select operation
       * @return QgsFeature
       */
      virtual QgsFeature * getNextFeature(bool fetchAttributes = false) = 0;

      /**Get the next feature resulting from a select operation.
       *@param attlist a list containing the indexes of the attribute fields to copy
     * @param featureQueueSize   a hint to the provider as to how many features are likely to be retrieved in a batch
       */
      // TODO: custom handling needed
      //virtual QgsFeature* getNextFeature(const std::list<int> & attlist, int featureQueueSize = 1);

      /**
       * Get the next feature using new method
       * TODO - make this pure virtual once it works and change existing providers
       *        to use this method of fetching features
       */

      virtual bool getNextFeature(QgsFeature &feature, bool fetchAttributes = false) = 0;

      /** Get feature type.
       * Gets the feature type as defined in WKBTYPE (qgis.h).
       * @return int representing the feature type
       */
      virtual int geometryType() const = 0;


      /**
       * Number of features in the layer
       * @return long containing number of features
       */
      virtual long featureCount() const = 0;

      /**
     * Get the attributes associated with a feature
     * TODO: Get rid of "row" and set up provider-internal caching instead
     */
    virtual void getFeatureAttributes(int oid, int& row, QgsFeature *f);
    
    /**
       * Number of attribute fields for a feature in the layer
       */
      virtual int fieldCount() const = 0;

      /**
       * Return a list of field names for this layer
       * @return vector of field names
       */
      // TODO: needed %MethodCode
      //virtual const std::vector<QgsField> & fields() const = 0;

      /** 
       * Reset the layer to clear any spatial filtering or other contstraints that
       * would prevent the entire record set from being traversed by call to 
       * getNextFeature(). Some data stores may not require any special action to
       * reset the layer. In this case, the provider should simply implement an empty
       * function body.
       */
      virtual void reset() = 0;

      /**Returns the minimum value of an attributs
        @param position the number of the attribute*/
      virtual QString minValue(int position) = 0;

      /**Returns the maximum value of an attributs
        @param position the number of the attribute*/
      virtual QString maxValue(int position) = 0;

      /**Adds a list of features
        @return true in case of success and false in case of failure*/
      // TODO: wrap
      //virtual bool addFeatures(const std::list<QgsFeature*> flist);

      /**Deletes a feature
        @param id list containing feature ids to delete
        @return true in case of success and false in case of failure*/
      // TODO: wrap
      //virtual bool deleteFeatures(const std::list<int> & id);

      /**Adds new attributes
        @param name map with attribute name as key and type as value
        @return true in case of success and false in case of failure*/
      // TODO: wrap
      //virtual bool addAttributes(const std::map<QString,QString> & name);

      /**Deletes existing attributes
        @param names of the attributes to delete
        @return true in case of success and false in case of failure*/
      // TODO: wrap
      //virtual bool deleteAttributes(const std::set<QString> & name);

      /**Changes attribute values of existing features
        @param attr_map a map containing the new attributes. The integer is the feature id,
        the first QString is the attribute name and the second one is the new attribute value
        @return true in case of success and false in case of failure*/
      // TODO: wrap
      //virtual bool changeAttributeValues(const std::map<int,std::map<QString,QString> > & attr_map);

      /**Returns the default value for attribute @c attr for feature @c f. */
      virtual QString getDefaultValue(const QString & attr, QgsFeature* f);

      /**
       Changes geometries of existing features
       @param geometry_map   A std::map containing the feature IDs to change the geometries of. 
                             the second map parameter being the new geometries themselves
       @return               true in case of success and false in case of failure
     */
      // TODO: wrap
      //virtual bool changeGeometryValues(std::map<int, QgsGeometry> & geometry_map);

    /**
       * Identify features within the search radius specified by rect
       * @param rect Bounding rectangle of search radius
       * @return std::vector containing QgsFeature objects that intersect rect
       */
      // TODO: wrap
      //virtual std::vector<QgsFeature>& identify(QgsRect *rect) = 0;

      /**Creates a spatial index on the datasource (if supported by the provider type). Returns true in case of success*/
      virtual bool createSpatialIndex();

      /** Sets filter based on attribute values. Returns false when input string contains errors */
      //virtual bool setAttributeFilter(const QgsSearchString& attributeFilter);

      /** Returns current attribute filter */
      //virtual QgsSearchString getAttributeFilter();
      
      /** Returns a bitmask containing the supported capabilities
          Note, some capabilities may change depending on whether
          a spatial filter is active on this provider, so it may
          be prudent to check this value per intended operation.
       */
      virtual int capabilities() const;

      /**
       *  Returns the above in friendly format.
       */
      QString capabilitiesString() const;

      // TODO: wrap
      //const std::list<QString>& nonNumericalTypes();
      //const std::list<QString>& numericalTypes();

      virtual void setEncoding(const QString& e);
      QString encoding() const;

      /*! Indicates if the provider does its own coordinate transforms
       * @return true if the provider transforms its coordinates, otherwise false
       */
      virtual bool supportsNativeTransform();
      /*! Used to determine if the provider supports transformation using the
       * SRID of the target SRS.
       *
       * @note XXXXX WARNING THIS METHOD WILL BE DEPRECATED
       *       XXXXX in favour of SpatialRefSys accessors
       *       XXXXX and mutators!
       *
       * @return true if SRID is used, otherwise false
       */
      virtual bool usesSrid();
      /*! Used to determine if the provider supports transformation using the
       * WKT of the target SRS.
       *
       * @note XXXXX WARNING THIS METHOD WILL BE DEPRECATED
       *       XXXXX in favour of SpatialRefSys accessors
       *       XXXXX and mutators!
       *
       * @return true if WKT is used, otherwise false
       */
      virtual bool usesWKT();
      /*! Set the SRID of the target SRS.
       * This is only implemented if the provider supports native
       * transformation of its coordinates
       *
       * @note XXXXX WARNING THIS METHOD WILL BE DEPRECATED
       *       XXXXX in favour of SpatialRefSys accessors
       *       XXXXX and mutators!
       *
       * @param srid Spatial reference id of the target (map canvas)
       */
      virtual void setSrid(int srid);
      /*! Get the SRID of the target SRS
       * If the provider isn't capable of reporting the SRID of
       * the projection, ti will return 0
       *
       * @note XXXXX WARNING THIS METHOD WILL BE DEPRECATED
       *       XXXXX in favour of SpatialRefSys accessors
       *       XXXXX and mutators!
       *
       */
      virtual int getSrid();
      /*! Set the WKT of the target SRS.
       * This is only implemented if the provider supports native
       * transformation of its coordinates
       *
       * @note XXXXX WARNING THIS METHOD WILL BE DEPRECATED
       *       XXXXX in favour of SpatialRefSys accessors
       *       XXXXX and mutators!
       *
       * @param wkt Well known text of the target (map canvas) SRS
       */
      virtual void setWKT(QString wkt);

};

