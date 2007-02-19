/***************************************************************************
                         qgsdiagramoverlay.h  -  description
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

#ifndef QGSDIAGRAMOVERLAY_H
#define QGSDIAGRAMOVERLAY_H

#include "qgsvectoroverlay.h"

class QgsDiagramRenderer;

/**An overlay class that places diagrams on a vectorlayer*/
class QgsDiagramOverlay: public QgsVectorOverlay
{
 public:
  QgsDiagramOverlay(QgsVectorLayer* vl);
  ~QgsDiagramOverlay();
  void createOverlayObjects(const QgsRect& viewExtent);
  void drawOverlayObjects(QPainter * p, const QgsRect& viewExtent, QgsMapToPixel * cXf, QgsCoordinateTransform* ct) const;

  //setters and getters
  QString name() const{return "diagram";}
  void setDiagramRenderer(QgsDiagramRenderer* r);
  const QgsDiagramRenderer* diagramRenderer() const {return mDiagramRenderer;}
  void setDisplayFlag(bool display=true){mDisplayFlag = display;}
  bool displayFlag() const {return mDisplayFlag;}

  bool writeXML(QDomNode& layer_node, QDomDocument& doc) const;
  
protected:
  int getOverlayObjectSize(int& width, int& height, double value, const QgsFeature& f) const;
  
 private:
  /**Does the classification and manages the diagram generation*/
  QgsDiagramRenderer* mDiagramRenderer;
  /**Flag if the overlay should be displayed or not*/
  bool mDisplayFlag;
};

#endif
