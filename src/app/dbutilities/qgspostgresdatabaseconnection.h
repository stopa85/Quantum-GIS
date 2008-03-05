/***************************************************************************
    qgspostgresdatabaseconnection.cpp - Connection Class for Postgres
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

 /* $Id$ */



#ifndef QGSPOSTGRESDATABASECONNECTION_H
#define QGSPOSTGRESDATABASECONNECTION_H

#include "qobject.h"
#include "qgsgeomcolumntypethread.h"
#include "qgsconnectionparameters.h"
#include "qgsdatabaseconnection.h"
#include "qstringlist.h"
#include "qgsgeometrycolumndescription.h"



extern "C"
{
#include <libpq-fe.h>
}

/**
 * Class to connect to Postgres spatial databases
 */
class QgsPostgresDatabaseConnection : public QgsDatabaseConnection
{

public:
	typedef std::pair<QString, QString> geomPair;

	typedef std::list<geomPair > geomCol;


	QgsPostgresDatabaseConnection();

	QgsPostgresDatabaseConnection(QgsConnectionParameters* conn);
	~QgsPostgresDatabaseConnection();
	bool connect();
	QString baseKey();
	QList<QgsGeometryColumnDescription *> geometryTables();
	QList<QgsGeometryColumnDescription *> geometryTables(bool searchGeometryColumnsOnly, bool searchPublicSchemaOnly);
	QString tableGeometry(QString tableName);
	QString tableGeometryFromData(QString schema, QString tableName, QString column);

private:
	QgsGeomColumnTypeThread* mColumnTypeThread;

	QString fullDescription(QString schema, QString table, QString column);
//	bool geometryTableInformation(PGconn* pg, GeometryColumns& details, bool searchGeometryColumnsOnly, bool searchPublicOnly);
	bool geometryColumnInformation(PGconn *pg,GeometryColumns& details, bool searchGeometryColumnsOnly, bool searchPublicOnly);
	
	PGconn *pd;

};
#endif 
