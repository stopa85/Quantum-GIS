/***************************************************************************
                         qgsdiagramfactory.h  -  description
                         -------------------
    begin                : September 2007
    copyright            : (C) 2007 by Marco Hugentobler
    email                : marco dot hugentobler at karto dot baug dot ethz dot ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSDIAGRAMFACTORY_H
#define QGSDIAGRAMFACTORY_H

class QgsFeature;
class QImage;

/**Interface for classes that create diagrams*/
class QgsDiagramFactory
{
 public:
  virtual QImage* createDiagram(int width, int height, const QgsFeature& f) const = 0;
  virtual ~QgsDiagramFactory(){}
};

#endif
