/***************************************************************************
    qgsmapimage.h  -  class for rendering map layer set to pixmap
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

#ifndef QGSMAPIMAGE_H
#define QGSMAPIMAGE_H


#include "qgis.h"
#include <qgsrect.h>
#include <deque>

#include <QColor>
#include <QString>

class QPixmap;
class QgsScaleCalculator;
class QgsMapToPixel;


class QgsMapLayerSet
{
  public:
    std::deque<QString>& layerSet() { return mLayerSet; }
    int layerCount() { return mLayerSet.size(); }
    void setLayerSet(const std::deque<QString>& layers) { mLayerSet = layers; updateFullExtent(); }
    QgsRect fullExtent() { return mFullExtent; }
  
    // mainly for internal use, called directly only when projection changes
    void updateFullExtent();
    
  private:
    
    std::deque<QString> mLayerSet;
    QgsRect mFullExtent;
};





/**
 * \class QgsMapImage
 * \brief Class for rendering map layer set to pixmap
 *
 * \note
 * TODO:
 * - composition engine?
 * - callback for showing progress?
 * - drawing in separate thread?
 */

class QgsMapImage
{
  public:
    
    //! constructor, needs to specify pixmap size
    QgsMapImage(int width, int height);
    
    //! destructor
    ~QgsMapImage();

    //! starts rendering
    void render();
    
    //! sets extent and checks whether suitable (returns false if not)
    bool setExtent(const QgsRect& extent);
    
    //! returns current extent
    QgsRect extent();
    
    //! changes pixmap size
    void setPixmapSize(int width, int height);
    
    //! change background color
    void setBgColor(const QColor& color);

    QPixmap* pixmap() { return mPixmap; }

    void setLayerSet(const QgsMapLayerSet& layers) { mLayers = layers; }
    QgsMapLayerSet& layers() { return mLayers; }
    
    QgsMapToPixel* coordXForm() { return mCoordXForm; }
    
    double scale() const { return mScale; }
    double mupp() const { return mMupp; }

    QGis::units mapUnits() const { return mMapUnits; }
    void setMapUnits(QGis::units u);
    
    //! sets whether map image will be for overview
    void setOverview(bool isOverview = true) { mOverview = isOverview; }
  
  protected:
    
    //! adjust extent to fit the pixmap size
    void adjustExtentToPixmap();
    
  protected:
    
    //! Background color for the map canvas
    QColor mBgColor;
    
    //! indicates drawing in progress
    bool mDrawing;
    
    //! map units per pixel
    double mMupp;
    
    //! Map scale at its current zool level
    double mScale;
    
    //! map units
    QGis::units mMapUnits;
    
    //! scale calculator
    QgsScaleCalculator * mScaleCalculator;
    
    //! utility class for transformation between map and pixmap units
    QgsMapToPixel* mCoordXForm;
    
    //! layers for rendering
    QgsMapLayerSet mLayers;
    
    //! current extent to be drawn
    QgsRect mExtent;
    
    //! stores pixmap with rendered map
    QPixmap* mPixmap;
    
    //! indicates whether it's map image for overview
    bool mOverview;
};

#endif

