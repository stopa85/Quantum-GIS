/***************************************************************************
                    qgspatialreferences.cpp  -  Singleton class for
                    storing spatial reference systems
                             -------------------
    begin                : 2005-01-22
    copyright            : (C) 2005 by Gary E.Sherman
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
 /* $Id$ */

#include <iostream>
#include <qstring.h>
#include <qdir.h>
#include "qgsspatialreferences.h"

QgsSpatialReferences *QgsSpatialReferences::_instance = 0;
QgsSpatialReferences *QgsSpatialReferences::instance()
{
  if (_instance == 0)
  {
    _instance = new QgsSpatialReferences();
  }
  return _instance;
}

QgsSpatialReferences::QgsSpatialReferences()
{
  // read the spatial reference systems from disk and populate the map
#if defined(Q_OS_MACX) || defined(WIN32)
  QString PKGDATAPATH = qApp->applicationDirPath() + "/share/qgis";
#endif
  QString theFileNameQString = PKGDATAPATH;
  theFileNameQString += "/resources/spatial_ref_sys.txt";


  QFile myQFile( theFileNameQString );
  if ( myQFile.open( IO_ReadOnly ) ) 
  {
    QTextStream myQTextStream( &myQFile );
    QString myCurrentLineQString;


    // Read the QGIS-supplied CS file which is actually taken from PostGIS spatial_ref_sys
    // table. The schema for this table:
    //         Table "public.spatial_ref_sys"
    //    Column   |          Type           | Modifiers
    //  -----------+-------------------------+-----------
    // 0 srid      | integer                 | not null
    // 1 auth_name | character varying(256)  |
    // 2 auth_srid | integer                 |
    // 3 srtext    | character varying(2048) |
    // 4 proj4text | character varying(2048) |
    int wktCount = 0;
    while ( !myQTextStream.atEnd() ) 
    {
      myCurrentLineQString = myQTextStream.readLine(); // line of text excluding '\n'
#ifdef QGISDEBUG
      //generates a lot of output to stdout!
      //std::cout << " Match found:" << myCurrentLineQString.ascii() << std::endl;
#endif

      QStringList wktParts = QStringList::split(QRegExp("\t"), myCurrentLineQString, true); 
      // store the parts in a SPATIAL_REF_SYS structure
      mSpatialReferences[wktParts[0]] = new QgsSpatialRefSys(
          wktParts[0],
          wktParts[1],
          wktParts[2],
          wktParts[3],
          wktParts[4]);
    }
  }
}
 QgsSpatialRefSys * QgsSpatialReferences::getSrsBySrid(QString srid)
{
  return mSpatialReferences[srid];
}

 QgsSpatialRefSys * QgsSpatialReferences::getSrsByWkt(QString &wkt)
{
  return 0;
}

ProjectionWKTMap QgsSpatialReferences::getMap()
{
  return mSpatialReferences;
}
