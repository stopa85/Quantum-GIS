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

 /* $Id$ */

#include "qgspostgresdatabaseconnection.h"
#include "qgsdatasourceuri.h"
#include "qstring.h"


QgsPostgresDatabaseConnection::QgsPostgresDatabaseConnection(){

}





QgsPostgresDatabaseConnection::QgsPostgresDatabaseConnection(QgsConnectionParameters* conn)
    : QgsDatabaseConnection(conn){

}


QgsPostgresDatabaseConnection::~QgsPostgresDatabaseConnection(){
  // free pg connection resources
  PQfinish(pd);                                                                
}


bool QgsPostgresDatabaseConnection::connect(){
  // Need to escape the password to allow for single quotes and backslashes
  qDebug("QgsPostgresDatabaseConnection::connect");
  bool result=false;
  QString pass = mConnectionParameters->password;
  pass.replace('\\', "\\\\");
  pass.replace('\'', "\\'");

  QString connInfo =
    "host=" + mConnectionParameters->host + 
    " dbname=" + mConnectionParameters->database + 
    " port=" + mConnectionParameters->port +
    " user=" + mConnectionParameters->user + 
    " password='" + pass + "'";
  PGconn *pd = PQconnectdb(connInfo.toLocal8Bit().data());
  //  std::cout << pd->ErrorMessage();
  if (PQstatus(pd) == CONNECTION_OK)
    {
      // Database successfully opened; we can now issue SQL commands.
      PQsetClientEncoding(pd, "UNICODE");
      result=true;
  } else
    {
      mError=QObject::tr("Connection failed - Check settings and try again.\n\nExtended error information:\n");
      qDebug (QString(PQerrorMessage(pd)));
      mError=mError + QString(PQerrorMessage(pd));
      qDebug (mError);
      result=false;
    }
  
   
  return result;     
}


QString QgsPostgresDatabaseConnection::fullDescription(QString schema, QString table, QString column){

	return  NULL;
}


QString QgsPostgresDatabaseConnection::baseKey(){

	return  NULL;
}


bool QgsPostgresDatabaseConnection::geometryColumnInformation(PGconn* pg, GeometryColumns& details, bool searchGeometryColumnsOnly, bool searchPublicOnly){
{
  bool ok = false;


  QString sql = "select * from geometry_columns";
  // where f_table_schema ='" + settings.readEntry(key + "/database") + "'";
  sql += " order by f_table_schema,f_table_name";
  //qDebug("Fetching tables using: " + sql);
  PGresult *result = PQexec(pg, sql.toLocal8Bit().data());
  if (result)
  {
    QString msg;
    msg=msg+"Fetched "+(QString)PQntuples(result)+" tables from database";
    //QTextOStream(&msg) << "Fetched " << PQntuples(result) << " tables from database";
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
        QString column = PQgetvalue(result, idx, PQfnumber(result, "f_geometry_column"));
        QString type = PQgetvalue(result, idx, PQfnumber(result, "type"));
        
	QString as = "";
	if(type=="GEOMETRY" && !searchGeometryColumnsOnly) {
	  //fredaddSearchGeometryColumn(schemaName, tableName,  column);
	  as=type="WAITING";
	}

	//freddetails.push_back(geomPair(fullDescription(schemaName, tableName, column, as), type));
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

    //addSearchGeometryColumn(schema, table, column);
    //details.push_back(GeometryPair(fullDescription(schema, table, column, "WAITING"), "WAITING"));
  }
  ok = true;

  PQclear(result);

  return ok;
}
}


GeometryColumns QgsPostgresDatabaseConnection::geometryTables(bool searchGeometryColumnsOnly, bool searchPublicSchemaOnly){
  GeometryColumns details;          
  if(geometryColumnInformation(pd,details, searchGeometryColumnsOnly, searchPublicSchemaOnly))	
      {
       //check the thread
      }
  else
    {
      qDebug("Unable to get list of spatially enabled tables from the database");
      qDebug(PQerrorMessage(pd));
    }                  
  return  details;
}


QString QgsPostgresDatabaseConnection::tableGeometry(QString tableName){

	return  NULL;
}


QString QgsPostgresDatabaseConnection::makeGeometryQuery(QString schema, QString table, QString column){

	return  NULL;
}

//******************************************

/*bool QgsPostgresDatabaseConnection::geometryTableInformation(PGconn *pg, bool searchGeometryColumnsOnly, bool searchPublicOnly)
{
  bool ok = false;
  
  QString sql = "select * from geometry_columns";
  sql += " order by f_table_schema,f_table_name";

  PGresult *result = PQexec(pg, sql.toLocal8Bit().data());
  if (result)
  {
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
        QString column = PQgetvalue(result, idx, PQfnumber(result, "f_geometry_column"));
        QString type = PQgetvalue(result, idx, PQfnumber(result, "type"));
        
	QString as = "";
	if(type=="GEOMETRY" && !searchGeometryColumnsOnly) 
	  {
	    addSearchGeometryColumn(schemaName, tableName,  column);
	    as=type="WAITING";
	  }

	mTableModel.addTableEntry(type, schemaName, tableName, column, "");
      }
      PQclear(exists);
    }
    ok = true;
  }
  PQclear(result);

  //search for geometry columns in tables that are not in the geometry_columns metatable
  

  QApplication::restoreOverrideCursor();
  if (searchGeometryColumnsOnly)
    {
      return ok;
    }

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

    addSearchGeometryColumn(schema, table, column);
    //details.push_back(geomPair(fullDescription(schema, table, column, "WAITING"), "WAITING"));
    mTableModel.addTableEntry("Waiting", schema, table, column, "");
  }
  ok = true;

  PQclear(result);
  return ok;
}

*/
