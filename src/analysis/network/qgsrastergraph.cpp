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

QgsRasterGraph::QgsRasterGraph( QgsRasterLayer *layer )
{
  
/*  QgsRasterDataProvider *prov = layer->dataProvider();
  if ( prov == NULL )
    return;
  
  int i = 0;
  for( i = 0; i < prov->bandCount(); ++i )
  {
       
    int dataTypeSize = prov->dataTypeSize( i );
    char *data = new char[ prov->xSize()*prov->ySize()*dataTypeSize ];
    prov->readBlock( i, 0, 0, data );
   
    mDataType.push_back( prov->dataType( i ) );
    mDataTypeSize.push_back( dataTypeSize );
    mData.push_back( data );

  } */

  mWidth = 3;
  mHeight = 3;
  mExtent = QgsRectangle(0.0,0.0,1,1);

}

const QgsGraphVertex QgsRasterGraph::vertex( int idx ) const
{
  double plusWidth = mExtent.width()/mWidth/2;
  double plusHeight = mExtent.height()/mHeight/2;

  int x = idx % mWidth;
  int y = idx / mHeight;
  
  QgsGraphVertex v;
  v.mCoordinate = QgsPoint( mExtent.xMinimum() + mExtent.width() * x / mWidth + plusWidth,
    mExtent.yMinimum() + mExtent.height() * y / mHeight + plusHeight );

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
        v.mOutArc.push_back( 12 + (mWidth - 2) * 5 * 2 + i );
    }else if ( x == mWidth - 1 )
    {
      for (i = 0; i < 5; ++i )
        v.mOutArc.push_back( 12 + (mWidth - 2) * 5 * 2 + (mHeight - 2) * 5 + i );
    }else
    {
      for (i = 0; i < 8; ++i )
        v.mOutArc.push_back( 12 + (mWidth - 2) * 5 * 2 + (mHeight - 2) * 5 * 2 + i );
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
        arc.mIn = (idx - 12) % 5 + mWidth - 1;
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
    arc.mOut = mWidth + 1 + arc.mOut / ( mWidth - 1 ) + arc.mOut % (mWidth - 1);
    if ( (idx - 12 - (mWidth - 2) * 2 * 5 - (mHeight - 2) * 2 * 5) % 8 == 0 )
    {
      arc.mIn = arc.mOut - mWidth;
    }else if ( (idx - 12 - (mWidth - 2) * 2 * 5 - (mHeight - 2) * 2 * 5) % 8 == 4 )
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

int QgsRasterGraph::findVertex( const QgsPoint& pt ) const
{
  return -1;
}
