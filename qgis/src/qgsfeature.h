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

/** Feature attribute class
  *@author Gary E.Sherman
  */

class QgsFeature {

public: 
	QgsFeature();
	~QgsFeature();
	QString fieldId();
	QString fieldName();
	QString fieldValue();
 protected:
 //! Path or uri of the datasource
 private:
 	QString fId;
	QString field;
	QString value;
};

#endif
