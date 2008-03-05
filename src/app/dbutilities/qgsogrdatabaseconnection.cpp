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

 /* $Id$ */

#include "qgsogrdatabaseconnection.h"
#include "qgsdatasourceuri.h"


QgsOgrDatabaseConnection::QgsOgrDatabaseConnection(){

}





/**
 * Class constructor
 */
QgsOgrDatabaseConnection::QgsOgrDatabaseConnection(QgsConnectionParameters* connParameters)
    : QgsDatabaseConnection(connParameters){                            
  //generate uri according to connParameters                                

}


/**
 * Class destructor
 */
QgsOgrDatabaseConnection::~QgsOgrDatabaseConnection(){

}


/**
 * Connects to ogr database
 */
bool QgsOgrDatabaseConnection::connect(){
  QString uri;   
  bool result=true;   
  //QgsURIManager* uriman= new QgsURIManager(mConnectionParameters);   
  //qDebug("QgsOgrDatabaseConnection::connect: "+ mConnection.name);
  //qDebug("QgsOgrDatabaseConnection::connect: "+ uriman->getURI());
  if ((this->uri().isEmpty())or(this->uri().isNull()))
   {
     //generateURI According to connectionParameters
     qDebug("not generic");
     QgsDataSourceURI dsUri;
      dsUri.setConnection(mConnectionParameters->type, 
             mConnectionParameters->host,
		     mConnectionParameters->port,
		     mConnectionParameters->database,
		     mConnectionParameters->user,
		     mConnectionParameters->password );
	   uri=dsUri.uri();  
   } 
  else
   {
     uri=this->uri();            
   }
  qDebug("inche uri:"+uri);  
  //clean all previous errors
  CPLErrorReset(); 
  // Register all OGR-drivers
  OGRRegisterAll();
  OGRDataSource *poDS;
  poDS = OGRSFDriverRegistrar::Open(uri, FALSE );

  if( poDS == NULL ) {
       QString error=QObject::tr( "Open failed.\n" );
	   error=error + QString (CPLGetLastErrorMsg());
	   setError(error);
       return (false);
    } 
  OGRDataSource::DestroyDataSource( poDS );
  return result;       
}


/**
 * Returns the base key for ogr database type connections
 */
QString QgsOgrDatabaseConnection::baseKey(){

	return  NULL;
}


/**
 * Returns the geometry tables of OGR database
 */
QList<QgsGeometryColumnDescription *> QgsOgrDatabaseConnection::geometryTables(){

	//return  NULL;
}


/**
 * Returns the geometry given a table name
 */
QString QgsOgrDatabaseConnection::tableGeometry(QString tableName){

	return  NULL;
}


/**
 * Disconnects from OGR database
 */
void QgsOgrDatabaseConnection::disconnect(){

}


/**
 * Returs true if connected to OGR database
 */
void QgsOgrDatabaseConnection::isConnected(){

}
