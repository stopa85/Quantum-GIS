/***************************************************************************
                          gsdatasource.h  -  description
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
/* $Id */
#ifndef QGSDATASOURCE_H
#define QGSDATASOURCE_H
#include <vector>
#include <qstring.h>
#include "qgsrect.h"
class QgsFeature;
/**Base class for spatial and tabular data
  *@author Gary E.Sherman
  */

class QgsDataSource {

public: 
	/* QgsDataSource(); 
	~QgsDataSource(); */
	virtual std::vector<QgsFeature> identify(QgsRect *rect)=0;
	virtual void filter(QgsRect *rect)=0;
	virtual QgsFeature *getNextFeature()=0;
	virtual QgsFeature *getFirstFeature()=0;
	virtual QString getDataSourceUri()=0;
/*  protected:
 //! Path or uri of the datasource
 	QString dataSource; */
};
//! Typedef for the function that returns a generic pointer to a 
// datasource plugin object
typedef QgsDataSource* create_datasource();
#endif
