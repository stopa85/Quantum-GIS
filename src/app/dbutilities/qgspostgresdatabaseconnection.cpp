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
  PQfinish(pd);                                                                
}


bool QgsPostgresDatabaseConnection::connect(){
  // Need to escape the password to allow for single quotes and backslashes
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
  pd = PQconnectdb(connInfo.toLocal8Bit().data());  
  if (PQstatus(pd) == CONNECTION_OK)
    {
      PQsetClientEncoding(pd, "UNICODE");
      result=true;
    }
  else
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
  sql += " order by f_table_schema,f_table_name";
  PGresult *result = PQexec(pg, sql.toLocal8Bit().data());
  if (result)
  {
    QString msg;
    msg=msg+"Fetched "+(QString)PQntuples(result)+" tables from database";
    for (int idx = 0; idx < PQntuples(result); idx++)
    {
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
	    if(type=="GEOMETRY" && !searchGeometryColumnsOnly){
	     as=type="WAITING";
        }
      }
      PQclear(exists);
    }
    ok = true;
  }
  PQclear(result);
  if (searchGeometryColumnsOnly)
    return ok;

  // Now have a look for geometry columns that aren't in the
  // geometry_columns table.
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
    // Make the assumption that the geometry type for the first
    // row is the same as for all other rows. 
    QString table  = PQgetvalue(result, i, 0); // relname
    QString schema = PQgetvalue(result, i, 1); // nspname
    QString column = PQgetvalue(result, i, 2); // attname
    QString relkind = PQgetvalue(result, i, 3); // relation kind
  }
  ok = true;

  PQclear(result);

  return ok;
}
}

QList<QgsGeometryColumnDescription *> QgsPostgresDatabaseConnection::geometryTables(){
  
  bool searchGeometryColumnsOnly=mConnectionParameters->geometryColumnsOnly;
  bool searchPublicSchemaOnly=mConnectionParameters->publicOnly;
  
  QList<QgsGeometryColumnDescription *> details;  
  
   
  QString sql = "select * from geometry_columns";
  sql += " order by f_table_schema,f_table_name";

  PGresult *result = PQexec(pd, sql.toLocal8Bit().data());
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

      PGresult* exists = PQexec(pd, sql.toLocal8Bit().data());
      if (PQntuples(exists) == 1)
      {
        QString column = PQgetvalue(result, idx, PQfnumber(result, "f_geometry_column"));
        QString type = PQgetvalue(result, idx, PQfnumber(result, "type"));
	    QString as = "";
	    if(type=="GEOMETRY" && !searchGeometryColumnsOnly) 
	     {
           as=type="WAITING";
	     }
       
        details.append(new QgsGeometryColumnDescription(type,schemaName,tableName,column));
        
	    
      }
      PQclear(exists);
    }
    //ok = true;
  }
  PQclear(result);

  //search for geometry columns in tables that are not in the geometry_columns metatable
  

  if (searchGeometryColumnsOnly)
    {
      return details;
    }

   // Now have a look for geometry columns that aren't in the
  // geometry_columns table. This code is specific to postgresql,
  // but an equivalent query should be possible in other
  // databases.
  sql = "select pg_class.relname, pg_namespace.nspname, pg_attribute.attname, "
    "pg_class.relkind from "
    "pg_attribute, pg_class, pg_type, pg_namespace where pg_type.typname = 'geometry' and "
    "pg_attribute.atttypid = pg_type.oid and pg_attribute.attrelid = pg_class.oid ";

  if (searchPublicSchemaOnly)
    sql += "and pg_namespace.nspname = 'public' ";

  sql += "and cast(pg_class.relname as character varying) not in "
    "(select f_table_name from geometry_columns) "
    "and pg_namespace.oid = pg_class.relnamespace "
    "and pg_class.relkind in ('v', 'r')"; // only from views and relations (tables)
  
  result = PQexec(pd, sql.toLocal8Bit().data());

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

    
    details.append(new QgsGeometryColumnDescription("Waiting",schema,table,column));
  }
  
  PQclear(result);
 
  return  details;
}


QString QgsPostgresDatabaseConnection::tableGeometry(QString tableName){

	return  NULL;
}

QString QgsPostgresDatabaseConnection::tableGeometryFromData(QString schema, QString tableName, QString column){
    qDebug("QgsPostgresDatabaseConnection::tableGeometryFromData");     
    
  QString query=QString("select distinct "
		  "case"
		  " when geometrytype(%1) IN ('POINT','MULTIPOINT') THEN 'POINT'"
		  " when geometrytype(%1) IN ('LINESTRING','MULTILINESTRING') THEN 'LINESTRING'"
		  " when geometrytype(%1) IN ('POLYGON','MULTIPOLYGON') THEN 'POLYGON'"
		  " end "
		  "from \"%2\".\"%3\"").arg(column).arg(schema).arg(tableName);  
  
  QString type;	  
  QStringList types;
  qDebug(query); 
  if (PQstatus(pd) == CONNECTION_OK)
    {
      qDebug("Connection ok");              
      PGresult* gresult = PQexec(pd, query.toLocal8Bit().data());
      if (PQresultStatus(gresult) == PGRES_TUPLES_OK) {
        qDebug("Tuples ok");                                        
     	for(int j=0; j<PQntuples(gresult); j++) {
  		   QString type = PQgetvalue(gresult, j, 0);
  		   if(type!="")
  		     types += type;
 	     }
       }
      type = types.join(",");
      PQclear(gresult);
    } 
  

  return type;    
}



