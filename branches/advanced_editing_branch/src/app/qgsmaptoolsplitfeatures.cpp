/***************************************************************************
    qgsmaptoolsplitfeatures.cpp
    ---------------------------
    begin                : August 2007
    copyright            : (C) 2007 by Marco Hugentobler
    email                : marco.hugentobler@karto.baug.ethz.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* $Id$ */

#include "qgsmaptoolsplitfeatures.h"
#include "qgsmapcanvas.h"
#include "qgsrubberband.h"
#include "qgsvectorlayer.h"
#include <QMessageBox>

QgsMapToolSplitFeatures::QgsMapToolSplitFeatures(QgsMapCanvas* canvas): QgsMapToolCapture(canvas, QgsMapToolCapture::CaptureLine)
{

}

QgsMapToolSplitFeatures::~QgsMapToolSplitFeatures()
{

}

void QgsMapToolSplitFeatures::canvasReleaseEvent(QMouseEvent * e)
{
  //check if we operate on a vector layer
  QgsVectorLayer *vlayer = dynamic_cast <QgsVectorLayer*>(mCanvas->currentLayer());
  
  if (!vlayer)
    {
      QMessageBox::information(0, QObject::tr("Not a vector layer"), \
			       QObject::tr("The current layer is not a vector layer"));
      return;
    }

  if (!vlayer->isEditable())
    {
      QMessageBox::information(0, QObject::tr("Layer not editable"),
			       QObject::tr("Cannot edit the vector layer. To make it editable, go to the file item "
					   "of the layer, right click and check 'Allow Editing'."));
      return;
    }
  
  //add point to list and to rubber band
  int error = addVertex(e->pos());
  if(error == 1)
    {
      //current layer is not a vector layer
      return;
    }
  else if (error == 2)
    {
      //problem with coordinate transformation
      QMessageBox::information(0, QObject::tr("Coordinate transform error"), \
			       QObject::tr("Cannot transform the point to the layers coordinate system"));
      return;
    }
  
  if (e->button() == Qt::LeftButton)
    {
      mCapturing = TRUE;
    }
  else if (e->button() == Qt::RightButton)
    {
      mCapturing = FALSE;	  
      delete mRubberBand;
      mRubberBand = 0;

      vlayer->splitFeatures(mCaptureList);
      
      mCaptureList.clear();
      mCanvas->refresh();
    }
}
