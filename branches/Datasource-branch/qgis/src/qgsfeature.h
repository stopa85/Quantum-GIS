/***************************************************************************
    qgsfeature.h - Spatial Feature Class
     --------------------------------------
    Date                 : 09-Sep-2003
    Copyright            : (C) 2003 by Gary E.Sherman
    email                : sherman at mrcc.com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* $Id$ */

#ifndef QGSFEATURE_H
#define QGSFEATURE_H
#include <qstring.h>
#include <map>
#include "qgsfeatureattribute.h"

class QString;

/** \class QgsFeature - Feature attribute class.
 * Encapsulates a single feature including id and field/value.
  *@author Gary E.Sherman
  */

class QgsFeature {

public:
	//! Constructor
	QgsFeature();
	//! Destructor
	~QgsFeature();
	/**
	* Get the feature id for this feature
	* @return Feature id
	*/
	QString featureId();
	/**
	* Get the attributes for this feature.
	* @return A std::map containing the field name/value mapping
	*/
	std::map<QString, QgsFeatureAttribute> attributeMap();
	/**
	* Get the fields for this feature
	* @return A std::map containing field position (index) and field name
	*/
	std::map<int, QString> fields();
	
	unsigned char * getGeometry();
	char * wellKnownText();
	void setGeometry(unsigned char *);
    void setWellKnownText(char *);
private:
	//! feature id
	QString fId;
	//! std::map containing field name/value pairs
	std::map<QString, QgsFeatureAttribute> attributes;
	//! std::map containing the field index and name
	std::map<int, QString> fieldNames;
	unsigned char *geometry;
    char * wkt;
};

#endif
