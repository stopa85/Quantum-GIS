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

#include <QMap>
#include <QString>

class QgsFeature;
class QDomDocument;
class QDomNode;
class QImage;

/**Interface for classes that create diagrams*/
class QgsDiagramFactory
{
 public:
  /**Describes if the size describes one dimensional height (e.g. bar chart), \
     a diameter (e.g. piechart) or a squareside. This may be taken into consideration \
     from a renderer for interpolation*/
  enum SizeType
    {
      HEIGHT,
      DIAMETER,
      SQUARESIDE,
    };
  
  /**Creates the diagram image for a feature in order to be placed on the map*/
  virtual QImage* createDiagram(int size, const QgsFeature& f) const = 0;
  /**Creates the text/images for the legend items. The caller takes ownership of the generated \
   image objects. 
  @param size diagram size that should be represented in the legend
  @param value diagram value that should be represented in the legend
  @param items generated items
  @return 0 in case of success*/
  virtual int createLegendContent(int size, QString value, QMap<QString, QImage*>& items) const = 0; 
  /**Gets the width and height of the diagram image for a feature. This depends on \
   the diagram type and is not necessary equal with the size.
  @param size diagram size calculated by diagram renderer
  @param f reference to the feature associated with the diagram
  @param width out: the width of the diagram image
  @param height out: the height of the diagram image*/
  virtual int getDiagramDimensions(int size, const QgsFeature& f, int& width, int& height) const = 0;
  virtual bool writeXML(QDomNode& overlay_node, QDomDocument& doc) const = 0;
  virtual ~QgsDiagramFactory(){}
  /**Default is one dimensional scaling*/
  virtual QgsDiagramFactory::SizeType sizeType() const {return QgsDiagramFactory::HEIGHT;}
};

#endif
