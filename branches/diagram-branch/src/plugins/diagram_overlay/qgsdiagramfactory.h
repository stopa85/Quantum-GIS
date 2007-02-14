/***************************************************************************
                         qgsdiagramfactory.h  -  description
                         -------------------
    begin                : January 2007
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

#include <list>
#include <QColor>
#include <QString>
#include "qgsvectorlayer.h" //for QgsAttributeList

class QImage;
class QgsDiagramItem;
class QgsFeature;

/**A class that renders diagrams for map overlay*/
class QgsDiagramFactory
{
 public:
  QgsDiagramFactory();
  ~QgsDiagramFactory();
  /**Creates a diagram for a feature and a given (classification) item. The calling method takes ownership of the generated image*/
  QImage* createDiagram(int width, int height, const QgsFeature& f) const;

  void setDiagramType(const QString& name)
  {mDiagramType = name;}
  void setAttributes(const QgsAttributeList& att){mAttributes = att;}
  void setColorSeries(const std::list<QColor>& c)
  {mColorSeries = c;}

  /**Returns the supported well known names in a list*/
  static void supportedWellKnownNames(std::list<QString>& names);

 private:
  QgsAttributeList mAttributes;
  /**Well known diagram name (e.g. pie, bar, line)*/
  QString mDiagramType;
  /***/
  QString mCustomDiagramString;
  /**Diagram colors*/
  std::list<QColor> mColorSeries;

  QImage* createPieChart(int height, const std::list<double>& dataValues) const;
  QImage* createBarChart(int height, const std::list<double>& dataValues) const;
};

#endif
