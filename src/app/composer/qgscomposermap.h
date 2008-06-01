/***************************************************************************
                         qgscomposermap.h
                             -------------------
    begin                : January 2005
    copyright            : (C) 2005 by Radim Blazek
    email                : blazek@itc.it
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QGSCOMPOSERMAP_H
#define QGSCOMPOSERMAP_H

//#include "ui_qgscomposermapbase.h"
#include "qgscomposeritem.h"
#include "qgsrect.h"
#include <QGraphicsRectItem>
#include <QObject>
#include <QPixmap>

class QgsComposition;
class QgsMapCanvas;
class QgsMapToPixel;
class QDomNode;
class QDomDocument;
class QPainter;

/** \class QgsComposerMap 
 *  \brief Object representing map window. 
 */
// NOTE: QgsComposerMapBase must be first, otherwise does not compile
class QgsComposerMap : /*public QWidget , private Ui::QgsComposerMapBase,*/ public QObject, public QgsComposerItem
{
  Q_OBJECT

public:
    /** Constructor. */
    QgsComposerMap( QgsComposition *composition, int id, int x, int y, int width, int height );
    /** Constructor. Settings are read from project. */
    QgsComposerMap( QgsComposition *composition, int id );
    ~QgsComposerMap();

    /** \brief Calculate scale/extent.  */
    enum Calculate {
	Scale = 0,   // calculate scale from extent 
	Extent      // calculate map extent from scale
    };

    /** \brief Preview style  */
    enum PreviewMode {
	Cache = 0,   // Use raster cache 
	Render,      // Render the map
	Rectangle    // Display only rectangle
    };

    /** \brief Initialise GUI and other settings, shared by constructors */
    void init ( void );

    // Reimplement QgsComposerItem:
    bool writeSettings ( void );
    bool readSettings ( void );
    bool removeSettings ( void );
    bool writeXML( QDomNode & node, QDomDocument & document, bool temp = false );
    bool readXML( QDomNode & node );
     
    /** \brief Draw to paint device 
	@param extent map extent
	@param size size in scene coordinates
	@param dpi scene dpi*/
    void draw(QPainter *painter, const QgsRect& extent, const QSize& size, int dpi);

    /** \brief Reimplementation of QCanvasItem::paint - draw on canvas */
    void paint (QPainter* painter, const QStyleOptionGraphicsItem* itemStyle, QWidget* pWidget);

    /** \brief Recalculate rectangle/extent/scale according to current rule */
    void recalculate ( void );
    
    /** \brief Create cache image */
    void cache ( void );

    /** \brief Set values in GUI to current values */
    //void setOptions ( void );
    
    /** \brief Map name, used in legend combobox etc. */
    QString name ( void );

    /** resizes an item in x- and y direction (canvas coordinates)*/
    void resize(double dx, double dy);

    /**Sets new scene rectangle bounds and recalculates hight and extent*/
    void setSceneRect(const QRectF& rectangle);

    /** \brief Scale */
    double scale ( void );

    Calculate calculationMode() {return mCalculate;}
    PreviewMode previewMode() {return mPreviewMode;}
    void setPreviewMode(PreviewMode m) {mPreviewMode = m;}
    void setCalculationMode(Calculate c) {mCalculate = c;}

    // Set cache outdated
    void setCacheUpdated ( bool u = false );

public slots:

    // Called if map canvas has changed
    void mapCanvasChanged ( );

 signals:
    /**Is emitted when width/height is changed as a result of user interaction*/
    void extentChanged();

private:

    // Pointer to composition
    QgsComposition *mComposition;
    
    // Pointer to map canvas
    QgsMapCanvas *mMapCanvas;
    
    /** \brief Map name, used in legend combobox etc. */
    QString mName;

    // Map region in map units realy used for rendering 
    // It can be the same as mUserExtent, but it can be bigger in on dimension if mCalculate==Scale,
    // so that full rectangle in paper is used.
    QgsRect mExtent;

    // Cache used in composer preview
    // NOTE:  QCanvasView is slow with bigger images but the spped does not decrease with image size.
    //        It is very slow, with zoom in in QCanvasView, it seems, that QCanvas is stored as a big image
    //        with resolution necessary for current zoom and so always a big image mus be redrawn. 
    QPixmap mCachePixmap; 

    // Is cache up to date
    bool mCacheUpdated;
    
    // Resize schema
    Calculate mCalculate;
    
    /** \brief Preview style  */
    PreviewMode mPreviewMode;

    /** \brief Number of layers when cache was created  */
    int mNumCachedLayers;

    /** \brief set to true if in state of drawing. Concurrent requests to draw method are returned if set to true */
    bool mDrawing;

    /**Store last scale factor to avoid unnecessary repaints in case preview mode is 'Render'*/
    double mLastScaleFactorX;

    /**Sets new Extent and changes only width and height*/
    void setNewExtent(const QgsRect& extent);
    /**Sets new scale and changes only mExtent*/
    void setNewScale(double scaleDenominator);

    /**Returns the zoom factor of the graphics view. If no 
     graphics view exists, the default 1 is returned*/
    double horizontalViewScaleFactor() const;
};

#endif
