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
  OGR_DS_Destroy(poDS);
  poDS = 0;
}


/**
 * Connects to ogr database
 */
bool QgsOgrDatabaseConnection::connect(){
  QString uri;   
  bool result=true;   
  
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
  //clean all previous errors
  CPLErrorReset(); 
  // Register all OGR-drivers
  OGRRegisterAll();
  
  
  
  CPLPushErrorHandler(CPLQuietErrorHandler);
  poDS = OGROpen(uri, FALSE,&ogrDriver);
  CPLPopErrorHandler();
  
  //poDS = OGRSFDriverRegistrar::Open(uri, FALSE );

  if( poDS == NULL ) {
       QString error=QObject::tr( "Open failed.\n" );
	   error=error + QString (CPLGetLastErrorMsg());
	   setError(error);
       return (false);
    } 
  //OGRDataSource::DestroyDataSource( poDS );
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


  bool searchGeometryColumnsOnly=mConnectionParameters->geometryColumnsOnly;
  bool searchPublicSchemaOnly=mConnectionParameters->publicOnly;
  
  QList<QgsGeometryColumnDescription *> details;  
  
   
  int nLayers = OGR_DS_GetLayerCount(poDS);
  
  // create a pointer to the layer
  OGRLayerH  poLayer;
  // create a pointer to the feature
  //OGRFeature *poFeature;
  // create a pointer to the feature-definition
  OGRFeatureDefnH poFDefn;
        
    
  QStringList result;
   
  
  for (int j = 0; j < nLayers  ; j++) {
    poLayer = OGR_DS_GetLayer(poDS,j);  
 	OGR_L_ResetReading(poLayer);   
	//poLayer->ResetReading();
	poFDefn = OGR_L_GetLayerDefn(poLayer);
	// show names of the layer
	qDebug("Name of Layer: " + QString(OGR_FD_GetName(poFDefn)));
	// show geometry of layer
	qDebug("Geometry of Layer: " +QString(OGR_FD_GetGeomType(poFDefn)));
    //result << poFDefn->GetName();		
	
  
    
    QString tableName = OGR_FD_GetName(poFDefn);
    QString schemaName = "public";
    QString type;
    //QString type = OGR_FD_GetGeomType(poFDefn);
    switch (OGR_FD_GetGeomType(poFDefn))
     {
      case wkbUnknown:type="UNKNOWN";break;
      case wkbPoint: type="POINT";break;
      case wkbMultiPoint: type="MULTIPOINT";break;
      case wkbLineString: type="LINESTRING";break;
      case wkbMultiLineString: type="MULTILINESTRING";break;
      case wkbPolygon: type="POLYGON";break;
      case wkbMultiPolygon:type="MULTIPOLYGON";break;
      case wkbGeometryCollection:type="GEOMETRYCOLLECTION";break;
      case wkbNone:type="NONE";break;     
     }
    //need to check for geometry column name
    QString column=""; 
    details.append(new QgsGeometryColumnDescription(type,schemaName,tableName,column));
        
	    
  }

  return  details;
}


/**
 * Returns the geometry given a table name
 */
QString QgsOgrDatabaseConnection::tableGeometry(QString tableName){

	return  NULL;
}

QString QgsOgrDatabaseConnection::tableGeometryFromData(QString schema, QString tableName, QString column){
  return NULL;
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
