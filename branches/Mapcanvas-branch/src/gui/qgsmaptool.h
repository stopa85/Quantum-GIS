/***************************************************************************
    qgsmaptool.h  -  base class for map canvas tools
    ----------------------
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

#ifndef QGSMAPTOOL_H
#define QGSMAPTOOL_H


class QgsMapCanvas;
class QMouseEvent;

class QgsMapTool
{
  public:
    virtual ~QgsMapTool() {}
    
    //! Mouse move event for overriding
    virtual void canvasMoveEvent(QMouseEvent * e) = 0;

    //! Mouse press event for overriding
    virtual void canvasPressEvent(QMouseEvent * e) = 0;

    //! Mouse release event for overriding
    virtual void canvasReleaseEvent(QMouseEvent * e) = 0;
    
    //! Called when rendering has finished
    virtual void renderComplete() {}
    
  protected:

    //! constructor takes map canvas as a parameter
    QgsMapTool(QgsMapCanvas* canvas) : mCanvas(canvas) {}
        
    QgsMapCanvas* mCanvas;
};

#endif
