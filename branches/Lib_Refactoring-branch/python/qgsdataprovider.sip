
/** \class QgsDataProvider
 * \brief Abstract base class for spatial data provider implementations
 * @author Gary E.Sherman
 * 
 * This object needs to inherit from QObject to enable event 
 * processing in the Postgres/PostGIS provider (QgsPostgresProvider).
 * It is called *here* so that this vtable and the vtable for 
 * QgsPostgresProvider don't get misaligned - 
 * the QgsVectorLayer class factory (which refers
 * to generic QgsVectorDataProvider's) depends on it.
 */


class QgsDataProvider : QObject 
{
%TypeHeaderCode
//#include <stdlib.h>
#include <qgsdataprovider.h>
%End
  
    public: 

      QgsDataProvider( const QString & uri = "" );

      /**
       * We need this so the subclass destructors get called
       */
      virtual ~QgsDataProvider();

      /*! Set the QgsSpatialReferenceSystem for this layer..
       * @note Must be reimplemented by each provider. 
       * @param theSRS QgsSpatialRefSys to be assigned to this layer
       *        A complete copy of the passed in SRS will be made.
       */
      virtual void setSRS(QgsSpatialRefSys * theSRS);

      /*! Get the QgsSpatialRefSys for this layer
       * @note Must be reimplemented by each provider. 
       * If the provider isn't capable of returning
       * its projection an empty srs will be return, ti will return 0
       */
      virtual QgsSpatialRefSys * getSRS();

      typedef unsigned int size_t;


      /** Used to ask the layer for its projection as a WKT string. 
       *
       * Must be reimplemented by each provider. 
       *
       * @note XXXXX WARNING THIS METHOD WILL BE DEPRECATED
       *       XXXXX in favour of SpatialRefSys accessors
              XXXXX and mutators!
       *
       */
      virtual QString getProjectionWKT() = 0;

      /** 
       * Set the data source specification. This may be a path or database
       * connection string
       * @param data source specification
       */
      virtual void setDataSourceUri( const QString & uri);

      /** 
       * Get the data source specification. This may be a path or database
       * connection string
       * @return data source specification
       */
      virtual const QString & getDataSourceUri();

      /**
         XXX why have the string versions then this?
      */
      virtual QgsDataSourceURI * getURI() = 0;

      /**
       * Get the extent of the layer
       * @return QgsRect containing the extent of the layer
       */
      virtual QgsRect * extent() = 0;

      /** type for byte order

        XDR is for network byte order, or big-endian
        NDR is for little-endian systems

        @note that default values were taken from similarly named WKB types
        */
      enum endian_t
      {
        XDR = 0,                    // network byte order (big-endian)
        NDR = 1                     // little endian
      };

      /**
        Return the endian of this layer.

        XDR for network, or big-endian, byte order
        NDR for little-endian byte order

        @note 

        By default this returns the endian-ness of the current platform.
        Sub-classes are free to over-ride this to perhaps return endian-ness of
        data as stored persistently instead of local hardware architecture
        endian-ness.
        */
      virtual endian_t endian();

      /**
       * Returns true if this is a valid layer. It is up to individual providers
       * to determine what constitutes a valid layer
       */
      virtual bool isValid()=0;

      /* Reset the layer - for an OGRLayer, this means clearing the
       * spatial filter and calling ResetReading
       */
      virtual void reset();

      /**
       * Update the extents of the layer. Not implemented by default
       */
      virtual void updateExtents();

      /**
       * Set the subset string used to create a subset of features in
       * the layer. This may be a sql where clause or any other string
       * that can be used by the data provider to create a subset.
       * Must be implemented in the dataprovider.
       */
      virtual void setSubsetString(QString subset);

      /**
       * Returns the subset definition string (typically sql) currently in
       * use by the layer and used by the provider to limit the feature set.
       * Must be overridden in the dataprovider, otherwise returns a null
       * QString.
       */
      virtual QString subsetString();
      
      /**
       * Sub-layers handled by this provider, in order from bottom to top
       *
       * Sub-layers are used when the provider's source can combine layers
       * it knows about in some way before it hands them off to the provider.
       */
      virtual QStringList subLayers();
    
    
      /** return the number of layers for the current data source

          @note 

         Should this be subLayerCount() instead?
      */
      virtual size_t layerCount() const = 0;


      /**
       * Reorder the list of layer names to be rendered by this provider
       * (in order from bottom to top)
       * \note   layers must have been previously added.
       */
      virtual void setLayerOrder(QStringList layers);
    
      /**
       * Set the visibility of the given sublayer name
       */
      virtual void setSubLayerVisibility(QString name, bool vis);

      /** return a provider name

      Essentially just returns the provider key.  Should be used to build file
      dialogs so that providers can be shown with their supported types. Thus
      if more than one provider supports a given format, the user is able to
      select a specific provider to open that file.

      @note

      Instead of being pure virtual, might be better to generalize this
      behavior and presume that none of the sub-classes are going to do
      anything strange with regards to their name or description?

      */
      virtual QString name() const = 0;


    /** return description

      Return a terse string describing what the provider is.

      @note

      Instead of being pure virtual, might be better to generalize this
      behavior and presume that none of the sub-classes are going to do
      anything strange with regards to their name or description?

     */
    virtual QString description() const = 0;


    /** return vector file filter string

      Returns a string suitable for a QFileDialog of vector file formats
      supported by the data provider.  Naturally this will be an empty string
      for those data providers that do not deal with plain files, such as
      databases and servers.

      @note

      It'd be nice to eventually be raster/vector neutral.
    */
    virtual QString fileVectorFilters() const;

    /** return raster file filter string

      Returns a string suitable for a QFileDialog of raster file formats
      supported by the data provider.  Naturally this will be an empty string
      for those data providers that do not deal with plain files, such as
      databases and servers.

      @note

      It'd be nice to eventually be raster/vector neutral.
    */
    virtual QString fileRasterFilters() const;

signals:

      /** 
       *   This is emitted whenever the worker thread has fully calculated the
       *   PostGIS extents for this layer, and its event has been received by this
       *   provider.
       */  
      void fullExtentCalculated();

};


