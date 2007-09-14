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

#include <QMap>
#include <QString>

class QgsDiagramFactory;
class QgsFeature;
class QDomDocument;
class QDomNode;
class QImage;

//structure that describes a renderer entry
struct QgsDiagramItem
{
  double value;
  int size;
};

/**An interface class for diagram renderer. The main method is 'renderDiagram', which returns the diagram image for a GIS feature. The renderer has a set of classification attributes, a set of colors for these attributes and a diagram type*/
class QgsDiagramRenderer
{
 public:
  QgsDiagramRenderer(int classificationAttribute);
  virtual ~QgsDiagramRenderer();
  /**Returns a diagram image for a feature.*/
  virtual QImage* renderDiagram(const QgsFeature& f) const = 0;
  /**Returns only the size of the diagram.
     @param width the width of the diagram
     @param height the height of the diagram
     @param value the attribute value used for the size calculation
     @return 0 in case of success*/
  virtual int getDiagramSize(int& width, int& height, const QgsFeature& f) const = 0;
  //setters and getters
  virtual QString rendererName() const = 0;
  QgsDiagramFactory* factory() const {return mFactory;}
  /**Set a (properly configured) factory class. Takes ownership of the factory object*/
  void setFactory(QgsDiagramFactory* f){mFactory = f;}
  void setClassificationAttribute(int attrNr){mClassificationAttribute = attrNr;}
  /**Reads the specific renderer settings from project file*/
  virtual bool readXML(const QDomNode& rendererNode) = 0;
  /**Saves settings to project file. Returns true in case of success*/
  virtual bool writeXML(QDomNode& overlay_node, QDomDocument& doc) const = 0;
  /**Creates pairs of strings / images for use in the legend
   @return 0 in case of success*/
  virtual int createLegendContent(QMap<QString, QImage*> items) const = 0; 

 private:
  QgsDiagramRenderer();

 protected:
  /**The object to generate the diagrams*/
  QgsDiagramFactory* mFactory;
  /**Attribute for the size of the diagram*/
  int mClassificationAttribute;
  /**Searches the value of the classification attribute
   @return 0 in case of success*/
  int classificationValue(const QgsFeature& f, double& value) const;
};

#endif
