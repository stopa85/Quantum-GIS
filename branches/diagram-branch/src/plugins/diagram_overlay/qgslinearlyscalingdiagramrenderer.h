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
#include "qgsdiagramitem.h"

/**This renderer scales the size of the diagram linearly between minimum/ maximum values of an attribute*/

class QgsLinearlyScalingDiagramRenderer: public QgsDiagramRenderer
{
 public:

  //Describes if the scaling is proportional to line (pie diameter) or area (pie area). Bar charts are always
  //proportional to line and area because the bar width is constant
  enum Proportion
    {
      LINE,
      AREA
    };

  QgsLinearlyScalingDiagramRenderer(const QString& name, const QgsAttributeList& att, const std::list<QColor>& c);
  ~QgsLinearlyScalingDiagramRenderer();
  /**Returns an diagram image for a geographic feature. The caller takes ownership of the QImage. Returns 0 in case of error*/
  QImage* renderDiagram(const QgsFeature& f) const;
  /**Returns only the diagram size for a feature*/
  int getDiagramSize(int& width, int& height, double& value, const QgsFeature& f) const;
  /**Sets value, height and width of lower bound*/
  void setLowerItem(const QgsDiagramItem& item){mLowerItem = item;}
  QgsDiagramItem lowerItem() const {return mLowerItem;}
  /**Sets value, height and width of upper bound*/
  void setUpperItem(const QgsDiagramItem& item){mUpperItem = item;}
  QgsDiagramItem upperItem() const {return mUpperItem;}
  void setProportion(QgsLinearlyScalingDiagramRenderer::Proportion p){mProportion = p;}
  QString rendererName() const {return "linearly scaling";}
  


  bool readXML(const QDomNode& rendererNode);
  bool writeXML(QDomNode& overlay_node, QDomDocument& doc) const;
  /**Creates a descriptive image for the legend together with a string describing the attribute 
     value for which this diagram is valid
   @return 0 in case of error*/
  QImage* getLegendImage(QString& legendString) const;

 private:
  /**Value, height, width of lower bound*/
  QgsDiagramItem mLowerItem;
  /**Value, height, width of upper bound*/
  QgsDiagramItem mUpperItem;
  /**Stores if scaling should be proportional to line or area*/
  QgsLinearlyScalingDiagramRenderer::Proportion mProportion;

  QgsLinearlyScalingDiagramRenderer(); //default constructor is forbidden
};

#endif
