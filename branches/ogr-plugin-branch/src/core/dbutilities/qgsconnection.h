/***************************************************************************
qgsconnectionmanager.h  Struct to store connection params
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

#ifndef QGSCONNECTION_H
#define QGSCONNECTION_H
#include "qstring.h"

struct CORE_EXPORT QgsConnection
  {
    QString type;
    QString name;
    QString host;
    QString database;
    QString port;
    QString user;
    QString password;
    bool geometryColumnsOnly;
    bool publicOnly;
    QString selected;
    bool save;
  };
#endif
