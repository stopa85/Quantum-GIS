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

//QGIS includes
#include "qgsrasterdistancearcprop.h"

double QgsRasterDistanceArcProperter::property( double distance, const QVector< double >& pixel1, const QVector< double >& pixel2 ) const
{
  Q_UNUSED( pixel1 );
  Q_UNUSED( pixel2 );
  return distance;
}
