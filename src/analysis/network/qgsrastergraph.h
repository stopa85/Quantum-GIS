/***************************************************************************
  qgsrastergraph.h
  --------------------------------------
  Date                 : 2011-12-13
  Copyright            : (C) 2011 by Yakushev Sergey
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
#ifndef QGSRASTERGRAPH_H
#define QGSRASTERGRAPH_H

#include <QVector>

#include <qgsrectangle.h>
#include <qgscoordinatetransform.h>
#include <qgsdistancearea.h>

#include "qgsgraph.h"

class QgsRasterLayer;

class ANALYSIS_EXPORT QgsRasterGraph : public QgsGraph
{

  public:
    QgsRasterGraph( QgsRasterLayer *layer, bool coordinateTansformEnbled, const QgsCoordinateReferenceSystem& destCrs );
    ~QgsRasterGraph();

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
    QVector< QVector<double> > mData;

    QVector< int > mDataType;

    QVector< int > mDataTypeSize;

    int mWidth;
    int mHeight;

    QgsRectangle mExtent;

    QgsCoordinateTransform  mCoordinateTransform;
     
    QVector < double > mCacheDistance;
};
#endif
