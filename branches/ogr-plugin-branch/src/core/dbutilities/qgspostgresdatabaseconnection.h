/***************************************************************************
    qgspostgresdatabaseconnection.h - Connection Class for Postgres
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

#ifndef QGSDATABASECONNECTION_H
#define QGSDATABASECONNECTION_H

#include "qgsdatabaseconnectionbase.h"
#include "qobject.h"
#include "qgsgeomcolumntypethread.h"

extern "C"
{
#include <libpq-fe.h>
}

/** Base class for spatial database connections
 */ 
class CORE_EXPORT QgsPostgresDatabaseConnection : public QgsDatabaseConnectionBase { 
      
public: 
  QgsPostgresDatabaseConnection(QgsConnection conn); 
  ~QgsPostgresDatabaseConnection(); 
  bool connect(); 
  QStringList getGeometryTables(); 
  QString getTableGeometry(QString tableName); 
private:
  QgsGeomColumnTypeThread* mColumnTypeThread;      
  typedef std::pair<QString, QString> geomPair;
  typedef std::list<geomPair > geomCol;        
  bool getGeometryColumnInfo(PGconn *pg, 
                geomCol& details, bool searchGeometryColumnsOnly,
                                              bool searchPublicOnly);
  // Combine the schema, table and column data into a single string
  // useful for display to the user
  QString fullDescription(QString schema, QString table, QString column);                                            
  
  // Utility function to construct the query for finding out the
  // geometry type of a column
  static QString makeGeomQuery(QString schema, QString table, QString column);
};

#endif
