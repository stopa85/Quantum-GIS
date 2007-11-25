/***************************************************************************
                         qgswkndiagramfactory.h  -  description
                         ----------------------
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


#ifndef QGSWKNDIAGRAMFACTORY_H
#define QGSWKNDIAGRAMFACTORY_H

#include <list>
#include <QColor>
#include <QString>
#include "qgsdiagramfactory.h"
#include "qgsvectorlayer.h" //for QgsAttributeList

class QImage;
class QgsDiagramItem;
class QgsFeature;

/**A class that renders diagrams for WellKnownNames diagram types
 (pie, bar)*/
class QgsWKNDiagramFactory: public QgsDiagramFactory
{
 public:
  QgsWKNDiagramFactory();
  ~QgsWKNDiagramFactory();
  /**Creates a diagram for a feature and a given (classification) item. The calling method takes ownership of the generated image*/
  QImage* createDiagram(int size, const QgsFeature& f) const;
  int createLegendContent(int size, QString value, QMap<QString, QImage*>& items) const{return 1;} //soon
  int getDiagramDimensions(int size, const QgsFeature& f, int& width, int& height) const;
  bool writeXML(QDomNode& overlay_node, QDomDocument& doc) const;

  QgsDiagramFactory::SizeType sizeType() const;

  //setters and getters for diagram type
  QString diagramType() const {return mDiagramType;}
  void setDiagramType(const QString& name){mDiagramType = name;}
  //setters and getters for attributes
  QgsAttributeList attributes() const {return mAttributes;}
  void setAttributes(const QgsAttributeList& att){mAttributes = att;}
  //setters and getters for color series
  std::list<QColor> colors() const {return mColorSeries;}
  void setColorSeries(const std::list<QColor>& c){mColorSeries = c;}
  //setters and getters for scaling attribute
  QList<int> scalingAttributes() const {return mScalingAttributes;}
  void setScalingAttributes(const QList<int>& att){mScalingAttributes = att;}
  
  /**Returns the supported well known names in a list*/
  static void supportedWellKnownNames(std::list<QString>& names);

 private:
  /**List of scaling attribute indexes (the values are summed up to 
     receive the scaling value)*/
  QList<int> mScalingAttributes;
  /**Indices of attributes for coloring*/
  QgsAttributeList mAttributes;
  /**Well known diagram name (e.g. pie, bar, line)*/
  QString mDiagramType;
  /**Diagram colors*/
  std::list<QColor> mColorSeries;
  int mBarWidth; //width of one bar (default 20 pixel)

  QImage* createPieChart(int size, const QgsAttributeMap& dataValues) const;
  QImage* createBarChart(int size, const QgsAttributeMap& dataValues) const;
  /**Calculates the maximum height of the bar chart (based on size for the 
   scaling attribute)*/
  int getHeightBarChart(int size, const QgsAttributeMap& featureAttributes) const;
  /**Calculates the value to pixel ratio for the bar chart (based on the size \
   of the scaling attribute)
  @return the ratio or -1 in case of error*/
  double pixelValueRatioBarChart(int size, const QgsAttributeMap& featureAttributes) const;
};

#endif
