/***************************************************************************
                         qgsbardiagramfactory.h  -  description
                         ----------------------
    begin                : December 2007
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

#ifndef QGSBARDIAGRAMFACTORY_H
#define QGSBARDIAGRAMFACTORY_H

#include "qgswkndiagramfactory.h"

/**A class that creates bar diagrams*/
class QgsBarDiagramFactory: public QgsWKNDiagramFactory
{
 public:
  QgsBarDiagramFactory();
  ~QgsBarDiagramFactory();

  /**Creates a diagram for a feature and a given size (that is usually determined by QgsDiagramRenderer. The calling method takes ownership of the generated image*/
  QImage* createDiagram(int size, const QgsFeature& f) const;

  /**Creates items to show in the legend*/
  int createLegendContent(int size, QString value, QMap<QString, QImage*>& items) const{return 1;} //soon

  /**Gets the diagram width and height for a given size. Considers different width, height values and the maximum width of the drawing pen.
   */
  int getDiagramDimensions(int size, const QgsFeature& f, int& width, int& height) const;

   /**Returns the property described by the size (e.g. diameter or height). This can be important to
   know if e.g. size has to be calculated proportional to pie area*/
  QgsDiagramFactory::SizeType sizeType() const {return QgsDiagramFactory::HEIGHT;}

 private: 

  /**width of one bar (default 20 pixel)*/
  int mBarWidth;
  

  /**Calculates the maximum height of the bar chart (based on size for the 
   scaling attribute)*/
  int getHeightBarChart(int size, const QgsAttributeMap& featureAttributes) const;

  /**Calculates the value to pixel ratio for the bar chart (based on the size \
   of the scaling attribute)
  @return the ratio or -1 in case of error*/
  double pixelValueRatioBarChart(int size, const QgsAttributeMap& featureAttributes) const;
};

#endif
