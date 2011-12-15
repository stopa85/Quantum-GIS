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

#include "qgsmemorygraph.h"

int QgsMemoryGraph::addVertex( const QgsPoint& pt )
{
  QgsGraphVertex v;
  v.mCoordinate = pt;
  mGraphVertexes.append( v );
  return mGraphVertexes.size() - 1;
}

int QgsMemoryGraph::addArc( int outVertexIdx, int inVertexIdx, const QVector< QVariant >& properties )
{
  QgsGraphArc e;

  e.mProperties = properties;
  e.mOut = outVertexIdx;
  e.mIn  = inVertexIdx;
  mGraphArc.push_back( e );
  int edgeIdx = mGraphArc.size() - 1;

  mGraphVertexes[ outVertexIdx ].mOutArc.push_back( edgeIdx );
  mGraphVertexes[ inVertexIdx ].mInArc.push_back( edgeIdx );

  return mGraphArc.size() - 1;
}

const QgsGraphVertex QgsMemoryGraph::vertex( int idx ) const
{
  return mGraphVertexes[ idx ];
}

const QgsGraphArc QgsMemoryGraph::arc( int idx ) const
{
  return mGraphArc[ idx ];
}


int QgsMemoryGraph::vertexCount() const
{
  return mGraphVertexes.size();
}

int QgsMemoryGraph::arcCount() const
{
  return mGraphArc.size();
}

int QgsMemoryGraph::findVertex( const QgsPoint& pt ) const
{
  int i = 0;
  for ( i = 0; i < mGraphVertexes.size(); ++i )
  {
    if ( mGraphVertexes[ i ].mCoordinate == pt )
    {
      return i;
    }
  }
  return -1;
}
