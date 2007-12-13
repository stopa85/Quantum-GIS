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

#include <QBrush>
#include <QList>
#include <QPen>
#include <QString>
#include "qgsdiagramcategory.h"
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
  /**Creates a diagram for a feature and a given size (that is usually determined by QgsDiagramRenderer. The calling method takes ownership of the generated image*/
  QImage* createDiagram(int size, const QgsFeature& f) const;
  /**Creates items to show in the legend*/
  int createLegendContent(int size, QString value, QMap<QString, QImage*>& items) const{return 1;} //soon
  /**Gets the diagram width and height for a given size. Considers different width, height values and the maximum width of the drawing pen.
   */
  int getDiagramDimensions(int size, const QgsFeature& f, int& width, int& height) const;
  /**Writes properties to an xml document*/
  bool writeXML(QDomNode& overlay_node, QDomDocument& doc) const;
  /**Returns the property described by the size (e.g. diameter or height). This can be important to
   know if e.g. size has to be calculated proportional to pie area*/
  QgsDiagramFactory::SizeType sizeType() const;

  //setters and getters for diagram type
  QString diagramType() const {return mDiagramType;}
  void setDiagramType(const QString& name){mDiagramType = name;}
  
  QList<QgsDiagramCategory> categories() const {return mCategories;}
  /**Adds a new category (attribute together with symbolisation)*/
  void addCategory(QgsDiagramCategory c);

  //setters and getters for scaling attribute
  QList<int> scalingAttributes() const {return mScalingAttributes;}
  void setScalingAttributes(const QList<int>& att){mScalingAttributes = att;}
  
  /**Returns the supported well known names in a list*/
  static void supportedWellKnownNames(std::list<QString>& names);

 private:
  /**List of scaling attribute indexes (the values are summed up to 
     receive the scaling value)*/
  QList<int> mScalingAttributes;
  /**Well known diagram name (e.g. pie, bar, line)*/
  QString mDiagramType;
  /**List of attributes that are represented as slices, pillars, etc.*/
  QList<QgsDiagramCategory> mCategories;
  /**width of one bar (default 20 pixel)*/
  int mBarWidth;
  /**Maximum line width. Needs to be considered for the size of the generated image*/
  int mMaximumPenWidth;
  /**Maximum Gap. Needs to be considered for the size of the generated image*/
  int mMaximumGap;

  /**Creates a pie image. The calling method takes ownership of the image*/
  QImage* createPieChart(int size, const QgsAttributeMap& dataValues) const;
  /**Creates a bar image. The calling method takes ownership of the image*/
  QImage* createBarChart(int size, const QgsAttributeMap& dataValues) const;
  /**Calculates the maximum height of the bar chart (based on size for the 
   scaling attribute)*/
  int getHeightBarChart(int size, const QgsAttributeMap& featureAttributes) const;
  /**Calculates the value to pixel ratio for the bar chart (based on the size \
   of the scaling attribute)
  @return the ratio or -1 in case of error*/
  double pixelValueRatioBarChart(int size, const QgsAttributeMap& featureAttributes) const;
  /**Calculates the x- and y-offsets for pie gaps
     @param gap the gap (in pixels)
     @param angle azimut of pie slice (mean azimut of both pie angles), 0 - 5760 (degrees * 16) with 0 in 3 o'clock position and counterclockwise direction
     @param xOffset out offset (in pixel) for x-direction
     @param yOffset out offset (in pixel) for y-direction
  @return 0 in case of success*/
  int gapOffsetsForPieSlice(int gap, int angle, int& xOffset, int& yOffset) const;
};

#endif
