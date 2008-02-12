/***************************************************************************
  qgsconnectionparameters.h  Struct to store connection parameters
  ------------------------------------------------------------
  Date                 : October 2, 2007
  Copyright            : (C) 2007 by Godofredo Contreras
  Email                : frdcn at hotmail dot com
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

#ifndef QGSCONNECTIONPARAMETERS_H
#define QGSCONNECTIONPARAMETERS_H
#include "qstring.h"

struct QgsConnectionParameters
{

public:
	QString type;
	QString name;
	QString host;
	QString database;
	QString port;
	QString user;
	QString password;
	QString uri;
	bool geometryColumnsOnly;
	bool publicOnly;
	QString selected;
	bool save;

};
#endif 
