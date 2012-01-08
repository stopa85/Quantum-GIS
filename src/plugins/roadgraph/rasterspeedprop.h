/***************************************************************************
 *   Copyright (C) 2011 by Sergey Yakushev                                 *
 *   yakushevs@list.ru                                                     *
 *                                                                         *
 *   This is file define vrp plugins time, distance and speed units        *
 *   classes                                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ROADGRAPH_RASTERSPEEDPROPERTER_H
#define ROADGRAPH_RASTERSPEEDPROPERTER_H

#include <qgsrasterarcproperter.h>

class RgRasterSpeedProperter : public QgsRasterArcProperter
{
  public:
    RgRasterSpeedProperter( int bandNo, double defaultValue, double toMetricFactor );

    double property( double distance, const QVector< double >& pixel1, const QVector< double >& pixel2 ) const;

  private:
    int mBandNo;
    double mDefaultValue;
    double mToMetricFactor;

};

#endif
