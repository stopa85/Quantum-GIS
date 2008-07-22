/***************************************************************************
                            qgsticksdownscalebarstyle.h
                            -----------------------------
    begin                : July 2008
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

#ifndef QGSTICKSDOWNSCALEBARSTYLE_H
#define QGSTICKSDOWNSCALEBARSTYLE_H

#include "qgsscalebarstyle.h"

/**Scale bar style that draws a line and ticks down*/
class QgsTicksDownScaleBarStyle: public QgsScaleBarStyle
{
  public:
  QgsTicksDownScaleBarStyle(const QgsComposerScaleBar* bar);
  ~QgsTicksDownScaleBarStyle();

  QString name() const;

  void draw(QPainter* p) const;

 private:
  QgsTicksDownScaleBarStyle(); //forbidden
}

#endif 
