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
#include <QVariant>
#include <QVector>

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
  QgsRasterDataProvider *provider = mLayer->dataProvider();

  tiedPoint = QVector< QgsPoint > ( additionalPoints.size(), QgsPoint( 0.0, 0.0 ) );

  QVector< double > pointsDist( additionalPoints.size(), std::numeric_limits<double>::infinity() );
  
  QgsCoordinateTransform ct;
  ct.setSourceCrs( mLayer->crs() );

  if ( builder->coordinateTransformationEnabled() )
  {
    ct.setDestCRS( builder->destinationCrs() );
  }else
  {
    ct.setDestCRS( mLayer->crs() );
  }

  QgsRectangle extent = mLayer->extent();
  int widthIt=0;
  int heightIt=0;
  double plusWidth 	= extent.width() / mLayer->width()/2.0;
  double plusHeight 	= extent.height()/ mLayer->height()/2.0;

  QVector< QgsPoint > vertex;
  vertex.reserve( mLayer->width()*mLayer->height() );

  for( heightIt = 0; heightIt != mLayer->height(); ++heightIt )
  {
    for ( widthIt = 0; widthIt != mLayer->width(); ++widthIt )
    {

      QgsPoint pt( extent.xMinimum() + (extent.width()*widthIt)/mLayer->width() + plusWidth, 
	  extent.yMinimum() + (extent.height()*heightIt)/mLayer->height() + plusHeight );
      pt = ct.transform( pt );
      vertex.push_back( pt );

      int i;
      for ( i = 0; i < pointsDist.size(); ++i )
      {
	double d = additionalPoints[i].sqrDist( pt );
	if ( pointsDist[i] > d )
	{
	  pointsDist[i] = d;
	  tiedPoint[i] = pt;
	}
      }
    }
  }
  
  double sqrt2 = 1.4142;

  //  QVector = [ pixel len, real len, time cap ]
  QVector< QVector< QVariant > > arc;
  for( heightIt = 0; heightIt != mLayer->height(); ++heightIt )
  {
    for ( widthIt = 0; widthIt != mLayer->width(); ++widthIt )
    {

      QVector< QVariant > a(3, QVariant(0.0) );
      a.push_back( QVariant( widthIt ) );
      a.push_back( QVariant( heightIt ) );
      // add [5][6]
      a.push_back( QVariant( widthIt ) );
      a.push_back( QVariant( heightIt ) );
      
      if ( widthIt != 0 && heightIt != 0 )
      {
	a[0] = sqrt2;
	a[1] = builder->distanceArea()->measureLine( 
	    vertex[ mLayer->width()*heightIt + widthIt  ], 
	    vertex[ mLayer->width()*(heightIt - 1) + widthIt - 1 ] );
	a[2] = sqrt2*a[1].toDouble(); // time cap
	a[5] = widthIt - 1;
	a[6] = heightIt - 1;
	arc.push_back( a );
      }

      if ( heightIt != 0 )
      {
 	a[0] = 1.0;
	a[1] = builder->distanceArea()->measureLine(
	    vertex[ mLayer->width()*heightIt + widthIt ], 
	    vertex[ mLayer->width()*(heightIt - 1) + widthIt ] );
	a[2] = 1.0*a[1].toDouble();
 	a[5] = widthIt;
	a[6] = heightIt - 1;
	arc.push_back( a );
      }

      if ( widthIt != mLayer->width() - 1 && heightIt != 0 )
      {
	a[0] = sqrt2;
	a[1] = builder->distanceArea()->measureLine( 
	    vertex[ mLayer->width()*heightIt + widthIt ], 
	    vertex[ mLayer->width()*(heightIt - 1) + widthIt + 1 ] );
	a[2] = sqrt2*a[1].toDouble(); // time cap
 	a[5] = widthIt + 1;
	a[6] = heightIt - 1;
	arc.push_back( a );
     }

      if ( widthIt != 0 )
      {
 	a[0] = 1.0;
	a[1] = builder->distanceArea()->measureLine( 
	    vertex[ mLayer->width()*heightIt + widthIt ], 
	    vertex[ mLayer->width()*heightIt + widthIt - 1 ] );
	a[2] = 1.0*a[1].toDouble();
 	a[5] = widthIt - 1;
	a[6] = heightIt;
	arc.push_back( a );
     }

      if ( widthIt != mLayer->width() - 1 )
      {
 	a[0] = 1.0;
	a[1] = builder->distanceArea()->measureLine( 
	    vertex[ mLayer->width()*heightIt + widthIt ], 
	    vertex[ mLayer->width()*heightIt + widthIt + 1 ] );
	a[2] = 1.0*a[1].toDouble();
 	a[5] = widthIt + 1;
	a[6] = heightIt;
	arc.push_back( a );
     }

      if ( widthIt != 0 && heightIt != mLayer->height() - 1 )
      {
	a[0] = sqrt2;
	a[1] = builder->distanceArea()->measureLine( 
	    vertex[ mLayer->width()*heightIt + widthIt ], 
	    vertex[ mLayer->width()*(heightIt + 1) + widthIt - 1 ] );
	a[2] = sqrt2*a[1].toDouble(); // time cap
 	a[5] = widthIt - 1;
	a[6] = heightIt + 1;
	arc.push_back( a );
     }

      if ( heightIt != mLayer->height() - 1 )
      {
	a[0] = 1.0;
	a[1] = builder->distanceArea()->measureLine( 
	    vertex[ mLayer->width()*heightIt + widthIt ], 
	    vertex[ mLayer->width()*(heightIt + 1) + widthIt ] );
	a[2] = 1.0*a[1].toDouble();
 	a[5] = widthIt;
	a[6] = heightIt + 1;
	arc.push_back( a );
     }

      if ( widthIt != mLayer->width() - 1 && heightIt != mLayer->height() - 1 )
      {
	a[0] = sqrt2;
	a[1] = builder->distanceArea()->measureLine( 
	    vertex[ mLayer->width()*heightIt + widthIt ], 
	    vertex[ mLayer->width()*(heightIt + 1) + widthIt + 1 ] );
	a[2] = sqrt2*a[1].toDouble(); // time cap
	a[5] = widthIt + 1;
	a[6] = heightIt + 1;
	arc.push_back( a );
      }
    }
  }

  int i;
  for ( i = 0; i < vertex.size(); ++i )
  {
    builder->addVertex( i, vertex[i] );
  }
  for ( i = 0; i < arc.size(); ++ i )
  {
    QVector< QVariant > prop;
    prop.push_back( arc[i][1] );
    prop.push_back( arc[i][2] );
    int i1 = mLayer->width()*arc[i][6].toInt()+arc[i][5].toInt();
    int i2 = mLayer->width()*arc[i][4].toInt()+arc[i][3].toInt();
    builder->addArc( i1, vertex[i1], i2, vertex[i2], prop );
  }
} // makeGraph( QgsGraphBuilderInterface *builder, const QVector< QgsPoint >& additionalPoints, QVector< QgsPoint >& tiedPoint )
