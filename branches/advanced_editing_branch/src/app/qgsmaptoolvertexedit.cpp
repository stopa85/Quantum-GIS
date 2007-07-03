/***************************************************************************
    qgsmaptoolvertexedit.cpp  - tool for adding, moving, deleting vertices
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

#include "qgsmaptoolvertexedit.h"
#include "qgsmapcanvas.h"
#include "qgsmaprender.h"
#include "qgsvertexmarker.h"
#include "qgsrubberband.h"
#include "qgsvectorlayer.h"
#include "qgsvectordataprovider.h"
#include "qgsmaptopixel.h"
#include "qgsproject.h"
#include "qgscursors.h"
#include <QCursor>
#include <QMessageBox>
#include <QPixmap>
#include <QSettings>

QgsMapToolVertexEdit::QgsMapToolVertexEdit(QgsMapCanvas* canvas): QgsMapTool(canvas)
{
  mSnapper.setMapCanvas(canvas);
}
  
QgsMapToolVertexEdit::~QgsMapToolVertexEdit()
{

}

QgsRubberBand* QgsMapToolVertexEdit::createRubberBand()
{
  QSettings settings;
  QgsRubberBand* rb = new QgsRubberBand(mCanvas, FALSE);
  QColor color( settings.value("/qgis/digitizing/line_color_red", 255).toInt(),
		settings.value("/qgis/digitizing/line_color_green", 0).toInt(),
		settings.value("/qgis/digitizing/line_color_blue", 0).toInt());
  rb->setColor(color);
  rb->setWidth(settings.value("/qgis/digitizing/line_width", 1).toInt());
  return rb;
}
