/***************************************************************************
      qgsshapefileprovider.h  -  Data provider for ESRI shapefile format
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

#include "../qgsdataprovider.h"
class QgsFeature;
class OGRDataSource;
class OGRLayer;
/**
\class QgsShapeFileProvider
\brief Data provider for ESRI shapefiles
*/
class QgsShapeFileProvider : public QgsDataProvider {
public:
	QgsShapeFileProvider(QString uri=0);
	virtual ~QgsShapeFileProvider();
/**
	* Get the first feature resutling from a select operation
	* @return QgsFeature
	*/
	QgsFeature * getFirstFeature();
	/** 
	* Get the next feature resutling from a select operation
	* @return QgsFeature
	*/
	QgsFeature * getNextFeature();
	/**
	* Select features based on a bounding rectangle. Features can be retrieved 
	* with calls to getFirstFeature and getNextFeature.
	* @param mbr QgsRect containing the extent to use in selecting features
	*/
	void select(QgsRect mbr);
	/** 
		* Set the data source specification. This may be a path or database
	* connection string
	* @uri data source specification
	*/
	void setDataSourceUri(QString uri);
	
		/** 
	* Get the data source specification. This may be a path or database
	* connection string
	* @return data source specification
	*/
	QString getDataSourceUri();
	
	/**
	* Identify features within the search radius specified by rect
	* @param rect Bounding rectangle of search radius
	* @return std::vector containing QgsFeature objects that intersect rect
	*/
	virtual std::vector<QgsFeature> identify(QgsRect *rect);

  /** Return endian-ness for this layer
  */	
	int endian();

  /** Return the extent for this data layer
  */
  virtual QgsRect * extent();
  
private:
	unsigned char *getGeometryPointer(OGRFeature *fet);
	std::vector<QgsFeature> features;
	std::vector<bool> * selected;
	QString dataSourceUri;
	OGRDataSource *ogrDataSource;
	OGREnvelope *extent_;
	OGRLayer *ogrLayer;
	bool valid;
	int geometryType;
	enum ENDIAN
	{
		NDR = 1,
		XDR = 0
	};
};
