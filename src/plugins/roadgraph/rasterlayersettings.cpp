/***************************************************************************
 *   Copyright (C) 2011 by Sergey Yakushev                                 *
 *   yakushevs <at> gmail.com                                              *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

/**
 * \file linevectorlayersettings.cpp
 * \brief implementation of RgLineVectorLayerSettings
 */

#include "rasterlayersettings.h"
#include "rasterlayerwidget.h"
#include "units.h"

// Qgis includes
#include <qgsrasterlayer.h>
#include <qgsproject.h>
#include <qgsmaplayerregistry.h>
#include <qgsrastergraph.h>
#include <qgscoordinatereferencesystem.h>
#include <qgsdistancearea.h>

// QT includes
#include <QComboBox>

//standard includes

RgRasterLayerSettings::RgRasterLayerSettings()
{
  mLayer = "";
}
RgRasterLayerSettings::~RgRasterLayerSettings()
{

}
bool RgRasterLayerSettings::test()
{
  return true;
} // RgLineVectorLayerSettings::test()

void RgRasterLayerSettings::read( const QgsProject *project )
{
  mLayer          = project->readEntry( "roadgraphplugin", "/layer" );
  mSpeedBand      = project->readNumEntry( "roadgraphplugin", "/speedBand" );
  mSpeedUnitName  = project->readEntry( "roadgraphplugin", "/speedUnitName" );
} // RgLineVectorLayerSettings::read( const QgsProject *project )

void RgRasterLayerSettings::write( QgsProject *project )
{
  project->writeEntry( "roadgraphplugin", "/layer", mLayer );
  project->writeEntry( "roadgraphplugin", "/speedBand", mSpeedBand );
  project->writeEntry( "roadgraphplugin", "/speedUnitName", mSpeedUnitName );
} // RgLineVectorLayerSettings::write( QgsProject *project )

QWidget* RgRasterLayerSettings::getGui( QWidget *parent )
{
  return new RgRasterLayerSettingsWidget( this, parent );
//  return new RgLineVectorLayerSettingsWidget( this, parent );
}

void RgRasterLayerSettings::setFromGui( QWidget *myGui )
{
  RgRasterLayerSettingsWidget* widget = dynamic_cast<RgRasterLayerSettingsWidget*>( myGui );
  if ( widget == NULL )
    return;
  mLayer = widget->mcbLayers->currentText();
  
  mSpeedBand  = widget->mcbSpeed->itemData( widget->mcbSpeed->currentIndex() ).toInt();

  mSpeedUnitName = widget->mcbSpeed->itemData( widget->mcbSpeed->currentIndex() ).toString();
}

QString RgRasterLayerSettings::name()
{
  return QString( "raster layer" );
}

QgsGraph* RgRasterLayerSettings::graph( const QgsCoordinateReferenceSystem& crs, bool crsTransformEnabled, const QVector< QgsPoint >& additionalPoint, QVector< QgsPoint >& tiedPoint )
{
  QgsRasterLayer *layer = NULL;
  QMap< QString, QgsMapLayer* > mapLayers = QgsMapLayerRegistry::instance()->mapLayers();
  QMap< QString, QgsMapLayer* >::const_iterator it;
  for ( it = mapLayers.begin(); it != mapLayers.end(); ++it )
  {
    if ( it.value()->name() != mLayer )
      continue;
    layer = dynamic_cast< QgsRasterLayer* >( it.value() );
    break;
  }
  if ( layer == NULL )
    return NULL;
  
  QgsRasterGraph *graph = new QgsRasterGraph( layer, crsTransformEnabled, crs );

  tiedPoint = QVector< QgsPoint >( additionalPoint.size(), QgsPoint(0.0,0.0) );
  QVector< double > distance( additionalPoint.size(), std::numeric_limits<double>::infinity() );

  for ( int j = 0; j < tiedPoint.size(); ++j )
  {
    int vertexId = graph->findVertex( additionalPoint[j] );
    if ( vertexId != -1 )
      tiedPoint[j] = graph->vertex( vertexId ).mCoordinate;
  }
  return graph;
}
