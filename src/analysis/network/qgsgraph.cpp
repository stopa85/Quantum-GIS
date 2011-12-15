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

/**
 * \file qgsgraph.cpp
 * \brief implementation QgsGraph, QgsGraphVertex, QgsGraphArc
 */

#include "qgsgraph.h"

QgsGraphArc::QgsGraphArc()
{

}

QVariant QgsGraphArc::property( int i ) const
{
  return mProperties[ i ];
}

QVector< QVariant > QgsGraphArc::properties() const
{
  return mProperties;
}

int QgsGraphArc::inVertex() const
{
  return mIn;
}

int QgsGraphArc::outVertex() const
{
  return mOut;
}

QgsGraphVertex::QgsGraphVertex( const QgsPoint& point )
    : mCoordinate( point )
{

}

QgsGraphArcIdList QgsGraphVertex::outArc() const
{
  return mOutArc;
}

QgsGraphArcIdList QgsGraphVertex::inArc() const
{
  return mInArc;
}

QgsPoint QgsGraphVertex::point() const
{
  return mCoordinate;
}
