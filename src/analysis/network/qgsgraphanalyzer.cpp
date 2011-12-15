/***************************************************************************
    qgsgraphanlyzer.cpp - QGIS Tools for graph analysis
                             -------------------
    begin                : 14 april 2011
    copyright            : (C) Sergey Yakushev
    email                : Yakushevs@list.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// C++ standard includes
#include <limits>

// QT includes
#include <QMap>
#include <QVector>
#include <QPair>

//QGIS-uncludes
#include "qgsmemorygraph.h"
#include "qgsgraphanalyzer.h"

void QgsGraphAnalyzer::shortestpath( const QgsGraph* source, int startPointIdx, int criterionNum, const QVector<int>& destPointCost, QVector<double>& cost, QgsMemoryGraph* treeResult )
{

  // QMultiMap< cost, vertexIdx > not_begin
  // I use it and not create any struct or class.
  QMultiMap< double, int > not_begin;
  QMultiMap< double, int >::iterator it;

  // QVector< QPair< cost, arc id > result
  QVector< QPair< double, int > > result;

  result.reserve( source->vertexCount() );
  int i;
  for ( i = 0; i < source->vertexCount(); ++i )
  {
    result.push_back( QPair<double, int> ( std::numeric_limits<double>::infinity() , i ) );
  }
  result[ startPointIdx ] = QPair<double, int> ( 0.0, -1 );

  not_begin.insert( 0.0, startPointIdx );

  // begin Dijkstra algorithm
  while ( !not_begin.empty() )
  {
    it = not_begin.begin();
    double curCost = it.key();
    int curVertex = it.value();
    not_begin.erase( it );

    // edge index list
    QgsGraphArcIdList l = source->vertex( curVertex ).mOutArc;
    QgsGraphArcIdList::iterator arcIt;
    for ( arcIt = l.begin(); arcIt != l.end(); ++arcIt )
    {
      const QgsGraphArc arc = source->arc( *arcIt );
      double cost = arc.mProperties[ criterionNum ].toDouble() + curCost;

      if ( cost < result[ arc.mIn ].first )
      {
        result[ arc.mIn ] = QPair< double, int >( cost, *arcIt );
        not_begin.insert( cost, arc.mIn );
      }
    }
  }

  // fill shortestpath tree
  if ( treeResult != NULL )
  {
    // sourceVertexIdx2resultVertexIdx
    QVector<int> source2result( result.size(), -1 );

    for ( i = 0; i < source->vertexCount(); ++i )
    {
      if ( result[ i ].first < std::numeric_limits<double>::infinity() )
      {
        source2result[ i ] = treeResult->addVertex( source->vertex( i ).mCoordinate );
      }
    }
    for ( i = 0; i < source->vertexCount(); ++i )
    {
      if ( result[ i ].first < std::numeric_limits<double>::infinity() && result[i].second != -1 )
      {
        const QgsGraphArc& arc = source->arc( result[i].second );

        treeResult->addArc( source2result[ arc.mOut ], source2result[ i ],
                            arc.mProperties );
      }
    }
  }

  // fill shortestpath's costs
  for ( i = 0; i < destPointCost.size(); ++i )
  {
    cost[i] = result[ destPointCost[i] ].first;
  }
}

QgsGraph* QgsGraphAnalyzer::shortestTree( const QgsGraph* source, int startVertexIdx, int criterionNum )
{
  QgsMemoryGraph *g = new QgsMemoryGraph;
  QVector<int> v;
  QVector<double> vv;
  QgsGraphAnalyzer::shortestpath( source, startVertexIdx, criterionNum, v, vv, g );

  return g;
}
