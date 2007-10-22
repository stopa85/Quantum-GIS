/***************************************************************************
    qgsdatabaseconnection.h - Spatial Database connection interface
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

#include "qgsdatabaseconnectionbase.h"


QgsDatabaseConnectionBase::QgsDatabaseConnectionBase(QgsConnection conn)
 {
   mConnection=conn;                                                                  
 }
QgsDatabaseConnectionBase::~QgsDatabaseConnectionBase()
 {
 } 

bool QgsDatabaseConnectionBase::connect()
 {
  return true;
 }
 
QString QgsDatabaseConnectionBase::getError()
 {
  return mError;
 } 

QStringList QgsDatabaseConnectionBase::getGeometryTables()
 {
 }

QString QgsDatabaseConnectionBase::getTableGeometry(QString tableName)
 {
 }
