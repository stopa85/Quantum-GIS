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

void QgsGeomColumnTypeThread::setDatabaseConnection(QgsDatabaseConnection *databaseConnection)
{
  mDatabaseConnection=databaseConnection;   
}

void QgsGeomColumnTypeThread::addGeometryColumn(QString schema, QString table, QString column)
{
  schemas.push_back(schema);
  tables.push_back(table);
  columns.push_back(column);
}

void QgsGeomColumnTypeThread::stop()
{
  mStopped=true;
}

void QgsGeomColumnTypeThread::getLayerTypes()
{
  mStopped=false;

  //PGconn *pd = PQconnectdb(mConnInfo.toLocal8Bit().data());
  //if (PQstatus(pd) == CONNECTION_OK)
  //{
  //  PQsetClientEncoding(pd, "UNICODE");

    for (uint i = 0; i<schemas.size(); i++)
    {
      //QString query = QgsDbSourceSelect::makeGeomQuery(schemas[i],
      //                                                 tables[i],
      //                                                 columns[i]);
      //PGresult* gresult = PQexec(pd, query.toLocal8Bit().data());
      qDebug("freddy1");
      //QString type;
      QString type=mDatabaseConnection->tableGeometryFromData(schemas[i], tables[i], columns[i]);
      qDebug("tipo : "+type);
      //if (PQresultStatus(gresult) == PGRES_TUPLES_OK) {
	  //QStringList types;

	  //for(int j=0; j<PQntuples(gresult); j++) {
	  //	QString type = PQgetvalue(gresult, j, 0);
	  //	if(type!="")
	  //	  types += type;
	  //}

	//type = types.join(",");
    //}
    //  PQclear(gresult);

      if(mStopped)
        break;

      // Now tell the layer list dialog box...
      emit setLayerType(schemas[i], tables[i], columns[i], type);
      qDebug("freddy2");
    }
  }

  //PQfinish(pd);


