/***************************************************************************
                              qgsconnectionmanager.h  
                              -------------------
  begin                : October 2, 2007
  copyright            : (C) 2007 by Godofredo Contreras
  email                : frdcn at hotmail dot com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QGSCONNECTIONMANAGER_H
#define QGSCONNECTIONMANAGER_H

#include "qstringlist.h"
#include "qsettings.h"
#include "qgsdatabaseconnectionbase.h"
#include "qgspostgresdatabaseconnection.h"
#include "qgsogrdatabaseconnection.h"




class CORE_EXPORT QgsConnectionManager
{
      QgsDatabaseConnectionBase* mConnection;
      QString mError;
	public:
        QgsConnectionManager();     
        ~QgsConnectionManager();     
        bool saveConnection(QgsConnection conn);
        bool removeConnection(QString type,QString name);
        bool modifyConnection(QgsConnection conn); 
        QStringList getConnections(QString type); 
        QgsConnection getConnectionDetails(QString type, QString name);
        QgsDatabaseConnectionBase* getConnection();  
    	bool connect(QString type, QgsConnection conn);
        void setSelected(QString type, QString name);
        QString getSelected(QString type);
    	QString getError();
};

#endif 
