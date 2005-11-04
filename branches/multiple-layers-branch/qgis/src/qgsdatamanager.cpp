/***************************************************************************
                          qgsdatamanager.cpp
                             -------------------
    begin                : 24, August 2005
    copyright            : (C) 2005 by Mark Coletti
    email                : mcoletti -> gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsdatamanager.h"


#include "qgsvectordataprovider.h"
#include "qgsproviderregistry.h"
#include "qgsmaplayerregistry.h"
#include "qgsmapcanvas.h"
#include "qgsmaplayer.h"

#include <list>

using namespace std;



static const char* const ident_ = "$Id$";



static QgsDataManager * instance_ = 0x0;


QgsDataManager::QgsDataManager()
{
} // QgsDataManager ctor



QgsDataManager::~QgsDataManager()
{
  if ( instance_ )
  {
    delete instance_;
  }
} // QgsDataManager dtor




QgsDataManager &
QgsDataManager::instance()
{
  if ( ! instance_ )
  {
    instance_ = new QgsDataManager;
  }
  
  return *instance_;
} // QgsDataManager::instance()



/** open the given data provider

  This generally means creating layers for each corresponding layer found in
  the data source.  Each newly created layer is then added to the map layer
  registry.

  This consolidates functionality in both overloaded
  QgsDataManager::openVector()s.

*/
static
bool
openVectorDataProvider_( QgsDataProvider * vectorDataProvider,
                         QgsMapCanvas & canvas )
{

  if ( ! vectorDataProvider )
  {
      QgsDebug( "Null vector data provider" );

      return false;
  }

  QgsDebug( "about to create layers from vector data provider" );

  // now get all the layers corresponding to the data provider

  list<QgsMapLayer*> mapLayers = vectorDataProvider->createLayers();

  if ( mapLayers.empty() )
  {
      QgsDebug( "Unable to get create map layers" );

      return false;
  }
  else
  {
      QgsDebug( QString("Got " + QString::number(mapLayers.size()) + " layers").ascii() );
  }


  // and then add them to the map layer registry; each time a layer is added,
  // a signal is emitted which will cause the GUI to be updated with the new
  // layer

  for ( list<QgsMapLayer*>::iterator i = mapLayers.begin();
        i != mapLayers.end();
        ++i )
  {
      QgsMapLayerRegistry::instance()->addMapLayer( *i );

      QObject::connect(*i, SIGNAL(editingStopped(bool)), 
                       &canvas, SLOT(removeDigitizingLines(bool)));

      (*i)->setVisible(true);
//    layer->setVisible(mAddedLayersHidden); // XXX need to pass in this flag
      (*i)->refreshLegend();
  }

  return true;

} // openVectorDataProvider_



bool QgsDataManager::openVector( QString const & dataSource,
                                 QgsMapCanvas & canvas  )
{
  // find the default provider that can handle the given name
  QgsDataProvider * vectorDataProvider = // XXX ogr temporarily hard-coded
      QgsProviderRegistry::instance()->openVector( dataSource, "ogr" ); 

  return openVectorDataProvider_( vectorDataProvider, canvas );

} // QgsDataManager::openVector



bool QgsDataManager::openVector( QString const & dataSource, 
                                 QString const & providerKey,
                                 QgsMapCanvas & canvas  )
{
  // find the default provider that can handle the given name

  QgsDataProvider * vectorDataProvider = 
      QgsProviderRegistry::instance()->openVector( dataSource, providerKey );

  return openVectorDataProvider_( vectorDataProvider, canvas );

} // QgsDataManager::openVector



bool QgsDataManager::openRaster( QString const & name,
                                 QgsMapCanvas & canvas  )
{
  return false;
} // QgsDataManager::openRaster



bool QgsDataManager::openRaster( QString const & name, 
                                 QString const & provider,
                                 QgsMapCanvas & canvas  )
{
  return false;
} // QgsDataManager::openRaster


