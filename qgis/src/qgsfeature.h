/***************************************************************************
                          qgsfeature.h  -  description
                             -------------------
    begin                : Mon Sep 01 2003
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
/* $Id */
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
	QgsFeature();
	~QgsFeature();
	QString fieldId();
	std::map<QString, QgsFeatureAttribute> attributeMap();
	std::map<int, QString> fields();
 protected:
 //! Path or uri of the datasource
 private:
 	QString fId;
	std::map<QString, QgsFeatureAttribute> attributes;
	std::map<int, QString> fieldNames;
};

#endif
