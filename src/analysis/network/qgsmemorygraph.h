/***************************************************************************
  qgsmemorygraph.h
  --------------------------------------
  Date                 : 2011-12-12
  Copyright            : (C) 2010 by Yakushev Sergey
  Email                : YakushevS <at> list.ru
****************************************************************************
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
*                                                                          *
***************************************************************************/

/*
 * \file qgsmemorygraph.h
 */
#ifndef QGSMEMORYGRAPH_H
#define QGSMEMORYGRAPH_H

#include "qgsgraph.h"

class ANALYSIS_EXPORT QgsMemoryGraph : public QgsGraph
{

  public:
    int vertexCount() const;

    const QgsGraphVertex vertex( int idx ) const;

    int arcCount() const;

    const QgsGraphArc arc( int idx ) const;

    int findVertex( const QgsPoint &pt ) const;

    // begin graph constructing methods
    /**
     * add vertex to a grap
     */
    int addVertex( const QgsPoint& pt );

    /**
     * add edge to a graph
     */
    int addArc( int outVertexIdx, int inVertexIdx, const QVector< QVariant >& properties );
  
  private:
    QVector<QgsGraphVertex> mGraphVertexes;

    QVector<QgsGraphArc> mGraphArc;
};
#endif
