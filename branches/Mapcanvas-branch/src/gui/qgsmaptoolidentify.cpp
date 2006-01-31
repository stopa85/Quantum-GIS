/***************************************************************************
    qgsmaptoolidentify.cpp  -  map tool for identifying features
    ---------------------
    begin                : January 2006
    copyright            : (C) 2006 by Martin Dobias
    email                : wonder.sk at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* $Id$ */

#include "qgsmaptoolidentify.h"
#include "qgsmapcanvas.h"
#include "qgsmaplayer.h"
#include "qgsmaptopixel.h"
#include <QSettings>
#include <QMessageBox>

QgsMapToolIdentify::QgsMapToolIdentify(QgsMapCanvas* canvas)
  : QgsMapTool(canvas)
{
}
    
void QgsMapToolIdentify::canvasMoveEvent(QMouseEvent * e)
{
}
  
void QgsMapToolIdentify::canvasPressEvent(QMouseEvent * e)
{
}

void QgsMapToolIdentify::canvasReleaseEvent(QMouseEvent * e)
{
  QgsMapLayer* layer = mCanvas->currentLayer();

  // call identify method for selected layer

  if (layer)
  {
    // load identify radius from settings
    QSettings settings;
    int identifyValue = settings.readNumEntry("/qgis/map/identifyRadius", QGis::DEFAULT_IDENTIFY_RADIUS);

    // create the search rectangle
    double searchRadius = mCanvas->extent().width() * (identifyValue/1000.0);
    
    // convert screen coordinates to map coordinates
    QgsPoint idPoint = mCanvas->getCoordinateTransform()->toMapCoordinates(e->x(), e->y());
    
    QgsRect search;
    search.setXmin(idPoint.x() - searchRadius);
    search.setXmax(idPoint.x() + searchRadius);
    search.setYmin(idPoint.y() - searchRadius);
    search.setYmax(idPoint.y() + searchRadius);

    // TODO: move identification here from QgsVectorLayer / QgsRasterLayer
    layer->identify(&search);

  }
  else
  {
    QMessageBox::warning(mCanvas,
      QObject::tr("No active layer"),
      QObject::tr("To identify features, you must choose an layer active by clicking on its name in the legend"));
  }

  
}
