/***************************************************************************
                             qgsgeomcolumntypethread.cpp  
A class that determines the geometry type of a given database 
schema.table.column, with the option of doing so in a separate
thread.
                              -------------------
begin                : Sat Jun 22 2002
copyright            : (C) 2002 by Gary E.Sherman
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

#include "qgsgeomcolumntypethread.h" 

void QgsGeomColumnTypeThread::setConnInfo(QString s)
{
  mConnInfo = s;
}

void QgsGeomColumnTypeThread::setGeometryColumn(QString schema, QString table, QString column)
{
  schemas.push_back(schema);
  tables.push_back(table);
  columns.push_back(column);
}

void QgsGeomColumnTypeThread::getLayerTypes()
{
  PGconn *pd = PQconnectdb(mConnInfo.toLocal8Bit().data());
  if (PQstatus(pd) == CONNECTION_OK)
  {
    PQsetClientEncoding(pd, "UNICODE");

    for (uint i = 0; i < schemas.size(); ++i)
    {
      QString query = makeGeomQuery(schemas[i],
                                    tables[i],
                                    columns[i]);
      PGresult* gresult = PQexec(pd, query.toLocal8Bit().data());
      QString type;
      if (PQresultStatus(gresult) == PGRES_TUPLES_OK)
        type = PQgetvalue(gresult, 0, 0);
      PQclear(gresult);

      // Now tell the layer list dialog box...
      emit setLayerType(schemas[i], tables[i], columns[i], type);
    }
  }

  PQfinish(pd);
}

QString QgsGeomColumnTypeThread::makeGeomQuery(QString schema, 
                                                QString table, QString column)
{
  QString query = "select GeometryType(\"" + column + "\") from ";
  if (schema.length() > 0)
    query += "\"" + schema + "\".";
  query += "\"" + table + "\" where \"" + column + "\" is not null limit 1";
  return query;
}
