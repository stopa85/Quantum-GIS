/***************************************************************************
                          qgsdataprovider.h  -  description
                             -------------------
    begin                : Sun Jun 30 2002
    copyright            : (C) 2002 by Gary E.Sherman
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
#ifndef QQGSDATAPROVIDER_H
#define QQGSDATAPROVIDER_H
#include <qstring.h>

/** \class QgsDataProvider
* \brief Abstract base class for spatial data provider implementations
  *@author Gary E.Sherman
  */

class QgsDataProvider {

public: 
	/** 
	* Get the first feature resutling from a select operation
	* @return QgsFeature
	*/
	QgsFeature QgsDataProvider::getFirstFeature()=0;
	/** 
	* Get the next feature resutling from a select operation
	* @return QgsFeature
	*/
	QgsFeature QgsDataProvider::getNextFeature()=0;
	/**
	* Select features based on a bounding rectangle. Features can be retrieved 
	* with calls to getFirstFeature and getNextFeature.
	* @param extent QgsRect containing the extent to use in selecting features
	*/
	void QgsDataProvider::select()=0;
	/** 
		* Set the data source specification. This may be a path or database
	* connection string
	* @uri data source specification
	*/
	void QgsDataProvider::setDataSourceUri(QString uri) = 0;
	
		/** 
	* Get the data source specification. This may be a path or database
	* connection string
	* @return data source specification
	*/
	QString QgsDataProvider::getDataSourceUri() = 0;
	
	/**
	* Identify features within the search radius specified by rect
	* @param rect Bounding rectangle of search radius
	* @return std::vector containing QgsFeature objects that intersect rect
	*/
	virtual std::vector<QgsFeature> QgsDataProvider::identify(QgsRect *rect)=0;
	
 
};

#endif
