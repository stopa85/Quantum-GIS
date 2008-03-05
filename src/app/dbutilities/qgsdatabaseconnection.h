/***************************************************************************
    qgsdatabaseconnection.h - Spatial Database connection interface
     --------------------------------------
    Date                 : 10-Oct-2007
    Copyright            : (C) 2007 by Godofredo Contreras
    Email                : frdc at hotmail dot com
***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

/* $Id$ */


#ifndef QGSDATABASECONNECTION_H
#define QGSDATABASECONNECTION_H

#include "qstringlist.h"
#include "qgsconnectionparameters.h"
#include "qgsgeometrycolumndescription.h"

typedef std::pair<QString, QString> GeometryPair;
typedef std::list<GeometryPair > GeometryColumns;

/**
 * Interface for classes that will connect to spatial databases
 */
class QgsDatabaseConnection
{

public:
	QgsDatabaseConnection();

	QgsDatabaseConnection(QgsConnectionParameters* connectionParameters);
	virtual ~QgsDatabaseConnection();
	virtual bool connect();
	QString baseKey();
	QString error();
	//virtual QList<QgsGeometryColumnDescription> geometryTables(bool searchGeometryColumnsOnly, bool searchPublicSchemaOnly);
	virtual QList<QgsGeometryColumnDescription *> geometryTables();
	virtual QString tableGeometry(QString tableName);
	virtual QString tableGeometryFromData(QString schema, QString tableName, QString column);
	void setError(QString error);
	void setUri(QString uri);
	QString uri();
	void disconnect();
	bool isConnected();
	

protected:
	/**
	 * Variable to store the connection QgsConnectionParams struct, passed in the
	 * constructor or through.
	 */
	QgsConnectionParameters* mConnectionParameters;
	/**
	 * Variable to store the registry base key for the connection
	 */
	QString mBaseKey;
	/**
	 * Variable to store error messages
	 */
	QString mError;
	/**
	 * Variable to store uri string
	 */
	QString mUri;

};
#endif 
