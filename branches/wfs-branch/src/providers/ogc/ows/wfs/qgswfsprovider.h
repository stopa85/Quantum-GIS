/***************************************************************************
      qgswfsprovider.h  -  QGIS Data provider for 
                           OGC Web Feature Service layers
                             -------------------
    begin                : 17 Mar, 2006
    copyright            : (C) 2006 by Brendan Morley
    email                : morb at beagle dot com dot au
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

#ifndef QGSWFSPROVIDER_H
#define QGSWFSPROVIDER_H

#include <vector>

#include "qgsvectordataprovider.h"

#include <qgscoordinatetransform.h>

#include "qgsrect.h"

#include <qdom.h>


/*
 * The following structs reflect the WFS XML schema, \TODO
 * as illustrated in Appendix E of the Web Map Service standard, version 1.3, 2004-08-02.
 */

  /** OnlineResource Attribute structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsOnlineResourceAttribute
  {
    QString xlinkHref;
  };

  /** Get Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsGetProperty
  {
    QgsWmsOnlineResourceAttribute onlineResource;
  };

  /** Post Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsPostProperty
  {
    QgsWmsOnlineResourceAttribute onlineResource;
  };

  /** HTTP Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsHttpProperty
  {
    QgsWmsGetProperty    get;
    QgsWmsPostProperty   post;  // can be null
  };

  /** DCP Type Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsDcpTypeProperty
  {
    QgsWmsHttpProperty http;
  };

  /** Operation Type structure (for GetMap and GetFeatureInfo) */
  // TODO: Fill to WMS specifications
  struct QgsWmsOperationType
  {
    QStringList                          format;
    std::vector<QgsWmsDcpTypeProperty>   dcpType;
  };

  /** Request Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsRequestProperty
  {
    // QgsWmsGetCapabilitiesProperty   ...
    // -- don't include since if we can get the capabilities,
    //    we already know what's in this part.
    QgsWmsOperationType     getMap;
    QgsWmsOperationType     getFeatureInfo;
  };

  /** Exception Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsExceptionProperty
  {
    QStringList                 format;   // text formats supported.
  };

  /** Primary Contact Person Property structure */
  struct QgsWmsContactPersonPrimaryProperty
  {
    QString            contactPerson;
    QString            contactOrganization;
  };

  /** Contact Address Property structure */
  struct QgsWmsContactAddressProperty
  {
    QString            addressType;
    QString            address;
    QString            city;
    QString            stateOrProvince;
    QString            postCode;
    QString            country;
  };

  /** Contact Information Property structure */
  struct QgsWmsContactInformationProperty
  {
    QgsWmsContactPersonPrimaryProperty contactPersonPrimary;
    QString                            contactPosition;
    QgsWmsContactAddressProperty       contactAddress;
    QString                            contactVoiceTelephone;
    QString                            contactFacsimileTelephone;
    QString                            contactElectronicMailAddress;
  };

  /** Service Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsServiceProperty
  {
    // QString                            name;  // Should always be "WMS"
    QString                            title;
    QString                            abstract;
    QStringList                        keywordList;
    QgsWmsOnlineResourceAttribute      onlineResource;
    QgsWmsContactInformationProperty   contactInformation;
    QString                            fees;
    QString                            accessConstraints;
    uint                               layerLimit;
    uint                               maxWidth;
    uint                               maxHeight;
  };

  /** Bounding Box Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsBoundingBoxProperty
  {
    QString   crs;
    QgsRect   box;    // consumes minx, miny, maxx, maxy.
    double    resx;   // spatial resolution (in CRS units)
    double    resy;   // spatial resolution (in CRS units)
  };

  /** Dimension Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsDimensionProperty
  {
    QString   name;
    QString   units;
    QString   unitSymbol;
    QString   defaultValue;   // plain "default" is a reserved word
    bool      multipleValues;
    bool      nearestValue;
    bool      current;
  };

  /** Logo URL Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsLogoUrlProperty
  {
    QString                         format;
    QgsWmsOnlineResourceAttribute   onlineResource;

    int                             width;
    int                             height;
  };

  /** Attribution Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsAttributionProperty
  {
    QString                         title;
    QgsWmsOnlineResourceAttribute   onlineResource;
    QgsWmsLogoUrlProperty           logoUrl;
  };

  /** Legend URL Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsLegendUrlProperty
  {
    QString                         format;
    QgsWmsOnlineResourceAttribute   onlineResource;

    int                             width;
    int                             height;
  };

  /** StyleSheet URL Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsStyleSheetUrlProperty
  {
    QString                         format;
    QgsWmsOnlineResourceAttribute   onlineResource;
  };

  /** Style URL Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsStyleUrlProperty
  {
    QString                         format;
    QgsWmsOnlineResourceAttribute   onlineResource;
  };

  /** Style Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsStyleProperty
  {
    QString                               name;
    QString                               title;
    QString                               abstract;
    std::vector<QgsWmsLegendUrlProperty>  legendUrl;
    QgsWmsStyleSheetUrlProperty           styleSheetUrl;
    QgsWmsStyleUrlProperty                styleUrl;
  };

  /** Authority URL Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsAuthorityUrlProperty
  {
    QgsWmsOnlineResourceAttribute   onlineResource;
    QString                         name;             // XML "NMTOKEN" type
  };

  /** Identifier Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsIdentifierProperty
  {
    QString   authority;
  };

  /** Metadata URL Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsMetadataUrlProperty
  {
    QString                         format;
    QgsWmsOnlineResourceAttribute   onlineResource;
    QString                         type;             // XML "NMTOKEN" type
  };

  /** Data List URL Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsDataListUrlProperty
  {
    QString                         format;
    QgsWmsOnlineResourceAttribute   onlineResource;
  };

  /** Feature List URL Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsFeatureListUrlProperty
  {
    QString                         format;
    QgsWmsOnlineResourceAttribute   onlineResource;
  };

  /** Layer Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsLayerProperty
  {
    // WMS layer properties
    QString                                     name;
    QString                                     title;
    QString                                     abstract;
    QStringList                                 keywordList;
    std::vector<QString>                        crs;        // coord ref sys
    QgsRect                                     ex_GeographicBoundingBox;
    std::vector<QgsWmsBoundingBoxProperty>      boundingBox;
    std::vector<QgsWmsDimensionProperty>        dimension;
    QgsWmsAttributionProperty                   attribution;
    std::vector<QgsWmsAuthorityUrlProperty>     authorityUrl;
    std::vector<QgsWmsIdentifierProperty>       identifier;
    std::vector<QgsWmsMetadataUrlProperty>      metadataUrl;
    std::vector<QgsWmsDataListUrlProperty>      dataListUrl;
    std::vector<QgsWmsFeatureListUrlProperty>   featureListUrl;
    std::vector<QgsWmsStyleProperty>            style;
    double                                      minScaleDenominator;
    double                                      maxScaleDenominator;
    std::vector<QgsWmsLayerProperty>            layer;      // nested layers

    // WMS layer attributes
    bool               queryable;
    int                cascaded;
    bool               opaque;
    bool               noSubsets;
    int                fixedWidth;
    int                fixedHeight;
  };

  /** Capability Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsCapabilityProperty
  {
    QgsWmsRequestProperty           request;
    QgsWmsExceptionProperty         exception;
    QgsWmsLayerProperty             layer;
  };

  /** Capabilities Property structure */
  // TODO: Fill to WMS specifications
  struct QgsWmsCapabilitiesProperty
  {
    QgsWmsServiceProperty           service;
    QgsWmsCapabilityProperty        capability;
    QString                         version;
  };


/**

  \brief Data provider for OGC WFS layers.

  This provider implements the
  interface defined in the QgsDataProvider class to provide access to spatial
  data residing in a OGC Web Feature Service.

  TODO: Make it work

*/
class QgsWfsProvider 
//: public QgsVectorDataProvider
: QObject // stick this in to make it compilable - in the long term reimplement QgsVectorDataProvider
{

  Q_OBJECT

public:


  /**
  * Constructor for the provider. 
  *
  * \param   uri   HTTP URL of the Web Server, optionally followed by a space then the proxy host name,
  *                another space, and the proxy host port.  If no proxy is declared then we will
  *                contact the host directly.
  *
  */
  QgsWfsProvider(QString const & uri = 0);

  //! Destructor
  virtual ~QgsWfsProvider();


  /**
   * \brief   Returns the caption error text for the last error in this provider
   *
   * If an operation returns 0 (e.g. draw()), this function
   * returns the text of the error associated with the failure.
   * Interactive users of this provider can then, for example,
   * call a QMessageBox to display the contents.
   */
  QString errorCaptionString();

  /**
   * \brief   Returns the verbose error text for the last error in this provider
   *
   * If an operation returns 0 (e.g. draw()), this function
   * returns the text of the error associated with the failure.
   * Interactive users of this provider can then, for example,
   * call a QMessageBox to display the contents.
   */

  QString errorString();

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


signals:

    /** \brief emit a signal to notify of a progress event */
    void setProgress(int theProgress, int theTotalSteps);

    /** \brief emit a signal to be caught by qgisapp and display a msg on status bar */
    void setStatus(QString const &  theStatusQString);


public slots:

//  void showStatusMessage(QString const &  theMessage);


private:



  //! Data source URI of the WMS for this layer
  QString httpuri;

  //! URL part of URI (httpuri)
  QString baseUrl;

  //! HTTP proxy host name for the WMS for this layer
  QString httpproxyhost;

  //! HTTP proxy port number for the WMS for this layer
  Q_UINT16 httpproxyport;

  /**
   * Flag indicating if the layer data source is a valid WMS layer
   */
  bool valid;

  /**
   * Spatial reference id of the layer
   */
  QString srid;

  /**
   * Rectangle that contains the extent (bounding box) of the layer
   */
  QgsRect layerExtent;

  /**
   * Capabilities of the WFS Server (raw)
   */
  QByteArray httpcapabilitiesresponse;

  /**
   * Capabilities of the WFS Server
   */
  QDomDocument capabilitiesDOM;

  /**
   * Last Service Exception Report from the WFS Server
   */
  QDomDocument serviceExceptionReportDOM;

  /**
   * Parsed capabilities of the WFS Server
   */
  QgsWmsCapabilitiesProperty capabilities;
  
  /**
   * layers hosted by the WFS Server
   */
  std::vector<QgsWmsLayerProperty> layersSupported;

  /**
   * extents per layer (in WMS CRS:84 datum)
   */
  std::map<QString, QgsRect> extentForLayer;

  /**
   * available CRSs per layer
   */
  std::map<QString, std::vector<QString> > crsForLayer;

  /**
   * Active sublayers managed by this provider in a draw function, in order from bottom to top
   * (some may not be visible in a draw function, cf. activeSubLayerVisibility)
   */
  QStringList activeSubLayers;

  /**
   * Visibility status of the given active sublayer
   */
  std::map<QString, bool> activeSubLayerVisibility;

  /**
   * The error caption associated with the last WFS error.
   */
  QString mErrorCaption;

  /**
   * The error message associated with the last WFS error.
   */
  QString mError;

  //! A QgsCoordinateTransform is used for transformation of WFS layer extents
  QgsCoordinateTransform * mCoordinateTransform;

  //! See if calculateExtents() needs to be called before extent() returns useful data
  bool extentDirty;

};

#endif

// ENDS
