/***************************************************************************
    qgsmaptoolpan.h  -  map tool for panning in map canvas
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

#include "qgsmaptoolpan.h"
#include "qgsmapcanvas.h"
#include "qgscursors.h"
#include <QBitmap>
#include <QCursor>


QgsMapToolPan::QgsMapToolPan(QgsMapCanvas* canvas)
  : QgsMapTool(canvas), mDragging(FALSE)
{
  // set cursor
  QBitmap panBmp(16, 16, pan_bits, true);
  QBitmap panBmpMask(16, 16, pan_mask_bits, true);
  mCanvas->setCursor(QCursor(panBmp, panBmpMask, 5, 5));
}

    
void QgsMapToolPan::canvasMoveEvent(QMouseEvent * e)
{
  if (e->state() == Qt::LeftButton)
  {
    // show the pmCanvas as the user drags the mouse
    mDragging = TRUE;
    mCanvas->panAction(e);
  }
}
  
    
void QgsMapToolPan::canvasPressEvent(QMouseEvent * e)
{
}
  
    
void QgsMapToolPan::canvasReleaseEvent(QMouseEvent * e)
{
  if (mDragging)
  {
    mDragging = TRUE;
    mCanvas->panActionEnd(e->pos());
  }
}
