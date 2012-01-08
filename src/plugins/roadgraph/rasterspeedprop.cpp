/***************************************************************************
 *   Copyright (C) 2009 by Sergey Yakushev                                 *
 *   yakushevs@list.ru                                                     *
 *                                                                         *
 *   This is file implements Units classes                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "rasterspeedprop.h"

RgRasterSpeedProperter::RgRasterSpeedProperter( int bandNo, double defaultValue, double toMetricFactor )
{
  mBandNo = bandNo;
  mDefaultValue = defaultValue;
  mToMetricFactor = toMetricFactor;
}

double RgRasterSpeedProperter::property( double distance, const QVector< double >& pixel1, const QVector< double >& pixel2 ) const
{

  if ( mBandNo < 0 )
    return distance / ( mDefaultValue*mToMetricFactor );

  double val = distance / ( ( pixel1[mBandNo] + pixel2[mBandNo] ) * mToMetricFactor / 2 );
  if ( val <= 0.0 )
    return distance / ( mDefaultValue*mToMetricFactor );

  return val;
}
