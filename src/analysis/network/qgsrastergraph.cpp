/***************************************************************************
 *   Copyright (C) 2011 by Sergey Yakushev                                 *
 *   yakushevs <at >list.ru                                                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

/**
 * \file qgsmemorygraph.cpp
 * \brief implementation QgsMemoryGraph
 */

#include <raster/qgsrasterlayer.h>
#include <qgsdataprovider.h>

#include "qgsrastergraph.h"

QgsRasterGraph::QgsRasterGraph( QgsRasterLayer *layer, bool coordinateTransformEnabled, const QgsCoordinateReferenceSystem& destCrs )
{
  
  QgsRasterDataProvider *prov = layer->dataProvider();
  if ( prov == NULL )
    return;
  
  int i = 0;
  for( i = 0; i < prov->bandCount(); ++i )
  {
       
    int dataTypeSize = prov->dataTypeSize( i + 1 );
    int size = prov->xSize()*prov->ySize()*dataTypeSize/8/sizeof(char);
     
    unsigned char *data = new unsigned char[ size ];
    prov->readBlock( i + 1, layer->extent(), layer->width(), layer->height(), data );
   
    mDataType.push_back( prov->dataType( i + 1 ) );
    mDataTypeSize.push_back( dataTypeSize );
    QVector< double > d( prov->xSize()*prov->ySize() );
    
    int k1;
    for ( k1 = 0; k1 < prov->xSize()*prov->ySize() ; ++k1)
    {
      double t = prov->readValue( data, prov->dataType( i + 1 ), k1 );
      int x = k1 % prov->xSize();
      int y = prov->ySize() - ( k1 / prov->xSize() ) - 1;
      d[ y*prov->xSize() + x ] = t;
    }
    mData.push_back( d );
    delete [] data;
  }

  mWidth = layer->width();
  mHeight = layer->height();
  mExtent = layer->extent();
  mExtent.normalize();

  mCoordinateTransform.setSourceCrs( layer->crs() );
  mDistanceArea.setSourceCrs( layer->crs().srsid() );

  if ( coordinateTransformEnabled )
  {
    mCoordinateTransform.setDestCRS( destCrs );
    mDistanceArea.setProjectionsEnabled( true );
  }
}

QgsRasterGraph::~QgsRasterGraph()
{
}

const QgsGraphVertex QgsRasterGraph::vertex( int idx ) const
{
  double plusWidth = mExtent.width()/mWidth/2;
  double plusHeight = mExtent.height()/mHeight/2;

  int x = idx % mWidth;
  int y = idx / mWidth;
  
  QgsGraphVertex v;
  v.mCoordinate = QgsPoint( mExtent.xMinimum() + mExtent.width() * x / mWidth + plusWidth,
    mExtent.yMinimum() + mExtent.height() * y / mHeight + plusHeight );
  v.mCoordinate = mCoordinateTransform.transform( v.mCoordinate );

  int i = 0;
  if ( y == 0 )
  {
    if ( x == 0 )
    {
      for ( i = 0; i < 3; ++i )
        v.mOutArc.push_back( i );
      
      /*v.mInArc.push_back( 16 );
      v.mInArc.push_back( 12 + (mWidth - 2) * 5 * 2 + ( mHeight - 2 ) * 5 * 2 + 7 );
      v.mInArc.push_back( 12 + (mWidth - 2) * 5 * 2 );*/
    }else if ( x == mWidth - 1 )
    {
      for ( i = 3; i < 6; ++i )
        v.mOutArc.push_back( i );
      /*v.mInArc.push_back( 12 + (mWidth-2)*5 - 5 );
      v.mInArc.push_back( 12 + (mWidth-2)*5*2 + (mHeight-2)*5 );
      v.mInArc.push_back( 12 + (mWidth-2)*5*2 + (mHeight-2)*5*2 + 1 );*/
    }else
    {
      for ( i = 0; i < 5; ++i )
        v.mOutArc.push_back( 12 + (x - 1) * 5 + i );    
    }
  }else if ( y == mHeight - 1 )
  {
    if ( x == 0 )
    {
      for ( i = 6; i < 9; ++i )
        v.mOutArc.push_back( i );
    }else if ( x == mWidth - 1 )
    {
      for ( i = 9; i < 12; ++i )
        v.mOutArc.push_back( i );
    }else
    {
      for ( i = 0; i < 5; ++i )
        v.mOutArc.push_back( 12 + (mWidth - 2) * 5 + (x - 1) * 5 + i );
    }
  }else
  {
    if ( x == 0 )
    {
      for (i = 0; i < 5; ++i ) 
        v.mOutArc.push_back( 12 + (mWidth - 2) * 5 * 2 + (y - 1) * 5 + i );
    }else if ( x == mWidth - 1 )
    {
      for (i = 0; i < 5; ++i )
        v.mOutArc.push_back( 12 + (mWidth - 2) * 5 * 2 + (mHeight - 2) * 5 + (y - 1) * 5  + i );
    }else
    {
      for (i = 0; i < 8; ++i )
        v.mOutArc.push_back( 12 + (mWidth - 2) * 5 * 2 + (mHeight - 2) * 5 * 2 + (x-1)*8 + (y-1)*(mWidth-2)*8 + i );
    }
  }
  return v;
}

const QgsGraphArc QgsRasterGraph::arc( int idx ) const
{
  QgsGraphArc arc;
  arc.mIn = 0;
  arc.mOut = 0;
  if ( idx < 12 )
  {
    if ( idx == 0 )
    {
      arc.mOut = 0;
      arc.mIn  = 1;
    }else if ( idx == 1 )
    {
      arc.mOut = 0;
      arc.mIn  = mWidth + 1;
    }else if ( idx == 2 )
    {
      arc.mOut = 0;
      arc.mIn  = mWidth;
    }else if ( idx == 3 )
    { 
      arc.mOut = mWidth - 1;
      arc.mIn  = mWidth + (mWidth - 1);
    }else if ( idx == 4 )
    {
      arc.mOut = mWidth - 1;
      arc.mIn  = mWidth + (mWidth - 2);
    }else if ( idx == 5 )
    {
      arc.mOut = mWidth - 1;
      arc.mIn  = mWidth - 2;
    }else if ( idx == 6 )
    {
      arc.mOut = mWidth * (mHeight - 1);
      arc.mIn  = mWidth * (mHeight - 2);
    }else if ( idx == 7 )
    {
      arc.mOut = mWidth * (mHeight - 1);
      arc.mIn  = mWidth * (mHeight - 2) + 1;
    }else if ( idx == 8 )
    {
      arc.mOut = mWidth * (mHeight - 1);
      arc.mIn  = mWidth * (mHeight - 1) + 1;
    }else if ( idx == 9 )
    {
      arc.mOut = mWidth * mHeight - 1;
      arc.mIn  = mWidth * (mHeight - 1) - 1;
    }else if ( idx == 10 )
    {
      arc.mOut = mWidth * mHeight - 1;
      arc.mIn  = mWidth * mHeight - 2;
    }else if ( idx == 11 )
    {
      arc.mOut = mWidth * mHeight - 1;
      arc.mIn  = mWidth * (mHeight - 1) - 2; 
    }
  }else if ( idx < 12 + (mWidth - 2) * 5 * 2 )
  {
    if ( idx < 12 + (mWidth - 2) * 5 )
    {
      arc.mOut = (idx - 12) / 5 + 1;
      if ( (idx - 12) % 5 == 0 )
      {
        arc.mIn = arc.mOut + 1;
      }else if ( (idx - 12) % 5 == 4 )
      {
        arc.mIn = arc.mOut - 1;
      }else
      {
        arc.mIn = arc.mOut + mWidth + 2 -(idx - 12) % 5;
      }
    }else
    {
      arc.mOut = (mWidth * ( mHeight - 1 )) + (idx - 12 - (mWidth - 2) * 5) / 5 + 1;
      if ( (idx - 12 - (mWidth - 2) * 5) % 5 == 2 )
      {
        arc.mIn = arc.mOut + 1;
      }else if ( (idx - 12 - (mWidth - 2) * 5) % 5 == 3 )
      {
        arc.mIn = arc.mOut - 1;
      }else if ( (idx - 12 - (mWidth - 2) * 5) % 5 == 4 )
      {
        arc.mIn = arc.mOut - mWidth - 1; 
      }else 
      {
        arc.mIn = arc.mOut - mWidth + (idx - 12 - (mWidth - 2) * 5) % 5;
      }
    }
  }else if ( idx < 12 + (mWidth - 2) * 2 * 5 + (mHeight - 2) * 2 *5 )
  {
    if ( idx < 12 + (mWidth - 2) * 2 * 5 + (mHeight - 2) * 5 )
    {
      arc.mOut = ((idx - 12 - (mWidth - 2) * 2 * 5 ) / 5 + 1) * mWidth;
      if ( ( idx - 12 - (mWidth - 2) * 2 * 5 ) % 5 == 0 )
      {
        arc.mIn = arc.mOut - mWidth;
      }else if ( ( idx - 12 - (mWidth - 2) * 2 * 5 ) % 5 == 4 )
      {
        arc.mIn = arc.mOut + mWidth;
      }else
      {
        arc.mIn = arc.mOut + 1 + mWidth*( ( idx - 12 - (mWidth - 2) * 2 * 5 ) % 5 - 2 );
      }
    }else
    {
      arc.mOut = ( idx - 12 - (mWidth - 2) * 2 * 5 - (mHeight - 2 ) * 5 ) / 5; 
      arc.mOut = (arc.mOut + 1)*mWidth + mWidth - 1;
      if ( ( idx - 12 - (mWidth - 2) * 2 * 5 - (mHeight - 2 ) * 5 ) % 5 == 0 )
      {
        arc.mIn = arc.mOut - mWidth;
      }else if ( ( idx - 12 - (mWidth - 2) * 2 * 5 - (mHeight - 2) * 5 ) % 5 == 1 )
      {
        arc.mIn = arc.mOut + mWidth;
      }else
      {
        arc.mIn = arc.mOut - 1 + ( ( idx - 12 - (mWidth - 2) * 2 * 5 - (mHeight - 2) * 5 )%5 - 3 )*mWidth;
      }
    }
  }else
  {
    arc.mOut = (idx - 12 - (mWidth - 2) * 2 * 5 - (mHeight - 2) * 2 * 5) / 8;
    int x = arc.mOut % (mWidth - 2);
    int y = arc.mOut / (mWidth - 2);
    
    arc.mOut = mWidth*(y+1) + 1 + x;// 44
    if ( (idx - 12 - (mWidth - 2) * 2 * 5 - (mHeight - 2) * 2 * 5) % 8 == 4 )
    {
      arc.mIn = arc.mOut - mWidth;
    }else if ( (idx - 12 - (mWidth - 2) * 2 * 5 - (mHeight - 2) * 2 * 5) % 8 == 0 )
    {
      arc.mIn = arc.mOut + mWidth;
    }else if ( (idx - 12 - (mWidth - 2) * 2 * 5 - (mHeight - 2) * 2 * 5) % 8 < 4 )
    {
      arc.mIn = arc.mOut + 1 + ((idx - 12 - (mWidth - 2) * 2 * 5 - (mHeight - 2) * 2 * 5) % 8 - 2 )*mWidth;
    }else
    {
      arc.mIn = arc.mOut - 1 + ((idx - 12 - (mWidth - 2) * 2 * 5 - (mHeight - 2) * 2 * 5) % 8 - 6 )*mWidth;
    }
  }
  
  double plusWidth = mExtent.width()/mWidth/2;
  double plusHeight = mExtent.height()/mHeight/2;

  int x = arc.mOut % mWidth;
  int y = arc.mOut / mWidth;
 
  QgsPoint pt1( mExtent.xMinimum() + mExtent.width() * x / mWidth + plusWidth,
    mExtent.yMinimum() + mExtent.height() * y / mHeight + plusHeight );

  x = arc.mIn % mWidth;
  y = arc.mIn / mWidth;

  QgsPoint pt2( mExtent.xMinimum() + mExtent.width() * x / mWidth + plusWidth,
          mExtent.yMinimum() + mExtent.height() * y / mHeight + plusHeight );
  
  //FIXME:currently for any test
  double dist = mDistanceArea.measureLine( pt1, pt2 );
  arc.mProperties.push_back( dist ); 
  arc.mProperties.push_back( dist*( mData[0][arc.mIn]+mData[0][arc.mOut] )/2 );

  return arc;
}

int QgsRasterGraph::vertexCount() const
{
  return mWidth * mHeight;
}

int QgsRasterGraph::arcCount() const
{
  return 4 * 3 + ( (mWidth - 2) * 5) * 2 + ( (mHeight - 2) * 5) * 2 + (mWidth - 2) * (mHeight - 2) * 8;
}

int QgsRasterGraph::findVertex( const QgsPoint& point ) const
{
  QgsPoint pt = mCoordinateTransform.transform( point, QgsCoordinateTransform::ReverseTransform );
  
  if ( !mExtent.contains( pt ) )
    return -1;

  int x = floor( ( pt.x() - mExtent.xMinimum() ) * mWidth / mExtent.width() );
  int y = floor( ( pt.y() - mExtent.yMinimum() ) * mHeight / mExtent.height() );

  return mWidth * y + x;
}
