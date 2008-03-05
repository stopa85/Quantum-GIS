/***************************************************************************
                             qgsgeomcolumntypethread.h  
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


#ifndef QGSGEOMCOLUMNTYPETHREAD_H
#define QGSGEOMCOLUMNTYPETHREAD_H

#include "qthread.h"
#include "qstring.h"
#include <vector>

#include "qgsdatabaseconnection.h"
extern "C"
{
#include <libpq-fe.h>
}
// Perhaps this class should be in its own file??
//
// A class that determines the geometry type of a given database 
// schema.table.column, with the option of doing so in a separate
// thread.

class QgsGeomColumnTypeThread : public QThread
{
  Q_OBJECT
 public:

  void setConnInfo(QString s);
  void setDatabaseConnection(QgsDatabaseConnection *databaseConnection);
  void addGeometryColumn(QString schema, QString table, QString column);

  // These functions get the layer types and pass that information out
  // by emitting the setLayerType() signal. The getLayerTypes()
  // function does the actual work, but use the run() function if you
  // want the work to be done as a separate thread from the calling
  // process. 
  virtual void run() { getLayerTypes(); }
  void getLayerTypes();

  signals:
         void setLayerType(QString schema, QString table, QString column,
                    QString type);
  public slots:
         void stop();                  

 private:
  QString mConnInfo;
  QgsDatabaseConnection *mDatabaseConnection;
  bool mStopped;
  std::vector<QString> schemas, tables, columns;
};
#endif
