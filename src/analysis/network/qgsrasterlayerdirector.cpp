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
  mLayer = myLayer;
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
  tiedPoint = QVector< QgsPoint > ( additionalPoints.size(), QgsPoint( 0.0, 0.0 ) );
  
  QgsCoordinateTransform ct;
  ct.setSourceCrs( mLayer->crs() );

  if ( builder->coordinateTransformationEnabled() )
  {
    ct.setDestCRS( builder->destinationCrs() );
  }else
  {
    ct.setDestCRS( mLayer->crs() );
  }
  
  int widthIt=0;
  int heightIt=0;

  for( widthIt = 0; widthIt <= mLayer->width(); ++widthIt )
  {
    for ( heightIt = 0; heightIt <= mLayer->height(); ++heightIt )
    {
      QgsRectangle extent = mLayer->extent();

      QgsPoint pt( extent.xMinimum() + (extent.width()*widthIt)/mLayer->width(), 
	  extent.yMinimum() + (extent.height()*heightIt)/mLayer->height() );
      pt = ct.transform( pt );
      
    }
  }

} // makeGraph( QgsGraphBuilderInterface *builder, const QVector< QgsPoint >& additionalPoints, QVector< QgsPoint >& tiedPoint )

