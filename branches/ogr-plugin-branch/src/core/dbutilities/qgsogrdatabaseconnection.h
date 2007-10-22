/***************************************************************************
    qgsogrdatabaseconnection.h - Connection to ogr databases
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

#ifndef QGSOGRDATABASECONNECTION_H
#define QGSOGRDATABASECONNECTION_H

#include "qgsdatabaseconnectionbase.h"
#include "qgsurimanager.h"
#include "cpl_port.h"
#include <ogr_api.h>
#include <ogrsf_frmts.h>

/** Base class for spatial database connections
 */ 
class CORE_EXPORT QgsOgrDatabaseConnection : public QgsDatabaseConnectionBase { 
      
public: 
  QgsOgrDatabaseConnection(QgsConnection conn); 
  ~QgsOgrDatabaseConnection(); 
  bool connect(); 
  QStringList getGeometryTables();
  QString getTableGeometry(QString tableName);
};

#endif
