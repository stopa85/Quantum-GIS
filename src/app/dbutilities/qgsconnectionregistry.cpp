/***************************************************************************
 qgsconnectionregistry.cpp   Class to save and load connection parameter
                            from qgis settings. 
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

/* $Id$ */

#include "QgsConnectionRegistry.h"
#include "qstringlist.h"
#include "qsettings.h"




QgsConnectionRegistry::QgsConnectionRegistry(){

}


/**
 * Class destructor
 */
QgsConnectionRegistry::~QgsConnectionRegistry(){

}


/**
 * gets connection params given a connection type and the connection name
 */
QgsConnectionParameters QgsConnectionRegistry::connectionParameters(QString type, QString name)
{
   QSettings settings;
   QString baseKey;
   QgsConnectionParameters conn;
  
   qDebug("QgsConnectionManager::getConnectionDetails : type "+type); 
  
   baseKey="/DatabaseConnections/"+type+"/";

  
   baseKey +=name;
   conn.name=name;
   conn.type=type;
   conn.host=settings.readEntry(baseKey + "/host");
   conn.database=settings.readEntry(baseKey + "/database");
   conn.port=settings.readEntry(baseKey + "/port");
   conn.user=settings.readEntry(baseKey + "/username");
   conn.password=settings.readEntry(baseKey + "/password");
   conn.uri=settings.readEntry(baseKey + "/uri");
   conn.publicOnly=settings.readBoolEntry(baseKey + "/publicOnly");
   conn.geometryColumnsOnly=settings.readBoolEntry(baseKey + "/geometryColumnsOnly");
   conn.save=settings.readBoolEntry(baseKey + "/save");
   qDebug("QgsConnectionManager::getConnectionDetails : host "+conn.host); 
   return conn;  
}


/**
 * returns a list of connection names of the given type
 */
QStringList QgsConnectionRegistry::connectionParametersList(QString type)
{
  QSettings settings;
  QString baseKey;
  
  qDebug("QgsConnectionManager::getConnections : type "+type); 
  
  baseKey="/DatabaseConnections/"+type+"/";
  
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


/**
 * Returns the connection params selected of the given type
 */
QString QgsConnectionRegistry::selected(QString type){
  QSettings settings;
  QString result;
  QString basekey;
    
  basekey="/DatabaseConnections/"+type+"/selected";
  result=settings.readEntry(basekey);                                      
     
  return result;
}


/**
 * Returns the selected connection type
 */
QString QgsConnectionRegistry::selectedType(){
  QSettings settings;
  QString result;

  result=settings.readEntry("/DatabaseConnections/SelectedConnectionType/selected");                                      
  return result; 
}


/**
 * Removes connection params
 */
bool QgsConnectionRegistry::removeConnectionParameters(QString type, QString name)
{
   QSettings settings; 
   QString baseKey;
   
   baseKey="/DatabaseConnections/"+type+"/";
   
     
   qDebug("QgsConnectionManager::removeConnection baseKey : "+baseKey);                           
   qDebug("QgsConnectionManager::removeConnection Connection : "+name);
   
   baseKey += name; 
   settings.removeEntry(baseKey + "/host");
   settings.removeEntry(baseKey + "/database");
   settings.removeEntry(baseKey + "/username");
   settings.removeEntry(baseKey + "/password");
   settings.removeEntry(baseKey + "/uri");
   settings.removeEntry(baseKey + "/port");
   settings.removeEntry(baseKey + "/save");
   settings.removeEntry(baseKey + "/publicOnly");
   settings.removeEntry(baseKey + "/geometryColumnsOnly");
   settings.removeEntry(baseKey);
     
   return true;
}


/**
 * Modifies the params of a certain connection
 */
bool QgsConnectionRegistry::modifyConnectionParameters(QgsConnectionParameters conn){

	return false;
}


bool QgsConnectionRegistry::saveConnection(QgsConnectionParameters conn)
{
     QSettings settings; 
     QString baseKey;
   
   baseKey="/DatabaseConnections/"+conn.type+"/";

   qDebug("baseKey : "+baseKey);                           
   qDebug("Connection : "+conn.name);
   settings.writeEntry(baseKey + "selected", conn.selected);
   baseKey += conn.name;
   settings.writeEntry(baseKey + "/host", conn.host);
   settings.writeEntry(baseKey + "/database", conn.database);
   settings.writeEntry(baseKey + "/port", conn.port);
   settings.writeEntry(baseKey + "/username", conn.user);
   settings.writeEntry(baseKey + "/password", conn.password);
   settings.writeEntry(baseKey + "/uri", conn.uri);
   settings.writeEntry(baseKey + "/publicOnly", conn.publicOnly);
   settings.writeEntry(baseKey + "/geometryColumnsOnly", conn.geometryColumnsOnly);
   if (conn.save)
     {
      settings.writeEntry(baseKey + "/save", "true");
     } else
     {
      settings.writeEntry(baseKey + "/save", "false");
     }
      

   return true;
}


/**
 * Sets the connection params selected of a given type
 */
void QgsConnectionRegistry::setSelected(QString type, QString name){
   QSettings settings;
   QString baseKey;
    
   baseKey="/DatabaseConnections/"+type+"/";
   settings.writeEntry(baseKey+"selected",name);     

}


/**
 * Sets the type selected
 */
void QgsConnectionRegistry::setSelectedType(QString type){
   QSettings settings;
   QString baseKey;
    
   baseKey="/DatabaseConnections/SelectedConnectionType/";                                      
   settings.writeEntry(baseKey+"selected",type);    

}
