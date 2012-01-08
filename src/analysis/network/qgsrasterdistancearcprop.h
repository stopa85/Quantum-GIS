/***************************************************************************
  qgsrasterdistancearcprop.h
  --------------------------------------
  Date                 : 2011-12-05
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

#ifndef QGSRASTERARCDISTANCEPROPERTERH
#define QGSRASTERARCDISTANCEPROPERTERH

// QT4 includes
#include <QVariant>
#include <QVector>

// QGIS includes
#include <qgsrasterarcproperter.h>

class ANALYSIS_EXPORT QgsRasterDistanceArcProperter : public QgsRasterArcProperter
{
  public:
    virtual double property( double distance, const QVector< double >& pixel1, const QVector< double >& pixel2 ) const;
};
#endif //QGSEDGEDISTANCEPROPERTYH
