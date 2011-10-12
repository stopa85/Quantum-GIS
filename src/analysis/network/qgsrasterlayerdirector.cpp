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
 * \file qgsrasterlayerdirector.cpp
 * \brief implementation of QgsRasterLayerDirector
 */

#include "qgsrasterlayerdirector.h"
#include "qgsgraphbuilderintr.h"

// Qgis includes
#include <qgsrasterlayer.h>

// QT includes
#include <QString>
#include <QtAlgorithms>

//standard includes
#include <limits>
#include <algorithm>

QgsRasterLayerDirector::QgsRasterLayerDirector( QgsRasterLayer *myLayer )
{
  mRasterLayer = myLayer;
}

QgsRasterLayerDirector::~QgsRasterLayerDirector()
{

}

QString QgsRasterLayerDirector::name() const
{
  return QString( "Raster" );
}

void QgsRasterLayerDirector::makeGraph( QgsGraphBuilderInterface *builder, const QVector< QgsPoint >& additionalPoints,
    QVector< QgsPoint >& tiedPoint ) const
{
} // makeGraph( QgsGraphBuilderInterface *builder, const QVector< QgsPoint >& additionalPoints, QVector< QgsPoint >& tiedPoint )

