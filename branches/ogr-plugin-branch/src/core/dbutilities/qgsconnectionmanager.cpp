/***************************************************************************
 qgsconnectionmanager.cpp   Class to manage connections to spatial databases
                              -------------------
  begin                : October 2, 2007
  copyright            : (C) 2007 by Godofredo Contreras
  email                : frdcn at hotmail dot com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "qgsconnectionmanager.h"



QgsConnectionManager::QgsConnectionManager()
 {
                                           
 }     
QgsConnectionManager::~QgsConnectionManager()     
 {
                                           
 }
bool QgsConnectionManager::saveConnection(QgsConnection conn)
 {
   QSettings settings; 
   QString baseKey;
   
   if (conn.type=="PostgreSQL")
     {
       baseKey="/PostgreSQL/connections/";                                      
     }
   else if (conn.type=="OgrPostgreSQL")
     {
       baseKey= "/ogr/PostgreSQL/connections/";                                         
     }                                        
   else if (conn.type=="OgrMySQL")
     {
       baseKey="/ogr/MySQL/connections/";                                         
     }                                         
   else if (conn.type=="OgrOracle")
     {
       baseKey= "/ogr/Oracle/connections/";                                         
     }                  
     
   qDebug("baseKey : "+baseKey);                           
   qDebug("Connection : "+conn.name);
   settings.writeEntry(baseKey + "selected", conn.selected);
   baseKey += conn.name;
   settings.writeEntry(baseKey + "/host", conn.host);
   settings.writeEntry(baseKey + "/database", conn.database);
   settings.writeEntry(baseKey + "/port", conn.port);
   settings.writeEntry(baseKey + "/username", conn.user);
   settings.writeEntry(baseKey + "/password", conn.password);
   settings.writeEntry(baseKey + "/publicOnly", conn.publicOnly);
   settings.writeEntry(baseKey + "/geometryColumnsOnly", conn.geometryColumnsOnly);
   if (conn.save)
     {
      settings.writeEntry(baseKey + "/save", "true");
     } else
     {
      settings.writeEntry(baseKey + "/save", "false");
     }
     
  } 
  
bool QgsConnectionManager::removeConnection(QString type,QString name)
 {
   QSettings settings; 
   QString baseKey;
   
   if (type=="PostgreSQL")
     {
       baseKey="/PostgreSQL/connections/";                                      
     }
   else if (type=="OgrPostgreSQL")
     {
       baseKey= "/ogr/PostgreSQL/connections/";                                         
     }                                        
   else if (type=="OgrMySQL")
     {
       baseKey="/ogr/MySQL/connections/";                                         
     }                                         
   else if (type=="OgrOracle")
     {
       baseKey= "/ogr/Oracle/connections/";                                         
     }                  
     
    qDebug("QgsConnectionManager::removeConnection baseKey : "+baseKey);                           
    qDebug("QgsConnectionManager::removeConnection Connection : "+name);
   
    baseKey += name; 
    settings.removeEntry(baseKey + "/host");
    settings.removeEntry(baseKey + "/database");
    settings.removeEntry(baseKey + "/username");
    settings.removeEntry(baseKey + "/password");
    settings.removeEntry(baseKey + "/port");
    settings.removeEntry(baseKey + "/save");
    settings.removeEntry(baseKey + "/publicOnly");
    settings.removeEntry(baseKey + "/geometryColumnsOnly");
    settings.removeEntry(baseKey);
     
	
 }
bool QgsConnectionManager::modifyConnection(QgsConnection conn)
 {
                                           
 }
 
QStringList QgsConnectionManager::getConnections(QString type)
 {
  QSettings settings;
  QString baseKey;
  
  qDebug("QgsConnectionManager::getConnections : type "+type); 
  
  if (type=="PostgreSQL")
     {
       baseKey="/PostgreSQL/connections/";                                      
     }
  else if (type=="OgrPostgreSQL")
     {
       baseKey= "/ogr/PostgreSQL/connections/";                                         
     }                                        
  else if (type=="OgrMySQL")
     {
       baseKey="/ogr/MySQL/connections/";                                         
     }                                         
  else if (type=="OgrOracle")
     {
       baseKey= "/ogr/Oracle/connections/";                                         
     }                  
  
  QStringList keys = settings.subkeyList(baseKey);
  QStringList connections;
  QStringList::Iterator it = keys.begin();
  while (it != keys.end())
  {
    connections << *it;
    ++it;
  }
  
  return connections;
 }

QgsConnection QgsConnectionManager::getConnectionDetails(QString type, QString name)
 {
   QSettings settings;
   QString baseKey;
   QgsConnection conn;
  
  qDebug("QgsConnectionManager::getConnectionDetails : type "+type); 
  
  if (type=="PostgreSQL")
     {
       baseKey="/PostgreSQL/connections/";                                      
     }
  else if (type=="OgrPostgreSQL")
     {
       baseKey= "/ogr/PostgreSQL/connections/";                                         
     }                                        
  else if (type=="OgrMySQL")
     {
       baseKey="/ogr/MySQL/connections/";                                         
     }                                         
  else if (type=="OgrOracle")
     {
       baseKey= "/ogr/Oracle/connections/";                                         
     }                  
  
   baseKey +=name;
   conn.name=name;
   conn.type=type;
   conn.host=settings.readEntry(baseKey + "/host");
   conn.database=settings.readEntry(baseKey + "/database");
   conn.port=settings.readEntry(baseKey + "/port");
   conn.user=settings.readEntry(baseKey + "/username");
   conn.password=settings.readEntry(baseKey + "/password");
   conn.publicOnly=settings.readBoolEntry(baseKey + "/publicOnly");
   conn.geometryColumnsOnly=settings.readBoolEntry(baseKey + "/geometryColumnsOnly");
   conn.save=settings.readBoolEntry(baseKey + "/save");
   qDebug("QgsConnectionManager::getConnectionDetails : host "+conn.host); 
   return conn;
 } 
 

QgsDatabaseConnectionBase* QgsConnectionManager::getConnection()
 {
   return mConnection;                        
 } 

bool QgsConnectionManager::connect(QString type, QgsConnection conn)
 {
    bool result; 
    if (type=="PostgreSQL")
      {
        qDebug("QgsConnectionManager::connect: connecting to postgres database");                   
        mConnection= new QgsPostgresDatabaseConnection(conn);                   
        qDebug("QgsConnectionManager::connect: postgres connection object created");
        result=mConnection->connect();
        mError=mConnection->getError();
      }
    else if (type.startsWith("Ogr"))
     {
        qDebug("QgsConnectionManager::connect: connecting to ogr database");                    
        mConnection=new QgsOgrDatabaseConnection(conn);                   
        result=mConnection->connect();
        mError=mConnection->getError();
     }
    else
     {  
       mError=QObject::tr("Database type not supported");  
       return false;             
     }
  return result;
 }

QString QgsConnectionManager::getError()
 {
   return mError;  
 }

void QgsConnectionManager::setSelected(QString type, QString name)
 {  
    QSettings settings;
    QString baseKey;
    
    if (type=="PostgreSQL")
     {
       baseKey="/PostgreSQL/connections/";                                      
     }
    else if (type=="OgrPostgreSQL")
     {
       baseKey= "/ogr/PostgreSQL/connections/";                                         
     }                                        
    else if (type=="OgrMySQL")
     {
       baseKey="/ogr/MySQL/connections/";                                         
     }                                         
    else if (type=="OgrOracle")
     {
       baseKey= "/ogr/Oracle/connections/";                                         
     }         
    settings.writeEntry(baseKey+"selected",name);  
 }
 
void QgsConnectionManager::setSelectedType(QString type)
 {  
    QSettings settings;
    QString baseKey;
    
    baseKey="/Database/SelectedConnectionType/";                                      
    settings.writeEntry(baseKey+"selected",type);  
 }
  
QString QgsConnectionManager::getSelected(QString type)
 {
    QSettings settings;
    QString result;
    
    if (type=="PostgreSQL")
     {
       result=settings.readEntry("/PostgreSQL/connections/selected");                                      
     }
    else if (type=="OgrPostgreSQL")
     {
       result=settings.readEntry("/ogr/PostgreSQL/connections/selected");                                         
     }                                        
    else if (type=="OgrMySQL")
     {
       result=settings.readEntry("/ogr/MySQL/connections/selected");                                         
     }                                         
    else if (type=="OgrOracle")
     {
       result=settings.readEntry("/ogr/Oracle/connections/selected");                                         
     }    
    qDebug("Selected connection from type "+type+" "+result);      
    return result;
 }

QString QgsConnectionManager::getSelectedType()
 {  
    QSettings settings;
    QString result;
    
    result=settings.readEntry("/Database/SelectedConnectionType/selected");                                      
    return result;
 }
