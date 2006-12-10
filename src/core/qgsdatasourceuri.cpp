/***************************************************************************
      qgsdatasourceuri.h  -  Structure to contain the component parts
                             of a data source URI
                             -------------------
    begin                : Dec 5, 2004
    copyright            : (C) 2004 by Gary E.Sherman
    email                : sherman at mrcc.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* $Id: qgsdatasourceuri.h 5839 2006-09-19 18:04:21Z wonder $ */

#include "qgsdatasourceuri.h"

#include <QStringList>

QgsDataSourceURI::QgsDataSourceURI()
{
  // do nothing
}

QgsDataSourceURI::QgsDataSourceURI(QString uri)
{
  // URI looks like this:
  //  host=192.168.1.5 dbname=test port=5342 user=gsherman password=xxx table=tablename
  // (optionally at the end there might be: sql=..... )
  
  // TODO: improve parsing
  
  // Strip the table and sql statement name off and store them
  int sqlStart = uri.find(" sql");
  int tableStart = uri.find("table=");
  
  // set table name
  table = uri.mid(tableStart + 6, sqlStart - tableStart -6);

  // set sql where clause
  if(sqlStart > -1)
  { 
    sql = uri.mid(sqlStart + 5);
  }
  else
  {
    sql = QString::null;
  }
  
  // calculate the schema if specified
  schema = "";
  if (table.find(".") > -1) {
    schema = table.left(table.find("."));
  }
  geometryColumn = table.mid(table.find(" (") + 2);
  geometryColumn.truncate(geometryColumn.length() - 1);
  table = table.mid(table.find(".") + 1, table.find(" (") - (table.find(".") + 1)); 

  // set connection info
  connInfo = uri.left(uri.find("table="));
  
  // parse the connection info
  QStringList conParts = QStringList::split(" ", connInfo);
  QStringList parm = QStringList::split("=", conParts[0]);
  if(parm.size() == 2)
  {
    host = parm[1];
  }
  parm = QStringList::split("=", conParts[1]);
  if(parm.size() == 2)
  {
    database = parm[1];
  }
  parm = QStringList::split("=", conParts[2]);
  if(parm.size() == 2)
  {
    port = parm[1];
  }

  parm = QStringList::split("=", conParts[3]);
  if(parm.size() == 2)
  {
    username = parm[1];
  }
  
  // The password can have '=' and ' ' characters in it, so we can't 
  // use the split on '=' and ' ' technique - use indexOf() 
  // instead. 
  QString key="password='"; 
  int i = connInfo.indexOf(key); 
  if (i != -1) 
  { 
    QString pass = connInfo.mid(i+key.length()); 
    // Now walk through the string till we find a ' character, but 
    // need to allow for an escaped ' character (which will be the 
    // \' character pair). 
    int n = 0; 
    bool escaped = false; 
    while (n < pass.length() && (pass[n] != '\'' || escaped)) 
    { 
      if (pass[n] == '\\') 
        escaped = true; 
      else 
        escaped = false; 
      n++; 
    } 
    // The -1 is to remove the trailing ' character 
    password = pass.left(n-1); 
  } 
}


QString QgsDataSourceURI::text() const
{
  return QString("host=" + host + 
      " dbname=" + database + 
      " port=" + port + 
      " user=" + username + 
      " password='" + password + 
      "' table=" + schema + '.' + table + 
      " (" + geometryColumn + ")" +
      " sql=" + sql);
}
