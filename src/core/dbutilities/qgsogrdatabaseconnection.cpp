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

#include "qgsogrdatabaseconnection.h"
QgsOgrDatabaseConnection::QgsOgrDatabaseConnection(QgsConnection conn)
:QgsDatabaseConnectionBase(conn)
 {
 }
  
QgsOgrDatabaseConnection::~QgsOgrDatabaseConnection()
 {
 }
bool QgsOgrDatabaseConnection::connect()
 {
  bool result=true;   
  QgsURIManager* uriman= new QgsURIManager(mConnection);   
  qDebug("QgsOgrDatabaseConnection::connect: "+ mConnection.name);
  qDebug("QgsOgrDatabaseConnection::connect: "+ uriman->getURI());
 
  //clean all previous errors
  CPLErrorReset(); 
  // Register all OGR-drivers
  OGRRegisterAll();
  OGRDataSource *poDS;
  poDS = OGRSFDriverRegistrar::Open(uriman->getURI(), FALSE );

  if( poDS == NULL ) {
        mError=QObject::tr( "Open failed.\n" );
		mError=mError + QString (CPLGetLastErrorMsg());
        result=false;
    } 
	
  OGRDataSource::DestroyDataSource( poDS );
  delete(uriman);
  return result;     
 }
 
QStringList QgsOgrDatabaseConnection::getGeometryTables()
 {
            
  QgsURIManager* uriman= new QgsURIManager(mConnection);   
  
 
  //clean all previous errors
  CPLErrorReset(); 
  // Register all OGR-drivers
  OGRRegisterAll();
  OGRDataSource *poDS;
  poDS = OGRSFDriverRegistrar::Open(uriman->getURI(), FALSE );

  if( poDS == NULL ) {
        mError=QObject::tr( "Open failed.\n" );
		mError=mError + QString (CPLGetLastErrorMsg());
    } 

  //generate the table list
  // get layercount
  int nLayers = poDS->GetLayerCount(); // poLayerCount;
	
  // create a pointer to the layer
  OGRLayer  *poLayer;
  // create a pointer to the feature
  //OGRFeature *poFeature;
  // create a pointer to the feature-definition
  OGRFeatureDefn *poFDefn;
        
  //layer in lstTables
  // Here's an annoying thing... calling clear() removes the
  // header items too, so we need to reinstate them after calling
  // clear(). 
  
  QStringList result;
  //result->clear(); 
  
  for (int j = 0; j < nLayers  ; j++) {
 	poLayer = poDS->GetLayer( j );
	poLayer->ResetReading();
	poFDefn = poLayer->GetLayerDefn();
	// show names of the layer
	qDebug("Name of Layer: " + QString(poFDefn->GetName()));
	// show geometry of layer
	qDebug("Geometry of Layer: " +QString(poFDefn->GetGeomType()));
    result << poFDefn->GetName();		
	} // end of for()
  
  OGRDataSource::DestroyDataSource( poDS );
  delete(uriman);
  return result;                
 }
 
QString QgsOgrDatabaseConnection::getTableGeometry(QString tableName)
 {
  QgsURIManager* uriman= new QgsURIManager(mConnection);   
  
 
  //clean all previous errors
  CPLErrorReset(); 
  // Register all OGR-drivers
  OGRRegisterAll();
  OGRDataSource *poDS;
  poDS = OGRSFDriverRegistrar::Open(uriman->getURI(), FALSE );

  if( poDS == NULL ) {
        mError=QObject::tr( "Open failed.\n" );
		mError=mError + QString (CPLGetLastErrorMsg());
    } 

  //generate the table list
  // get layercount
  
	
  // create a pointer to the layer
  OGRLayer  *poLayer;
  // create a pointer to the feature
  //OGRFeature *poFeature;
  // create a pointer to the feature-definition
  OGRFeatureDefn *poFDefn;
        
        
  poLayer = poDS->GetLayerByName(tableName);
  poLayer->ResetReading();
  poFDefn = poLayer->GetLayerDefn();
  // show geometry of layer
  qDebug("Geometry of Layer: " +QString(poFDefn->GetGeomType()));
  QString result;
  switch (poFDefn->GetGeomType())
   {
      case wkbUnknown:result="UNKNOWN";break;
      case wkbPoint: result="POINT";break;
      case wkbMultiPoint: result="MULTIPOINT";break;
      case wkbLineString: result="LINESTRING";break;
      case wkbMultiLineString: result="MULTILINESTRING";break;
      case wkbPolygon: result="POLYGON";break;
      case wkbMultiPolygon:result="MULTIPOLYGON";break;
      case wkbGeometryCollection:result="GEOMETRYCOLLECTION";break;
      case wkbNone:result="NONE";break;
      /*case wkbLinearRing:
      case wkbPoint25D:
      case wkbLineString25D:
      case wkbPolygon25D:
      case wkbMultiPoint25D:
      case wkbMultiLineString25D:
      case wkbMultiPolygon25D:
      case wkbGeometryCollection25D:*/
   }

    
  OGRDataSource::DestroyDataSource( poDS );
  delete(uriman);
  return result;          
        
 } 


