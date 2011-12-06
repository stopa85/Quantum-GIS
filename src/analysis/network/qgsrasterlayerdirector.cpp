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
#include "qgsrasterarcproperter.h"

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

void QgsRasterLayerDirector::addNumericProperter( const QgsRasterArcProperter* prop )
{
  mNumericProperter.push_back( prop );
}

QVector< QVariant > QgsRasterLayerDirector::identify( int widthIt, int heightIt ) const
{
  QgsRectangle extent = mLayer->extent();
  double plusWidth 	= extent.width() / mLayer->width()/2.0;
  double plusHeight 	= extent.height()/ mLayer->height()/2.0;

  QMap< QString, QString > identmap;      
  QMap< QString, QString >::iterator it;

  QgsPoint pt( extent.xMinimum() + (extent.width()*widthIt)/mLayer->width() + plusWidth,
      extent.yMinimum() + (extent.height()*heightIt)/mLayer->height() + plusHeight );
      
  mLayer->dataProvider()->identify( pt, identmap );
  QVector< QVariant > v;
  for ( it = identmap.begin(); it != identmap.end(); ++it )
  {
    v.push_back( it.value() );
  }
  return v;
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

  QVector< QVector< QVariant > > arc;
  for( heightIt = 0; heightIt != mLayer->height(); ++heightIt )
  {
    for ( widthIt = 0; widthIt != mLayer->width(); ++widthIt )
    {
      
      QVector< QVariant > pixel1 = identify( widthIt, heightIt );
      //1
      if ( widthIt != 0 && heightIt != 0 )
      {
 	int x = widthIt - 1;
	int y = heightIt - 1;
 	QVector< QVariant > pixel2 = identify( x, y );
	
	QVector< QVariant > a(5);	
	a[0] = sqrt2;// pixel length
	a[1] = x;
	a[2] = y;
	a[3] = widthIt;
	a[4] = heightIt;

	double distance = builder->distanceArea()->measureLine(
	                vertex[ mLayer->width()*heightIt + widthIt  ],
			vertex[ mLayer->width()*y + x ] );
	QVector< const QgsRasterArcProperter* >::const_iterator it;
	for ( it = mNumericProperter.begin(); it != mNumericProperter.end(); ++it )
	{
	  a.push_back( (*it)->property( distance, pixel1, pixel2 ) );
	}
	arc.push_back( a );
      }

      //2
      if ( heightIt != 0 )
      {
 	int x = widthIt;
	int y = heightIt - 1;
 	QVector< QVariant > pixel2 = identify( x, y );
	
	QVector< QVariant > a(5);	
	a[0] = sqrt2;// pixel length
	a[1] = x;
	a[2] = y;
	a[3] = widthIt;
	a[4] = heightIt;

	double distance = builder->distanceArea()->measureLine(
	                vertex[ mLayer->width()*heightIt + widthIt  ],
			vertex[ mLayer->width()*y + x ] );
	QVector< const QgsRasterArcProperter* >::const_iterator it;
	for ( it = mNumericProperter.begin(); it != mNumericProperter.end(); ++it )
	{
	  a.push_back( (*it)->property( distance, pixel1, pixel2 ) );
	}
	arc.push_back( a );
      }

      //3
      if ( widthIt != mLayer->width() - 1 && heightIt != 0 )
      {
 	int x = widthIt + 1;
	int y = heightIt - 1;
 	QVector< QVariant > pixel2 = identify( x, y );
	
	QVector< QVariant > a(5);	
	a[0] = sqrt2;// pixel length
	a[1] = x;
	a[2] = y;
	a[3] = widthIt;
	a[4] = heightIt;

	double distance = builder->distanceArea()->measureLine(
	                vertex[ mLayer->width()*heightIt + widthIt  ],
			vertex[ mLayer->width()*y + x ] );
	QVector< const QgsRasterArcProperter* >::const_iterator it;
	for ( it = mNumericProperter.begin(); it != mNumericProperter.end(); ++it )
	{
	  a.push_back( (*it)->property( distance, pixel1, pixel2 ) );
	}
	arc.push_back( a );
      }

      //4
      if ( widthIt != 0 )
      {
	int x = widthIt - 1;
	int y = heightIt;
 	QVector< QVariant > pixel2 = identify( x, y );
	
	QVector< QVariant > a(5);	
	a[0] = sqrt2;// pixel length
	a[1] = x;
	a[2] = y;
	a[3] = widthIt;
	a[4] = heightIt;

	double distance = builder->distanceArea()->measureLine(
	                vertex[ mLayer->width()*heightIt + widthIt  ],
			vertex[ mLayer->width()*y + x ] );
	QVector< const QgsRasterArcProperter* >::const_iterator it;
	for ( it = mNumericProperter.begin(); it != mNumericProperter.end(); ++it )
	{
	  a.push_back( (*it)->property( distance, pixel1, pixel2 ) );
	}
	arc.push_back( a );

      }

      //5
      if ( widthIt != mLayer->width() - 1 )
      {
	int x = widthIt + 1;
	int y = heightIt;
 	QVector< QVariant > pixel2 = identify( x, y );
	
	QVector< QVariant > a(5);	
	a[0] = sqrt2;// pixel length
	a[1] = x;
	a[2] = y;
	a[3] = widthIt;
	a[4] = heightIt;

	double distance = builder->distanceArea()->measureLine(
	                vertex[ mLayer->width()*heightIt + widthIt  ],
			vertex[ mLayer->width()*y + x ] );
	QVector< const QgsRasterArcProperter* >::const_iterator it;
	for ( it = mNumericProperter.begin(); it != mNumericProperter.end(); ++it )
	{
	  a.push_back( (*it)->property( distance, pixel1, pixel2 ) );
	}
	arc.push_back( a );
      }

      //6
      if ( widthIt != 0 && heightIt != mLayer->height() - 1 )
      {
	int x = widthIt - 1;
	int y = heightIt + 1;
 	QVector< QVariant > pixel2 = identify( x, y );
	
	QVector< QVariant > a(5);	
	a[0] = sqrt2;// pixel length
	a[1] = x;
	a[2] = y;
	a[3] = widthIt;
	a[4] = heightIt;

	double distance = builder->distanceArea()->measureLine(
	                vertex[ mLayer->width()*heightIt + widthIt  ],
			vertex[ mLayer->width()*y + x ] );
	QVector< const QgsRasterArcProperter* >::const_iterator it;
	for ( it = mNumericProperter.begin(); it != mNumericProperter.end(); ++it )
	{
	  a.push_back ( (*it)->property( distance, pixel1, pixel2 ) );
	}
	arc.push_back( a );

      }

      //7
      if ( heightIt != mLayer->height() - 1 )
      {
	int x = widthIt;
	int y = heightIt + 1;
 	QVector< QVariant > pixel2 = identify( x, y );
	
	QVector< QVariant > a(5);	
	a[0] = sqrt2;// pixel length
	a[1] = x;
	a[2] = y;
	a[3] = widthIt;
	a[4] = heightIt;

	double distance = builder->distanceArea()->measureLine(
	                vertex[ mLayer->width()*heightIt + widthIt  ],
			vertex[ mLayer->width()*y + x ] );
	QVector< const QgsRasterArcProperter* >::const_iterator it;
	for ( it = mNumericProperter.begin(); it != mNumericProperter.end(); ++it )
	{
	  a.push_back ( (*it)->property( distance, pixel1, pixel2 ) );
	}
	arc.push_back( a );
      }

      //8
      if ( widthIt != mLayer->width() - 1 && heightIt != mLayer->height() - 1 )
      {
	int x = widthIt + 1;
	int y = heightIt + 1;
 	QVector< QVariant > pixel2 = identify( x, y );
	
	QVector< QVariant > a(5);	
	a[0] = sqrt2;// pixel length
	a[1] = x;
	a[2] = y;
	a[3] = widthIt;
	a[4] = heightIt;

	double distance = builder->distanceArea()->measureLine(
	                vertex[ mLayer->width()*heightIt + widthIt  ],
			vertex[ mLayer->width()*y + x ] );
	QVector< const QgsRasterArcProperter* >::const_iterator it;
	for ( it = mNumericProperter.begin(); it != mNumericProperter.end(); ++it )
	{
	  a.push_back( (*it)->property( distance, pixel1, pixel2 ) );
	}
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

    int j = 0;
    for ( j = 0; j < mNumericProperter.size(); ++j )
    {
      prop.push_back( arc[i][j+5] );
    }
    
    int i1 = mLayer->width()*arc[i][4].toInt()+arc[i][3].toInt();
    int i2 = mLayer->width()*arc[i][2].toInt()+arc[i][1].toInt();
    builder->addArc( i2, vertex[i2], i1, vertex[i1], prop );
  }
} // makeGraph( QgsGraphBuilderInterface *builder, const QVector< QgsPoint >& additionalPoints, QVector< QgsPoint >& tiedPoint )
