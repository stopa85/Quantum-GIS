/***************************************************************************
                         qgsdiagramrenderer.h  -  description
                         --------------------
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


#ifndef QGSDIAGRAMRENDERER_H
#define QGSDIAGRAMRENDERER_H

#include <list>
#include <QColor>
#include "qgsdiagramfactory.h"
#include "qgsvectorlayer.h" //for QgsAttributeList

class QgsFeature;
class QImage;

/**An interface class for diagram renderer. The main method is 'renderDiagram', which returns the diagram image for a GIS feature. The renderer has a set of classification attributes, a set of colors for these attributes and a diagram type*/
class QgsDiagramRenderer
{
 public:
  QgsDiagramRenderer(const QString& name, const QgsAttributeList& att, const std::list<QColor>& c);
  virtual ~QgsDiagramRenderer();
  /**Returns a diagram image for a feature.*/
  virtual QImage* renderDiagram(const QgsFeature& f) const = 0;
  /**Returns only the size of the diagram.
     @param width the width of the diagram
     @param height the height of the diagram
     @param value the attribute value used for the size calculation
     @return 0 in case of success*/
  virtual int getDiagramSize(int& width, int& height, double& value, const QgsFeature& f) const = 0;
  //setters and getters
  virtual QString rendererName() const = 0;
  void setWellKnownName(const QString& wkn){mWellKnownName = wkn;}
  QString wellKnownName() const {return mWellKnownName;}
  void setAttributes(const QgsAttributeList& att){mAttributes = att;}
  QgsAttributeList attributes() const {return mAttributes;}
  void setColors(const std::list<QColor>& colors){mColors = colors;}
  std::list<QColor> colors() const {return mColors;}
  void setClassificationField(int index){mClassificationField = index;}
  int classificationField() const {return mClassificationField;}

 private:
  QgsDiagramRenderer(){} //default constructor is forbidden 

 protected:
  /**The object to generate the diagrams*/
  QgsDiagramFactory mFactory;
 /**The diagram type*/
  QString mWellKnownName;
  /**The number of attributes in the features*/
  QgsAttributeList mAttributes;
  /**Index of the classification attribute*/
  int mClassificationField;
  /**The color series. The positions correspond to the entries the attribute list*/
  std::list<QColor> mColors;
};

#endif
