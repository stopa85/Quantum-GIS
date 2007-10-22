/***************************************************************************
    qgsurimaneger.h - Database URI connection manager
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

#ifndef qgsurimanager_H
#define qgsurimanager_H

#include <qstring.h>
#include <qstringlist.h>
#include "qgsconnection.h"

enum QgsOgrDatabase
     {ogrMySQL,
      ogrPostgreSQL,
      ogrOracle,
      ogrFile,
      ogrNone,
      PostgreSQL};


class CORE_EXPORT QgsURIManager
{
          
  public: 
   /**
    * Constructor of the uri manager
    * @param uri  uniform resource locator (URI) or database params string 
   */
    QgsURIManager(QString const & uri = "");
    
   /**
    * Alternative Constructor of the uri manager
    * @param conn  Connection params 
   */ 
    QgsURIManager(QgsConnection conn);
   /**
    * Destructor of uri manager
    */
   virtual ~QgsURIManager();


   
   void setType(QString const type);
   void setServer(QString const server);
   void setDatabase(QString const database);
   void setUser(QString const user);
   void setPassword(QString const password);
   void setLayer(QString const layer);
   void setPort(QString const port);
   
   QString getURI();
   QString getParamsString();
   QString getLayer();
   
   private:
       QString mURI;     
       QgsOgrDatabase mType;
       QString mServer;
       QString mDatabase;
       QString mUser;
       QString mPassword;
       QString mLayer;
       QString mPort;
               
};




#endif
