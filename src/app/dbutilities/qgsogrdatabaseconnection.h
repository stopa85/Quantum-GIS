/***************************************************************************
    qgsogrdatabaseconnection.cpp - Connection class for ogr databases
     --------------------------------------
    Date                 : 10-Oct-2007
    Copyright            : (C) 2007 by Godofredo Contreras
    email                : frdc at hotmail dot com
***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef QGSOGRDATABASECONNECTION_H
#define QGSOGRDATABASECONNECTION_H

#include "qgsdatabaseconnection.h"
#include "qgsconnectionparameters.h"

//#include "cpl_port.h"
//#include <ogr_api.h>
//#include <ogrsf_frmts.h>

#define CPL_SUPRESS_CPLUSPLUS

#include <ogr_api.h>
#include <ogr_srs_api.h>
#include <cpl_error.h>

/**
 * Class to connect to databases using OGR
 */
class QgsOgrDatabaseConnection : public QgsDatabaseConnection
{

public:
	QgsOgrDatabaseConnection();
	QgsOgrDatabaseConnection(QgsConnectionParameters* connectionParameters);
	~QgsOgrDatabaseConnection();
	bool connect();
	QList<QgsGeometryColumnDescription *> geometryTables();
	QString tableGeometry(QString tableName);
	QString tableGeometryFromData(QString schema, QString tableName, QString column);
	void disconnect();
	void isConnected();
	QString baseKey();
private:
    //ogr datasource    
    OGRDataSourceH poDS;
    //ogr driver
    OGRSFDriverH ogrDriver;    	   
};
#endif  
