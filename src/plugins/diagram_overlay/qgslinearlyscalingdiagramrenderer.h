/***************************************************************************
                         qgslinearlyscalingdiagramrenderer.h  -  description
                         ----------------------------------
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

#ifndef QGSLINEARLYSCALINGDIAGRAMRENDERER_H
#define QGSLINEARLYSCALINGDIAGRAMRENDERER_H

#include "qgsdiagramrenderer.h"
#include <QString>

/**This renderer scales the size of the diagram linearly between minimum/ maximum values of an attribute*/

class QgsLinearlyScalingDiagramRenderer: public QgsDiagramRenderer
{
 public:

  //describes the type of interpolation
  enum InterpolationType
    {
      DISCRETE,
      LINEAR
    };

  QgsLinearlyScalingDiagramRenderer(const QList<int> classificationAttributes);
  ~QgsLinearlyScalingDiagramRenderer();
  /**Returns an diagram image for a geographic feature. The caller takes ownership of the QImage. Returns 0 in case of error*/
  QImage* renderDiagram(const QgsFeature& f) const;
  /**Returns only the diagram size for a feature*/
  int getDiagramSize(int& width, int& height, const QgsFeature& f) const;
  /**Sets the items for interpolation. The values of the items must be in ascending order*/
  void setDiagramItems(const QList<QgsDiagramItem>& items) {mItems = items;}
  void setInterpolationType(InterpolationType t){mInterpolationType = t;}
  /**Returns the interpolation items*/
  QList<QgsDiagramItem> diagramItems() const {return mItems;}
  QString rendererName() const {return "linearly scaling";}
  

  bool readXML(const QDomNode& rendererNode);
  bool writeXML(QDomNode& overlay_node, QDomDocument& doc) const;
  
  int createLegendContent(QMap<QString, QImage*> items) const;

 protected:
  /**Describes the type of interpolation (linear by default)*/
  InterpolationType mInterpolationType;
  /**Interpolation points for size*/
  QList<QgsDiagramItem> mItems;

  /**Gets diagram size
   @return 0 in case of success*/
  int calculateDiagramSize(const QgsFeature& f, int& size) const;
  /**Does (linear or discrete) interpolation*/
  int interpolateSize(double value, double lowerValue, double upperValue, int lowerSize, \
		      int upperSize) const;

 private:

  QgsLinearlyScalingDiagramRenderer(); //default constructor is forbidden
};

#endif
