/***************************************************************************
  qgsedgeproperter.h
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

#ifndef QGSRASTERARCPROPERTERH
#define QGSRASTERARCPROPERTERH

// QT4 includes
#include <QVector>
#include <QVariant>

// QGIS includes
#include <qgsfeature.h>
#include <qgslabel.h>

/**
 * \ingroup networkanalysis
 * \class QgsEdgeProperter
 * \brief QgsEdgeProperter is a strategy pattern.
 * You can use it for customize arc property. For example look at QgsDistanceArcProperter or src/plugins/roadgraph/speedproperter.h
 */
class ANALYSIS_EXPORT QgsRasterArcProperter
{
  public:
    /**
     * default constructor
     */
    QgsRasterArcProperter()
    { }

    /**
     * calculate and return adge property
     */
    virtual QVariant property( double distance, const QVector<QVariant>& pixel1, const QVector<QVariant>& pixel2 ) const
    {
      Q_UNUSED( distance );
      Q_UNUSED( pixel1 );
      Q_UNUSED( pixel2 );
      return QVariant();
    }
};
#endif //QGSRASTERARCPROPERTERH
