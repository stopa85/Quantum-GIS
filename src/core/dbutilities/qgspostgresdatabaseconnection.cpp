/***************************************************************************
    qgspostgresdatabaseconnection.cpp - Connection Class for Postgres
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

#include "qgspostgresdatabaseconnection.h"


QgsPostgresDatabaseConnection::QgsPostgresDatabaseConnection(QgsConnection conn)
:QgsDatabaseConnectionBase(conn)
 {
 }
  
QgsPostgresDatabaseConnection::~QgsPostgresDatabaseConnection()
 {
 }
 
 
bool QgsPostgresDatabaseConnection::connect()
 {
  
  // Need to escape the password to allow for single quotes and backslashes
  qDebug("QgsPostgresDatabaseConnection::connect");
  bool result=false;
  QString password = mConnection.password;
  password.replace('\\', "\\\\");
  password.replace('\'', "\\'");

  QString connInfo =
    "host=" + mConnection.host + 
    " dbname=" + mConnection.database + 
    " port=" + mConnection.port +
    " user=" + mConnection.user + 
    " password='" + password + "'";
  PGconn *pd = PQconnectdb(connInfo.toLocal8Bit().data());
  //  std::cout << pd->ErrorMessage();
  if (PQstatus(pd) == CONNECTION_OK)
    {
      // Database successfully opened; we can now issue SQL commands.
      result=true;
  } else
    {
      mError=QObject::tr("Connection failed - Check settings and try again.\n\nExtended error information:\n");
      qDebug (QString(PQerrorMessage(pd)));
      mError=mError + QString(PQerrorMessage(pd));
      qDebug (mError);
      result=false;
    }
  // free pg connection resources
  PQfinish(pd);
   
  return result;     
 }


QStringList QgsPostgresDatabaseConnection::getGeometryTables() 
{
   // Need to escape the password to allow for single quotes and backslashes
  qDebug("QgsPostgresDatabaseConnection::connect");
  QStringList result;
  QString password = mConnection.password;
  password.replace('\\', "\\\\");
  password.replace('\'', "\\'");

  QString connInfo =
    "host=" + mConnection.host + 
    " dbname=" + mConnection.database + 
    " port=" + mConnection.port +
    " user=" + mConnection.user + 
    " password='" + password + "'";
  PGconn *pd = PQconnectdb(connInfo.toLocal8Bit().data());
  //  std::cout << pd->ErrorMessage();
  if (PQstatus(pd) == CONNECTION_OK)
    {
      // Database successfully opened; we can now issue SQL commands.
      // tell the DB that we want text encoded in UTF8
      PQsetClientEncoding(pd, "UNICODE");
      geomCol details;
      if (getGeometryColumnInfo(pd, details, mConnection.geometryColumnsOnly,
                                  mConnection.publicOnly))
      {
        details.sort();
        geomCol::const_iterator iter = details.begin();
        for (; iter != details.end(); ++iter)
        {
            result<<iter->first;
        }    
      }
  } else
  {
      mError=QObject::tr("Connection failed - Check settings and try again.\n\nExtended error information:\n");
      qDebug (QString(PQerrorMessage(pd)));
      mError=mError + QString(PQerrorMessage(pd));
      qDebug (mError);
  }
  // free pg connection resources
 PQfinish(pd);
 return result;            
} 


QString QgsPostgresDatabaseConnection::getTableGeometry(QString tableName) 
{
   // Need to escape the password to allow for single quotes and backslashes
  qDebug("QgsPostgresDatabaseConnection::connect");
  QString result;
  QString password = mConnection.password;
  password.replace('\\', "\\\\");
  password.replace('\'', "\\'");

  QString connInfo =
    "host=" + mConnection.host + 
    " dbname=" + mConnection.database + 
    " port=" + mConnection.port +
    " user=" + mConnection.user + 
    " password='" + password + "'";
  PGconn *pd = PQconnectdb(connInfo.toLocal8Bit().data());
  //  std::cout << pd->ErrorMessage();
  if (PQstatus(pd) == CONNECTION_OK)
    {
      // Database successfully opened; we can now issue SQL commands.
      // tell the DB that we want text encoded in UTF8
      PQsetClientEncoding(pd, "UNICODE");
      geomCol details;
      if (getGeometryColumnInfo(pd, details, mConnection.geometryColumnsOnly,
                                  mConnection.publicOnly))
      {
        details.sort();
        geomCol::const_iterator iter = details.begin();
        for (; iter != details.end(); ++iter)
         {
            if (iter->first==tableName)
              result=iter->second;
            break;  
         }    
      }
  } else
  {
      mError=QObject::tr("Connection failed - Check settings and try again.\n\nExtended error information:\n");
      qDebug (QString(PQerrorMessage(pd)));
      mError=mError + QString(PQerrorMessage(pd));
      qDebug (mError);
  }
  // free pg connection resources
 PQfinish(pd);
 return result;            
} 

bool QgsPostgresDatabaseConnection::getGeometryColumnInfo(PGconn *pg, 
                geomCol& details, bool searchGeometryColumnsOnly,
                                              bool searchPublicOnly)
{
                                              
  //QString res;
  QString password = mConnection.password;
  password.replace('\\', "\\\\");
  password.replace('\'', "\\'");

  QString connInfo =
    "host=" + mConnection.host + 
    " dbname=" + mConnection.database + 
    " port=" + mConnection.port +
    " user=" + mConnection.user + 
    " password='" + password + "'";
  PGconn *pd = PQconnectdb(connInfo.toLocal8Bit().data());
                                              
  bool ok = false;

  QString sql = "select * from geometry_columns";
  // where f_table_schema ='" + settings.readEntry(key + "/database") + "'";
  sql += " order by f_table_schema,f_table_name";
  //qDebug("Fetching tables using: " + sql);
  PGresult *result = PQexec(pg, sql.toLocal8Bit().data());
  if (result)
  {
    QString msg;
    qDebug( "Fetched " +QString(PQntuples(result))+ " tables from database");
    //qDebug(msg);
    for (int idx = 0; idx < PQntuples(result); idx++)
    {
      // Be a bit paranoid and check that the table actually
      // exists. This is not done as a subquery in the query above
      // because I can't get it to work correctly when there are tables
      // with capital letters in the name.

      // Take care to deal with tables with the same name but in different schema.
      QString tableName = PQgetvalue(result, idx, PQfnumber(result, "f_table_name"));
      QString schemaName = PQgetvalue(result, idx, PQfnumber(result, "f_table_schema"));
      sql = "select oid from pg_class where relname = '" + tableName + "'";
      if (schemaName.length() > 0)
	sql +=" and relnamespace = (select oid from pg_namespace where nspname = '" +
	  schemaName + "')";

      PGresult* exists = PQexec(pg, sql.toLocal8Bit().data());
      if (PQntuples(exists) == 1)
      {
        QString v = "";

        if (schemaName.length() > 0)
        {
          v += '"';
          v += schemaName;
          v += "\".";
        }

        v += '"';
        v += tableName;
        v += "\" (";
        v += PQgetvalue(result, idx, PQfnumber(result, "f_geometry_column"));
        v += ")";

        QString type = PQgetvalue(result, idx, PQfnumber(result, "type"));
        details.push_back(geomPair(v, type));
      }
      PQclear(exists);
    }
    ok = true;
  }
  PQclear(result);

  if (searchGeometryColumnsOnly)
    return ok;

  // Now have a look for geometry columns that aren't in the
  // geometry_columns table. This code is specific to postgresql,
  // but an equivalent query should be possible in other
  // databases.
  sql = "select pg_class.relname, pg_namespace.nspname, pg_attribute.attname, "
    "pg_class.relkind from "
    "pg_attribute, pg_class, pg_type, pg_namespace where pg_type.typname = 'geometry' and "
    "pg_attribute.atttypid = pg_type.oid and pg_attribute.attrelid = pg_class.oid ";

  if (searchPublicOnly)
    sql += "and pg_namespace.nspname = 'public' ";

  sql += "and cast(pg_class.relname as character varying) not in "
    "(select f_table_name from geometry_columns) "
    "and pg_namespace.oid = pg_class.relnamespace "
    "and pg_class.relkind in ('v', 'r')"; // only from views and relations (tables)
  
  result = PQexec(pg, sql.toLocal8Bit().data());

  for (int i = 0; i < PQntuples(result); i++)
  {
    // Have the column name, schema name and the table name. The concept of a
    // catalog doesn't exist in postgresql so we ignore that, but we
    // do need to get the geometry type.

    // Make the assumption that the geometry type for the first
    // row is the same as for all other rows. 

    QString table  = PQgetvalue(result, i, 0); // relname
    QString schema = PQgetvalue(result, i, 1); // nspname
    QString column = PQgetvalue(result, i, 2); // attname
    QString relkind = PQgetvalue(result, i, 3); // relation kind

    QString full_desc = fullDescription(schema, table, column);

    QString type = "UNKNOWN";
    if (relkind == "r")
    {
      QString query = makeGeomQuery(schema, table, column);
      PGresult* gresult = PQexec(pg, query.toLocal8Bit().data());
      if (PQresultStatus(gresult) != PGRES_TUPLES_OK)
        {
          QString myError = (QObject::tr("Access to relation ") + table + QObject::tr(" using sql;\n") + query +
                             QObject::tr("\nhas failed. The database said:\n"));
          qDebug(myError + QString(PQresultErrorMessage(gresult)));
        }
      else if (PQntuples(gresult) > 0)
        type = PQgetvalue(gresult, 0, 0); // GeometryType
      PQclear(gresult);
    }
    else // view
    {
      // store the column details and do the query in a thread
      if (mColumnTypeThread == NULL)
      {
        mColumnTypeThread = new QgsGeomColumnTypeThread();
        mColumnTypeThread->setConnInfo(connInfo);
      }
      mColumnTypeThread->setGeometryColumn(schema, table, column);
      type = "WAITING";
    }

    details.push_back(geomPair(full_desc, type));
  }
  ok = true;

  PQclear(result);

  return ok;
}

QString QgsPostgresDatabaseConnection::fullDescription(QString schema, QString table, 
                                           QString column)
{
  QString full_desc = "";
  if (schema.length() > 0)
    full_desc = '"' + schema + "\".\"";
  full_desc += table + "\" (" + column + ")";
  return full_desc;
}

QString QgsPostgresDatabaseConnection::makeGeomQuery(QString schema, 
                                                QString table, QString column)
{
  QString query = "select GeometryType(\"" + column + "\") from ";
  if (schema.length() > 0)
    query += "\"" + schema + "\".";
  query += "\"" + table + "\" where \"" + column + "\" is not null limit 1";
  return query;
}


