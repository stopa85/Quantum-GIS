/***************************************************************************
  qgsrasterlayerdirector.h
  --------------------------------------
  Date                 : 2011-10-12
  Copyright            : (C) 2010 by Yakushev Sergey
  Email                : YakushevS <at> gmail.com
****************************************************************************
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
*                                                                          *
***************************************************************************/
#ifndef QGSRASTERGRAPHDIRECTORH
#define QGSRASTERGRAPHDIRECTORH

//QT4 includes
#include <QObject>
#include <QVector>
#include <QList>

//QGIS includes
#include "qgsgraphdirector.h"
#include <qgspoint.h>

//forward declarations
class QgsGraphBuilderInterface;
class QgsRasterLayer;

/**
 * \ingroup networkanalysis
 * \class QgsRasterLayerDirector
 * \brief Determine making the graph form vector layer. QgsVectorGraphDirector class can use 
 * QgsArcProperter class as propety strategy. QgsGraphBuilder and QgsGraphDirector 
 * is a builder patter.
 */
class ANALYSIS_EXPORT QgsRasterLayerDirector : public QgsGraphDirector
{
    Q_OBJECT

  public:
    QgsRasterLayerDirector( QgsRasterLayer *layer );
    virtual ~QgsRasterLayerDirector();
    /**
     * Make a graph using RgGraphBuilder
     *
     * @param builder   The graph builder
     *
     * @param additionalPoints  Vector of points that must be tied to the graph
     *
     * @param tiedPoints  Vector of tied points
     *
     * @note if tiedPoints[i]==QgsPoint(0.0,0.0) then tied failed.
     */
    virtual void makeGraph( QgsGraphBuilderInterface *builder,
                            const QVector< QgsPoint > &additionalPoints,
                            QVector< QgsPoint > &tiedPoints ) const;

/*    void addProperter( QgsArcProperter* prop )
    {
      mProperterList.push_back( prop );
    }*/

    /**
     * return Director name
     */
    virtual QString name() const;

  private:
    QgsRasterLayer *mLayer;
};
#endif //QGSRASTERGRAPHDIRECTORH
