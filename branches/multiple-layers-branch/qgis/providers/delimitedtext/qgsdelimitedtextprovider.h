/***************************************************************************
      qgsdelimitedtextprovider.h  -  Data provider for delimted text
                             -------------------
    begin                : 2004-02-27
    copyright            : (C) 2004 by Gary E.Sherman
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


#include <map>


class QgsFeature;
class QgsField;
class QFile;


/**
\class QgsDelimitedTextProvider
\brief Data provider for delimited text files.
*
* The provider needs to know both the path to the text file and
* the delimiter to use. Since the means to add a layer is farily
* rigid, we must provide this information encoded in a form that
* the provider can decipher and use.
* The uri must contain the path and delimiter in this format:
* /full/path/too/delimited.txt?delimiter=<delimiter>
*
* Example uri = "/home/foo/delim.txt?delimiter=|"
*/
class QgsDelimitedTextProvider : public QgsVectorDataProvider
{
public:

  QgsDelimitedTextProvider(QString const & uri = "");

  virtual ~ QgsDelimitedTextProvider();

  /**
    *   Returns the permanent storage type for this layer as a friendly name.
    */
  QString storageType();

  /** get the first feature

    mFile should be open with the file pointer at the record of the next
    feature, or EOF.  The feature found on the current line is parsed.

    @param feature the feature object to be populated with next feature
    @param fetchAttributes is true if you want to get the feature attributes, too

    The search will be limited to the search region if one is active.

    @return found QgsFeature, or null if not found
  */
  QgsFeature *getFirstFeature(bool fetchAttributes = false, size_t dataSourceLayerNum = 0);

  /** Get the next feature, if any

    mFile should be open with the file pointer at the record of the next
    feature, or EOF.  The feature found on the current line is parsed.

    @param feature the feature object to be populated with next feature
    @param fetchAttributes is true if you want to get the feature attributes, too

    The search will be limited to the search region if one is active.

  * @return found QgsFeature, or null if not found
  */
  QgsFeature *getNextFeature(bool fetchAttributes = false, size_t dataSourceLayerNum = 0);

  /** get the next feature, if any

    mFile should be open with the file pointer at the record of the next
    feature, or EOF.  The feature found on the current line is parsed.

    @param feature the feature object to be populated with next feature
    @param fetchAttributes is true if you want to get the feature attributes, too

    @return false if unable to get the next feature

    @note that feature will also be set invalid if unable to locate the next
    feature

    The search will be limited to the search region if one is active.

  /** Used to ask the layer for its projection as a WKT string. Implements
  virtual method of same name in QgsDataProvider. */
  QString getProjectionWKT()  {return QString("Not implemented yet");} ;

  /**
  * Get the first feature resulting from a select operation
  * @return QgsFeature
  */
  bool getNextFeature(QgsFeature & feature, bool fetchAttributes = false, size_t dataSourceLayerNum = 0);

  /** get the next feature, if any

    mFile should be open with the file pointer at the record of the next
    feature, or EOF.  The feature found on the current line is parsed.

    @param feature the feature object to be populated with next feature @param
           attlist is list of attributes by their field positions to fetch
                   with object

    @return false if unable to get the next feature

    @note That feature will also be set invalid if unable to locate the next
    feature.

    The search will be limited to the search region if one is active.

  */
  QgsFeature *getNextFeature(std::list<int> const & desiredAttributes, int featureQueueSize = 1, size_t dataSourceLayerNum = 0);


  /** Get the feature type. This corresponds to 
      WKBPoint,
      WKBLineString,
      WKBPolygon,
      WKBMultiPoint,
      WKBMultiLineString or
      WKBMultiPolygon
  * as defined in qgis.h
  * This provider will always return WKBPoint
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
  */
  void select(QgsRect * mbr, bool useIntersect = false, size_t dataSourceLayerNum = 0);

  /**
  * Identify features within the search radius specified by rect
  * @param rect Bounding rectangle of search radius
  * @return std::vector containing QgsFeature objects that intersect rect
  */
  // virtual std::vector < QgsFeature > &identify(QgsRect * rect, size_t dataSourceLayerNum = 0);
  void identify(QgsRect * rect, size_t dataSourceLayerNum = 0);

  /** Return the extent for this data layer
  */
  virtual QgsRect *extent(size_t dataSourceLayerNum = 0);
  /**
  * Get the attributes associated with a feature
  */
  void getFeatureAttributes(int key, QgsFeature * f, size_t dataSourceLayerNum = 0);
 /**
 * Get the field information for the layer
 */
    std::vector<QgsField> const & fields(size_t dataSourceLayerNum = 0) const;

  /* Reset the layer (ie move the file pointer to the head
     of the file.
   */
  void reset(size_t dataSourceLayerNum = 0);

 /**Returns the minimum value of an attribute
    @param position the number of the attribute*/
  QString minValue(int position,size_t dataSourceLayerNum = 0);

 /**Returns the maximum value of an attribute
    @param position the number of the attribute*/
  QString maxValue(int position,size_t dataSourceLayerNum = 0);

 /**Returns true if this is a valid delimited file
 */
  bool isValid() const;

 /**
  * Check to see if the point is withn the selection
  * rectangle
  * @param x X value of point
  * @param y Y value of point
  * @return True if point is within the rectangle
  */
  bool boundsCheck(double x, double y);

  int capabilities() const;

  //! Save the layer as a shapefile
  bool saveAsShapefile();

  QgsDataSourceURI *getURI()
  {
    return 0;
  };
  /** The delimited text provider does not do its own transforms so we return
     * false for the following three functions to indicate that transforms
     * should be handled by the QgsCoordinateTransform object. See the
     * documentation on QgsVectorDataProvider for details on these functions.
     */
    bool supportsNativeTransform(){return false;}
    bool usesSrid(){return false;}
    bool usesWKT(){return false;}

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





    /** creates and returns a list of map layers

    This corresponds to layers found within the data source.

    @note

    Not sure if this is a const member or not.

    */
    /* virtual */ std::list<QgsMapLayer*> createLayers()
    {
        // TODO
        return std::list<QgsMapLayer*>();
    }



private:

  /** get the next feature, if any

    mFile should be open with the file pointer at the record of the next
    feature, or EOF.  The feature found on the current line is parsed.

    @param feature the feature object to be populated with next feature
    @param getAttributes is true if you want to get the feature attributes, too
    
    @param desiredAttributes attributes fields to be collected for the feature
                             as denoted by their position

    @return false if unable to get the next feature
  */
  bool getNextFeature_( QgsFeature & feature, 
                        bool getAttributes, 
                        std::list<int> const * desiredAttributes = 0 );

  void fillMinMaxCash(size_t dataSourceLayerNum = 0);

  int *getFieldLengths(size_t dataSourceLayerNum = 0);

  //! Fields
  std::vector < QgsField > attributeFields;

  //! Map to store field position by name
  std::map < QString, int >fieldPositions;

  QString mFileName;
  QString mDelimiter;
  QString mXField;
  QString mYField;

  //! Layer extent
  QgsRect *mExtent;

  //! Current selection rectangle

  QgsRect *mSelectionRectangle;

  //! Text file
  QFile *mFile;

  bool mValid;

  int mGeomType;

  long mNumberFeatures;

  //! Feature id
  long mFid;

  /**Flag indicating, if the minmaxcache should be renewed (true) or not (false)*/
  bool mMinMaxCacheDirty;

  /**Matrix storing the minimum and maximum values*/
  double **mMinMaxCache;

  /**Fills the cash and sets minmaxcachedirty to false*/
  void mFillMinMaxCash();

  struct wkbPoint
  {
    unsigned char byteOrder;
    Q_UINT32 wkbType;
    double x;
    double y;
  };
  wkbPoint mWKBpt;

};
