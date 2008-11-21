/***************************************************************************
                         qgssvgdiagramfactory.h  -  description
                         ----------------------
    begin                : November 2007
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

#ifndef QGSSVGDIAGRAMFACTORY_H
#define QGSSVGDIAGRAMFACTORY_H

#include "qgsdiagramfactory.h"
#include <QSvgRenderer>

class QgsSVGDiagramFactory: public QgsDiagramFactory
{
 public:
  QgsSVGDiagramFactory();
  ~QgsSVGDiagramFactory();

  /**Creates the diagram image for a feature in order to be placed on the map*/
  QImage* createDiagram(int size, const QgsFeature& f) const;
  /**Creates the text/images for the legend items. The caller takes ownership of the generated \
   image objects. 
  @param size diagram size that should be represented in the legend
  @param value diagram value that should be represented in the legend
  @param items generated items
  @return 0 in case of success*/
  int createLegendContent(int size, QString value, QMap<QString, QImage*>& items) const {return 1;} //later...
  
  /**Gets the width and height of the diagram image for a feature. This depends on \
   the diagram type and is not necessary equal with the size.
  @param size diagram size calculated by diagram renderer
  @param f reference to the feature associated with the diagram
  @param width out: the width of the diagram image
  @param height out: the height of the diagram image*/
  int getDiagramDimensions(int size, const QgsFeature& f, int& width, int& height) const;

  bool writeXML(QDomNode& overlay_node, QDomDocument& doc) const {return false;} //later...

  /**Sets the SVG data to be rendered.
   @return true in case of success*/
  bool setSVGData(const QByteArray& data);

 private:
  mutable QSvgRenderer mRenderer;
};

#endif
