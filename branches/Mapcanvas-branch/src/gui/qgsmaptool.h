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
    //! constructor takes map canvas as a parameter
    QgsMapTool(QgsMapCanvas* canvas) : mCanvas(canvas) {}
    
    virtual ~QgsMapTool() {}
    
    //! Mouse move event for overriding
    virtual void mouseMoveEvent(QMouseEvent * e) = 0;

    //! Mouse press event for overriding
    virtual void mousePressEvent(QMouseEvent * e) = 0;

    //! Mouse release event for overriding
    virtual void mouseReleaseEvent(QMouseEvent * e) = 0;
    
  protected:
    QgsMapCanvas* mCanvas;
};

#endif
