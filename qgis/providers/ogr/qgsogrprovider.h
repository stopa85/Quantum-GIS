/***************************************************************************
            qgsogrprovider.h Data provider for ESRI shapefile format
                    Formerly known as qgsshapefileprovider.h  
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

#include "../../src/qgsvectordataprovider.h"

#include <memory>

#include <geos.h>

#include <ogr_spatialref.h>
#include <ogrsf_frmts.h>

class QgsFeature;
class QgsField;
class OGRDataSource;
class OGRLayer;
class OGRFeature;
class OGREnvelope;
class OGRPolygon;

/**
  \class QgsOgrProvider
  \brief Data provider for ESRI shapefiles
  */
class QgsOgrProvider:public QgsVectorDataProvider
{
  public:

    QgsOgrProvider(QString const & uri = "");

    /// XXX why is this virtual if there will be no sub-classes?
    virtual ~ QgsOgrProvider();

    /**
      *   Returns the permanent storage type for this layer as a friendly name.
      */
    QString storageType() const;

    /** Used to ask the layer for its projection as a WKT string. Implements
     * virtual method of same name in QgsDataProvider. */
    QString getProjectionWKT(size_t dataSourceLayerNum = 0);

    /**
     * Get the first feature resulting from a select operation
     * @return QgsFeature
     */
    QgsFeature *getFirstFeature(bool fetchAttributes = false, size_t dataSourceLayerNum = 0);

    /** 
     * Get the next feature resutling from a select operation
     * @return QgsFeature
     */
    QgsFeature *getNextFeature(bool fetchAttributes = false, size_t dataSourceLayerNum = 0);

    /**Get the next feature resulting from a select operation.
    *@param attlist a list containing the indexes of the attribute fields to copy
    *@param getnotcommited flag indicating if not commited features should be returned
    */
    QgsFeature *getNextFeature(std::list<int> const& attlist, int featureQueueSize = 1, size_t dataSourceLayerNum = 0);

    /** 
     * Get the next feature resutling from a select operation
     * @return True if the feature was read. This does not indicate
     * that the feature is valid. Use QgsFeature::isValid() to check
     * the validity of a feature before using it.
     */
    bool getNextFeature(QgsFeature &feature, bool fetchAttributes = false, size_t dataSourceLayerNum = 0);

    /** Get the feature type. This corresponds to 
      WKBPoint,
      WKBLineString,
      WKBPolygon,
      WKBMultiPoint,
      WKBMultiLineString or
      WKBMultiPolygon
     * as defined in qgis.h
     */
    int geometryType(size_t dataSourceLayerNum = 0) const;

    /** return the number of layers for the current data source

    @note 

    Should this be subLayerCount() instead?
    */
    size_t layerCount() const;

    /** 
     * Get the number of features in the layer
     */
    long featureCount(size_t dataSourceLayerNum = 0) const;
    /** 
     * Get the number of fields in the layer
     */
    int fieldCount(size_t dataSourceLayerNum = 0) const;
    /**
     * Select features based on a bounding rectangle. Features can be retrieved 
     * with calls to getFirstFeature and getNextFeature.
     * @param mbr QgsRect containing the extent to use in selecting features
     * @param useIntersect Use geos functions to determine the selected set
     */
    void select(QgsRect * mbr, bool useIntersect, size_t dataSourceLayerNum = 0);

    /**
     * Identify features within the search radius specified by rect

     XXX Check the source, this does NOT return a vector of features.

     * @param rect Bounding rectangle of search radius
     * @return std::vector containing QgsFeature objects that intersect rect
     */
    //virtual std::vector < QgsFeature > &identify(QgsRect * rect, size_t dataSourceLayerNum = 0);
    void identify(QgsRect * rect, size_t dataSourceLayerNum = 0);

    /** Return the extent for this data layer
    */
    virtual QgsRect *extent( size_t dataSourceLayerNum = 0 );

    /**Get an attribute associated with a feature*/
    void getFeatureAttribute(OGRFeature * ogrFet, QgsFeature * f, int attindex, size_t dataSourceLayerNum = 0);
    /**
     * Get the attributes associated with a feature
     */
    void getFeatureAttributes(OGRFeature * ogrFet, QgsFeature * f, size_t dataSourceLayerNum = 0);
    /**
     * Get the field information for the layer
     */
    std::vector < QgsField > const & fields(size_t dataSourceLayerNum = 0) const;

    /* Reset the layer - for an OGRLayer, this means clearing the
     * spatial filter and calling ResetReading
     */
    /* virtual */ void reset(size_t dataSourceLayerNum = 0);

    /**Returns the minimum value of an attribut
      @param position the number of the attribute*/
    QString minValue(int position, size_t dataSourceLayerNum = 0);

    /**Returns the maximum value of an attribut
      @param position the number of the attribute*/
    QString maxValue(int position, size_t dataSourceLayerNum = 0);

    /**Returns true if this is a valid shapefile
    */
    bool isValid() const;

    /**Writes a list of features to the file*/
    bool addFeatures(std::list<QgsFeature*> const flist, size_t dataSourceLayerNum = 0);

    /**Adds new attributess. Unfortunately not supported for layers with features in it*/
    bool addAttributes(std::map<QString,QString> const & name, size_t dataSourceLayerNum = 0);

    /**Changes attribute values of existing features */
    bool changeAttributeValues(std::map<int,std::map<QString,QString> > const & attr_map, size_t dataSourceLayerNum = 0);

    QgsDataSourceURI * getURI()
    { 
        return 0;
    }

    /**Tries to create a .qix index file for faster access if only a subset of the features is required
     @return true in case of success*/
    bool createSpatialIndex(size_t dataSourceLayerNum = 0);

    /** Returns a bitmask containing the supported capabilities
        Note, some capabilities may change depending on whether
        a spatial filter is active on this provider, so it may
        be prudent to check this value per intended operation.
        See the OGRLayer::TestCapability API for details.
      */
    int capabilities(size_t dataSourceLayerNum = 0) const;

    void setEncoding(const QString& e);


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
    QString name() const;


    /** return description

      Return a terse string describing what the provider is.

      @note

      Instead of being pure virtual, might be better to generalize this
      behavior and presume that none of the sub-classes are going to do
      anything strange with regards to their name or description?

     */
    QString description() const;

    /** return vector file filter string

      Returns a string suitable for a QFileDialog of vector file formats
      supported by the data provider.  Naturally this will be an empty string
      for those data providers that do not deal with plain files, such as
      databases and servers.

      @note

      It'd be nice to eventually be raster/vector neutral.
    */
    /* virtual */ QString fileVectorFilters() const;



    /** creates and returns a list of map layers

    This corresponds to layers found within the data source.

    @note

    Not sure if this is a const member or not.

    */
    /* virtual */ list<QgsMapLayer*> createLayers();


  protected:
    /** loads fields from input file to member attributeFields */
    void loadFields();


  private:

    /**Adds one feature*/
    bool addFeature(QgsFeature* f, size_t dataSourceLayerNum = 0);

    /**Fills the cash and sets minmaxcachedirty to false*/
    void fillMinMaxCache(size_t dataSourceLayerNum = 0);

    // XXX isn't this already ultimately defined in QgsDataProvider?
    // XXX (commented out to see if it is so)
//     enum ENDIAN
//     {
//       NDR = 1,
//       XDR = 0
//     };

    /** internal implementation object containing hidden provider state.
    */
    struct Imp;

    std::auto_ptr<Imp> imp_;

};
