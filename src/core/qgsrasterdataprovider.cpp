/***************************************************************************
    qgsrasterdataprovider.cpp - DataProvider Interface for raster layers
     --------------------------------------
    Date                 : Mar 11, 2005
    Copyright            : (C) 2005 by Brendan Morley
    email                : morb at ozemail dot com dot au
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

#include "qgsrasterdataprovider.h"
#include "qgslogger.h"

#include <QMap>

QgsRasterDataProvider::QgsRasterDataProvider(): mDpi( -1 )
{
}

QgsRasterDataProvider::QgsRasterDataProvider( QString const & uri )
    : QgsDataProvider( uri )
    , mDpi( -1 )
{
}

//
//Random Static convenience function
//
/////////////////////////////////////////////////////////
//TODO: Change these to private function or make seprate class
// convenience function for building metadata() HTML table cells
// convenience function for creating a string list from a C style string list
QStringList QgsRasterDataProvider::cStringList2Q_( char ** stringList )
{
  QStringList strings;

  // presume null terminated string list
  for ( size_t i = 0; stringList[i]; ++i )
  {
    strings.append( stringList[i] );
  }

  return strings;

} // cStringList2Q_


QString QgsRasterDataProvider::makeTableCell( QString const & value )
{
  return "<p>\n" + value + "</p>\n";
} // makeTableCell_



// convenience function for building metadata() HTML table cells
QString QgsRasterDataProvider::makeTableCells( QStringList const & values )
{
  QString s( "<tr>" );

  for ( QStringList::const_iterator i = values.begin();
        i != values.end();
        ++i )
  {
    s += QgsRasterDataProvider::makeTableCell( *i );
  }

  s += "</tr>";

  return s;
} // makeTableCell_

QString QgsRasterDataProvider::metadata()
{
  QString s;
  return s;
}

QString QgsRasterDataProvider::capabilitiesString() const
{
  QStringList abilitiesList;

  int abilities = capabilities();

  if ( abilities & QgsRasterDataProvider::Identify )
  {
    abilitiesList += tr( "Identify" );
  }

  if ( abilities & QgsRasterDataProvider::Draw )
  {
    abilitiesList += tr( "Draw" );
  }

  if ( abilities & QgsRasterDataProvider::Data )
  {
    abilitiesList += tr( "Data" );
  }

  QgsDebugMsg( "Capability: " + abilitiesList.join( ", " ) );

  return abilitiesList.join( ", " );
}

bool QgsRasterDataProvider::identify( const QgsPoint& thePoint, QMap<QString, QString>& theResults )
{
  theResults.clear();
  return false;
}

QString QgsRasterDataProvider::lastErrorFormat()
{
  return "text/plain";
}

//void QgsRasterDataProvider::buildSupportedRasterFileFilter( QString & theFileFiltersString ) 
//{ 
  //QgsDebugMsg ( "Entered" );  
//};



// ENDS
