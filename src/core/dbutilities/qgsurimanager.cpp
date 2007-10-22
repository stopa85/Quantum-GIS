/***************************************************************************
    qgsurimaneger.cpp - Database URI connection manager
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

#include "qgsurimanager.h"

QgsURIManager::QgsURIManager(QString const & uri)
 {
   if ((uri.contains("type=MySQL",FALSE)>0)
        ||(uri.contains("type=PostgreSQL",FALSE)>0)
        ||(uri.contains("type=Oracle",FALSE)>0))
    {
     //is a database params string, need to decode   
     QStringList paramList;
     paramList=paramList.split(",",uri);                                     
     
     QString aux;
     aux = paramList.at(0);
     aux = aux.section('=', 1, 1 ); 
     setType("Ogr"+aux);
     
     aux = paramList.at(1);
     aux = aux.section('=', 1, 1 ); 
     setServer(aux);
     
     aux = paramList.at(2);
     aux = aux.section('=', 1, 1 ); 
     setDatabase(aux);
     
     aux = paramList.at(3);
     aux = aux.section('=', 1, 1 ); 
     setUser(aux);
     
     aux = paramList.at(4);
     aux = aux.section('=', 1, 1 ); 
     setPassword(aux);
     
     aux = paramList.at(5);
     aux = aux.section('=', 1, 1 ); 
     setLayer(aux);
        
    }
   else
    {
      //check for postgresql type
        
      //no further processing is a file dataset                                    
      mType=ogrFile;
      mURI=uri;
      
    }   
   
 }
 
QgsURIManager::QgsURIManager(QgsConnection conn)
 {
     qDebug("QgsURIManager::QgsURIManager QgsConnection constructor");                                      
     setType(conn.type);
     setServer(conn.host);
     setDatabase(conn.database);
     setUser(conn.user);
     setPassword(conn.password);
     setLayer("");
     setPort(conn.port);
 } 
 
QgsURIManager::~QgsURIManager()
 {
 }
 
void QgsURIManager::setType(QString const type)
 {
    if (type=="OgrMySQL")                            
    mType=ogrMySQL; 
    else if (type=="OgrPostgreSQL")
    mType=ogrPostgreSQL; 
    else if (type=="OgrOracle")                            
    mType=ogrOracle;    
    else if (type=="PostgreSQL")                            
    mType=PostgreSQL;    
    else
     mType=ogrNone;                                                               
 }
 
void QgsURIManager::setServer(QString const server)
 {
   mServer=server;                                 
 }
 
void QgsURIManager::setDatabase(QString const database)
 {
   mDatabase=database;                                 
 }
 
void QgsURIManager::setUser(QString const user)
 {
   mUser=user;                                 
 }
 
void QgsURIManager::setPassword(QString const password)
 {
   mPassword=password;                                 
 }
void QgsURIManager::setPort(QString const port)
 {
   mPort=port;                                 
 } 
 
void QgsURIManager::setLayer(QString const layer)
 {
   mLayer=layer;                                 
 }

QString QgsURIManager::getLayer()
 {
   return mLayer;                                 
 }
 
QString QgsURIManager::getURI()
 {
   QString connString;
                                 
   switch (mType)
    { 
     case ogrMySQL:{
             connString="MySQL:"+mDatabase+","+
                 "host="+mServer+","+
                 "user="+mUser+","+
                 "password="+mPassword;
             break;    
          } 
                              
     case ogrPostgreSQL:{
             connString="PG:dbname="+mDatabase+" "+
                 "host="+mServer+" "+
                 "user="+mUser+" "+
                 "password="+mPassword;
             break;    
          }      
     case ogrOracle:{
             connString="OCI:"+mUser+"/"+
                 mPassword+"@"+
                 mServer+"/"+
                 mDatabase;
             break;    
          }           
     case ogrFile:{
             connString=mURI;
             break;    
          }                                        
                      
         
    }                            
   return connString;                                 
 }

QString QgsURIManager::getParamsString()
 {
    QString connParams;
                                 
   switch (mType)
    { 
     case ogrMySQL:{
          connParams=QString("type=MySQL,")
                    +"server="+mServer+","
                    +"database="+mDatabase+","
                    +"user="+mUser+","
                    +"password="+mPassword+ ",";
                    //+"table="+mLayer;
          break;                   
          } 
                              
     case ogrPostgreSQL:{
          connParams=QString("type=PostgreSQL,")
                    +"server="+mServer+","
                    +"database="+mDatabase+","
                    +"user="+mUser+","
                    +"password="+mPassword+ ",";
                    //+"table="+mLayer;
          break;
          }    
     case ogrOracle:{
          connParams=QString("type=Oracle,")
                    +"server="+mServer+","
                    +"database="+mDatabase+","
                    +"user="+mUser+","
                    +"password="+mPassword+ ",";
                    //+"table="+mLayer;
          break;
          }         
     case PostgreSQL:{
          connParams="host=" + mServer
                    +" dbname=" + mDatabase
                    +" port=" + mPort
                    +" user=" + mUser
                    +" password='" + mPassword + "'";
                    //+" table="+mLayer;
          break;
          }                        
     }  
             
  return connParams;      
 }        
