/***************************************************************************
  qgswfsprovider.cpp  -  QGIS Data provider for 
                         OGC Web Feature Service layers
                             -------------------
    begin                : 17 Mar, 2006
    copyright            : (C) 2006 by Brendan Morley
    email                : morb at beagle dot com dot au
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

#include "qgswfsprovider.h"

#include <fstream>
#include <iostream>

#ifdef WIN32
#define QGISEXTERN extern "C" __declspec( dllexport )
#else
#define QGISEXTERN extern "C"
#endif


static QString WFS_KEY = "wfs";
static QString WFS_DESCRIPTION = "OGC Web Feature Service version 1.0 data provider";

static QString DEFAULT_LATLON_CRS = "CRS:84";


QgsWfsProvider::QgsWfsProvider(QString const & uri)
//  : QgsVectorDataProvider(uri)
{
#ifdef QGISDEBUG
  std::cout << "QgsWfsProvider: constructing with uri '" << uri.toLocal8Bit().data() << "'." << std::endl;
#endif
  // assume this is a valid layer until we determine otherwise
  valid = true;

  // URI is in form: URL[ proxyhost[ [proxyport]]

  // Split proxy from the provider-encoded uri  
  QStringList drawuriparts = QStringList::split(" ", httpuri, TRUE);

  baseUrl = drawuriparts.front();
  drawuriparts.pop_front();

  if (drawuriparts.count())
  {
    httpproxyhost = drawuriparts.front();
    drawuriparts.pop_front();

    if (drawuriparts.count())
    {
      bool ushortConversionOK;
      httpproxyport = drawuriparts.front().toUShort(&ushortConversionOK);
      drawuriparts.pop_front();

      if (!ushortConversionOK)
      {
        httpproxyport = 80;  // standard HTTP port
      }
    }
    else
    {
      httpproxyport = 80;  // standard HTTP port
    }
  }

  // URL can be in 3 forms:
  // 1) http://xxx.xxx.xx/yyy/yyy
  // 2) http://xxx.xxx.xx/yyy/yyy?
  // 3) http://xxx.xxx.xx/yyy/yyy?zzz=www
  
  // Prepare the URI so that we can later simply append param=value
  if ( !(baseUrl.contains("?")) ) 
  {  
    baseUrl.append("?");
  }
  else if ( baseUrl.right(1) != "?" ) 
  {
    baseUrl.append("&");
  }

#ifdef QGISDEBUG
  std::cout << "QgsWfsProvider: baseUrl = " << baseUrl.toLocal8Bit().data() << std::endl;
#endif


#ifdef QGISDEBUG
  std::cout << "QgsWfsProvider: exiting constructor." << std::endl;
#endif

}

QgsWfsProvider::~QgsWfsProvider()
{

#ifdef QGISDEBUG
  std::cout << "QgsWfsProvider: deconstructing." << std::endl;
#endif

}


QString QgsWfsProvider::errorCaptionString()
{
  return mErrorCaption;
}


QString QgsWfsProvider::errorString()
{
#ifdef QGISDEBUG
  std::cout << "QgsWfsProvider::errorString: returning '" 
            << mError.toLocal8Bit().data() << "'." << std::endl;
#endif
  return mError;
}


QString  QgsWfsProvider::name() const
{
    return WFS_KEY;
} //  QgsWfsProvider::name()



QString  QgsWfsProvider::description() const
{
    return WFS_DESCRIPTION;
} //  QgsWfsProvider::description()




   
/**
 * Class factory to return a pointer to a newly created 
 * QgsWmsProvider object
 */
QGISEXTERN QgsWfsProvider * classFactory(const QString *uri)
{
  return new QgsWfsProvider(*uri);
}
/** Required key function (used to map the plugin to a data store type)
*/
QGISEXTERN QString providerKey(){
  return WFS_KEY;
}
/**
 * Required description function 
 */
QGISEXTERN QString description()
{
    return WFS_DESCRIPTION;
} 
/**
 * Required isProvider function. Used to determine if this shared library
 * is a data provider plugin
 */
QGISEXTERN bool isProvider(){
  return true;
}

