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

#ifndef QGSDATABASECONNECTIONBASE_H
#define QGSDATABASECONNECTIONBASE_H

#include "qgsconnection.h"
#include "qstringlist.h"
/** Base class for spatial database connections
 */ 
class CORE_EXPORT QgsDatabaseConnectionBase { 
  
protected:  
  QString mError;
  QgsConnection mConnection;    
public: 
  QgsDatabaseConnectionBase(QgsConnection conn);
  virtual ~QgsDatabaseConnectionBase();      
  virtual bool connect();
  QString getError();
  virtual QStringList getGeometryTables();
  virtual QString getTableGeometry(QString tableName);
};

#endif
