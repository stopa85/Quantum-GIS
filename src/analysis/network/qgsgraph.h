/***************************************************************************
  graph.h
  --------------------------------------
  Date                 : 2011-04-01
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
 * This file describes the built-in QGIS classes modeling a mathematical graph.
 * Vertex is identified by its geographic coordinates (but you can add two vertex
 * with unique coordinate), no additional properties it can not be assigned.
 * Count the number of properties not limited along the arc. Graph may
 * be have incidence arcs.
 *
 * \file qgsgraph.h
 */

#ifndef QGSGRAPHH
#define QGSGRAPHH

// QT4 includes
#include <QList>
#include <QVector>
#include <QVariant>

// QGIS includes
#include "qgspoint.h"

class QgsGraphVertex;

/**
 * \ingroup networkanalysis
 * \class QgsGraphArc
 * \brief This class implement a graph edge
 */
struct ANALYSIS_EXPORT QgsGraphArc
{
    QVector< QVariant > mProperties;
    int mOut;
    int mIn;
};

typedef QList< int > QgsGraphArcIdList;

/**
 * \ingroup networkanalysis
 * \struct QgsGraphVertex
 * \brief This class implement a graph vertex
 */
struct ANALYSIS_EXPORT QgsGraphVertex
{
  QgsPoint mCoordinate;
  QgsGraphArcIdList mOutArc;
  QgsGraphArcIdList mInArc;
};

/**
 * \ingroup networkanalysis
 * \class QgsGraph
 * \brief Mathematics graph representation. Read access interface
 */
class ANALYSIS_EXPORT QgsGraph
{
  public:
    QgsGraph() 
    {   }

    virtual ~QgsGraph() 
    {   }

    /**
     * retrun vertex count
     */
    virtual int vertexCount() const
    { return 0;	}

    /**
     * return vertex at index
     */
    virtual const QgsGraphVertex vertex( int idx ) const
    { 
      Q_UNUSED( idx );
      return QgsGraphVertex(); 
    }

    /**
      * retrun edge count
      */
    virtual int arcCount() const
    { return 0; }

    /**
     * retrun edge at index
     */
    virtual const QgsGraphArc arc( int idx ) const
    { 
      Q_UNUSED( idx );
      return QgsGraphArc(); 
    }

    /**
     * find vertex by point
     * \return vertex index
     */
    virtual int findVertex( const QgsPoint& pt ) const
    { 
      Q_UNUSED( pt );
      return 0; 
    }

};

#endif //QGSGRAPHH
