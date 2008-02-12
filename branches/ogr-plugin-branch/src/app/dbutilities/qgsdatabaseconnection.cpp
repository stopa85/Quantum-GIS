/***************************************************************************
    qgsdatabaseconnection.cpp - Spatial Database connection interface
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


#include "QgsDatabaseConnection.h"


QgsDatabaseConnection::QgsDatabaseConnection(){

}





QgsDatabaseConnection::QgsDatabaseConnection(QgsConnectionParameters* connectionParameters){
  mConnectionParameters=connectionParameters;
}


/**
 * Destructor for class
 */
QgsDatabaseConnection::~QgsDatabaseConnection(){

}


/**
 * Connects to database
 */
bool QgsDatabaseConnection::connect(){

	return false;
}


/**
 * Returns base key for connection
 */
QString QgsDatabaseConnection::baseKey(){
}


/**
 * Return error message.
 */
QString QgsDatabaseConnection::error(){
   return mError;     
}


/**
 * Returns a list of geometry tables available in the database.
 */
GeometryColumns QgsDatabaseConnection::geometryTables(bool searchGeometryColumnsOnly, bool searchPublicSchemaOnly){
}


/**
 * Returns the geometry of a given table
 */
QString QgsDatabaseConnection::tableGeometry(QString tableName){
}

/**
 * Sets error message  for connection
 */
void QgsDatabaseConnection::setError(QString error)
{
   mError=error;  
}
/**
 * Sets uri string for connection
 */
void QgsDatabaseConnection::setUri(QString uri){
  mUri=uri;      
}

/**
 * Returns uri string for connection
 */
QString QgsDatabaseConnection::uri(){
  return mUri;      
}
/**
 * Disconnect from database
 */
void QgsDatabaseConnection::disconnect(){

}


/**
 * Returns true if it is connected to database.
 */
bool QgsDatabaseConnection::isConnected(){

	return false;
}
