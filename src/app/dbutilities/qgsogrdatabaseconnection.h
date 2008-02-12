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

#include "cpl_port.h"
#include <ogr_api.h>
#include <ogrsf_frmts.h>

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
	QString baseKey();
	QStringList geometryTables();
	QString tableGeometry(QString tableName);
	void disconnect();
	void isConnected();
	

};
#endif // !defined(EA_55811BBB_229C_4411_8F41_521DE4B175F7__INCLUDED_)
