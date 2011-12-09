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

void QgsRasterLayerDirector::addArc( QgsGraphBuilderInterface* builder, int x1, int y1, const QgsPoint& pt1, int x2, int y2, const QVector< QVariant>& pixel1 ) const
{ 
  QgsPoint pt2( mLayerExtent.xMinimum() + (mLayerExtent.width()*x2)/mLayer->width() + mPlusWidth,
	        mLayerExtent.yMinimum() + (mLayerExtent.height()*y2)/mLayer->height() + mPlusHeight );

  QVector< QVariant > pixel2 = identify( x2, y2 );	
  QVector< QVariant > prop;
  double distance = builder->distanceArea()->measureLine( pt1, pt2 );
  QVector< const QgsRasterArcProperter* >::const_iterator it;
  for ( it = mNumericProperter.begin(); it != mNumericProperter.end(); ++it )
  {
    prop.push_back( (*it)->property( distance, pixel1, pixel2 ) );
  }
  
  builder->addArc( mLayer->width()*y1 + x1, pt1, mLayer->width()*y2 + x2, pt2, prop );
}

void QgsRasterLayerDirector::makeGraph( QgsGraphBuilderInterface *builder, const QVector< QgsPoint >& additionalPoints,
    QVector< QgsPoint >& tiedPoint ) const

{
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

  mLayerExtent = ct.transformBoundingBox( mLayer->extent() );
  
  int widthIt=0;
  int heightIt=0;
  double mPlusWidth 	= mLayerExtent.width() / mLayer->width()/2.0;
  double mPlusHeight 	= mLayerExtent.height()/ mLayer->height()/2.0;

  for( heightIt = 0; heightIt != mLayer->height(); ++heightIt )
  {
    for ( widthIt = 0; widthIt != mLayer->width(); ++widthIt )
    {

      QgsPoint pt( mLayerExtent.xMinimum() + (mLayerExtent.width()*widthIt)/mLayer->width() + mPlusWidth, 
	  mLayerExtent.yMinimum() + (mLayerExtent.height()*heightIt)/mLayer->height() + mPlusHeight );
      
      builder->addVertex( mLayer->width()*heightIt + widthIt, pt );
 
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
  
  for( heightIt = 0; heightIt != mLayer->height(); ++heightIt )
  {
    for ( widthIt = 0; widthIt != mLayer->width(); ++widthIt )
    {
      QgsPoint pt1( mLayerExtent.xMinimum() + ( mLayerExtent.width() * widthIt ) / mLayer->width() + mPlusWidth,
	  mLayerExtent.yMinimum() + ( mLayerExtent.height() * heightIt ) / mLayer->height() + mPlusHeight );
      
      QVector< QVariant > pixel1 = identify( widthIt, heightIt );
      //1
      if ( widthIt != 0 && heightIt != 0 )
      {
 	addArc( builder, widthIt, heightIt, pt1, widthIt - 1, heightIt - 1, pixel1 );
      }

      //2
      if ( heightIt != 0 )
      {
        addArc( builder, widthIt, heightIt, pt1, widthIt, heightIt - 1, pixel1 );
      }

      //3
      if ( widthIt != mLayer->width() - 1 && heightIt != 0 )
      {
        addArc( builder, widthIt, heightIt, pt1, widthIt + 1, heightIt - 1, pixel1 );
      }

      //4
      if ( widthIt != 0 )
      {
        addArc( builder, widthIt, heightIt, pt1, widthIt - 1, heightIt, pixel1 );
      }

      //5
      if ( widthIt != mLayer->width() - 1 )
      {
        addArc( builder, widthIt, heightIt, pt1, widthIt + 1, heightIt, pixel1 );
      }

      //6
      if ( widthIt != 0 && heightIt != mLayer->height() - 1 )
      {
        addArc( builder, widthIt, heightIt, pt1, widthIt - 1, heightIt + 1, pixel1 );
      }

      //7
      if ( heightIt != mLayer->height() - 1 )
      {
        addArc( builder, widthIt, heightIt, pt1, widthIt, heightIt + 1, pixel1 );
      }

      //8
      if ( widthIt != mLayer->width() - 1 && heightIt != mLayer->height() - 1 )
      {
        addArc( builder, widthIt, heightIt, pt1, widthIt + 1, heightIt + 1, pixel1 );
      }
    }
  }

} // makeGraph( QgsGraphBuilderInterface *builder, const QVector< QgsPoint >& additionalPoints, QVector< QgsPoint >& tiedPoint )
