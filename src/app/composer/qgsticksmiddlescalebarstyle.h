/***************************************************************************
                            qgsticksmiddlescalebarstyle.h
                            -----------------------------
    begin                : June 2008
    copyright            : (C) 2008 by Marco Hugentobler
    email                : marco.hugentobler@karto.baug.ethz.ch
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSTICKSMIDDLESCALEBARSTYLE_H
#define QGSTICKSMIDDLESCALEBARSTYLE_H 

#include "qgsscalebarstyle.h"

class QgsTicksMiddleScaleBarStyle: public QgsScaleBarStyle
{
  public:
  enum TickPosition
    {
      UP,
      DOWN,
      MIDDLE
    };

  QgsTicksMiddleScaleBarStyle(const QgsComposerScaleBar* bar);
  ~QgsTicksMiddleScaleBarStyle();

  QString name() const;

  void draw(QPainter* p) const;

  void setTickPosition(TickPosition p){mTickPosition = p;}

 private:
  QgsTicksMiddleScaleBarStyle(); //forbidden

  TickPosition mTickPosition;
};

#endif 
