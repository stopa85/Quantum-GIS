/***************************************************************************
    qgsfeature.cpp - Spatial Feature Implementation
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

#include <qstring.h>
#include "qgsfeature.h"
/** \class QgsFeature
* \brief Encapsulates a spatial feature with attributes
*/
//! Constructor
QgsFeature::QgsFeature(){}
//! Destructor
QgsFeature::~QgsFeature(){
	delete[] geometry;
}
/**
* Get the feature id for this feature
* @return Feature id
*/
QString QgsFeature::featureId(){
	return fId;
}
/**
* Get the attributes for this feature.
* @return A std::map containing the field name/value mapping
*/
std::map<QString, QgsFeatureAttribute> QgsFeature::attributeMap(){
	return attributes;
}
/**
* Get the fields for this feature
* @return A std::map containing field position (index) and field name
*/
std::map<int, QString> QgsFeature::fields(){
	return fieldNames;
}

/**
* Get the pointer to the feature geometry
*/
unsigned char *QgsFeature::getGeometry(){
	return geometry;
}

/** Set the pointer to the feature geometry
*/
void QgsFeature::setGeometry(unsigned char *geom){
	geometry = geom;
}
