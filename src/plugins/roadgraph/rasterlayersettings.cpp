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

// Qgis includes
#include <qgsproject.h>
#include <qgsgraphdirector.h>

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

} // RgLineVectorLayerSettings::read( const QgsProject *project )

void RgRasterLayerSettings::write( QgsProject *project )
{

} // RgLineVectorLayerSettings::write( QgsProject *project )

QWidget* RgRasterLayerSettings::getGui( QWidget *parent )
{
  return new RgRasterLayerSettingsWidget( this, parent );
//  return new RgLineVectorLayerSettingsWidget( this, parent );
}

void RgRasterLayerSettings::setFromGui( QWidget *myGui )
{
}

QString RgRasterLayerSettings::name()
{
  return QString( "raster layer" );
}
QgsGraphDirector* RgRasterLayerSettings::director()
{
  return NULL;
}
