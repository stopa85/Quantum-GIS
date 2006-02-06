/***************************************************************************
  qgsmapcanvas.cpp  -  description
  -------------------
begin                : Sun Jun 30 2002
copyright            : (C) 2002 by Gary E.Sherman
email                : sherman at mrcc.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* $Id$ */


#include <QtGlobal>
#include <Q3Canvas>
#include <Q3CanvasRectangle>
#include <QCursor>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPaintDevice>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QRect>
#include <QResizeEvent>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QWheelEvent>

#include "qgis.h"
#include "qgsmapcanvas.h"
#include "qgsmapimage.h"
#include "qgsmaplayer.h"
#include "qgsmaplayerregistry.h"
#include "qgsmaptool.h"
#include "qgsmaptopixel.h"
#include "qgsmapoverviewcanvas.h"
#include "qgsproject.h"
#include "qgsrubberband.h"

#include "qgsmaptoolzoom.h"
#include "qgsmaptoolpan.h"

/**  @DEPRECATED: to be deleted, stuff from here should be moved elsewhere */
class QgsMapCanvas::CanvasProperties
{
  public:

    CanvasProperties() : panSelectorDown( false ) { }

    //!Flag to indicate status of mouse button
    bool mouseButtonDown;

    //! Last seen point of the mouse
    QPoint mouseLastXY;

    //! Beginning point of a rubber band
    QPoint rubberStartPoint;

    //! Flag to indicate the pan selector key is held down by user
    bool panSelectorDown;

};


#define RTTI_MapImage 11111

class QgsMapCanvasMapImage : public Q3CanvasRectangle
{
  public:
    QgsMapCanvasMapImage(Q3Canvas *canvas)
      : Q3CanvasRectangle(canvas) { }
    
    void setPixmap(QPixmap* pixmap) { mPixmap = pixmap; }
    
    int rtti () const { return RTTI_MapImage; }
    
  protected:
    void drawShape(QPainter & p)
    {
      std::cerr << "~~~~~~~~~ drawing map pixmap at " << x() << "," << y() << std::endl;
      p.drawPixmap(int(x()), int(y()), *mPixmap);
    }

  private:
    QPixmap* mPixmap;
};

// But the static members must be initialised outside the class! (or GCC 4 dies)
const double QgsMapCanvas::scaleDefaultMultiple = 2.0;


/** note this is private and so shouldn't be accessible */
QgsMapCanvas::QgsMapCanvas()
{}

  QgsMapCanvas::QgsMapCanvas(QWidget * parent, const char *name)
: Q3CanvasView(parent, name),
  mCanvasProperties(new CanvasProperties)
{
  mCanvas = new Q3Canvas();
  setCanvas(mCanvas);
  setHScrollBarMode(Q3ScrollView::AlwaysOff);
  setVScrollBarMode(Q3ScrollView::AlwaysOff);
  
  mCurrentLayer = NULL;
  mMapOverview = NULL;
  mMapTool = NULL;
  mLastNonZoomMapTool = NULL;
  
  mDrawing = false;
  mFrozen = false;
  mDirty = true;
  
  // by default, the canvas is rendered
  mRenderFlag = true;
  
  viewport()->setMouseTracking(true);
  setFocusPolicy(Qt::StrongFocus);
  
  mMapImage = new QgsMapImage(10,10);

  int myRedInt = QgsProject::instance()->readNumEntry("Gui","/CanvasColorRedPart",255);
  int myGreenInt = QgsProject::instance()->readNumEntry("Gui","/CanvasColorGreenPart",255);
  int myBlueInt = QgsProject::instance()->readNumEntry("Gui","/CanvasColorBluePart",255);
  QColor myColor = QColor(myRedInt,myGreenInt,myBlueInt);
  setCanvasColor(myColor);
  
  // create map canvas item which will show the map
  QgsMapCanvasMapImage* map = new QgsMapCanvasMapImage(mCanvas);
  map->setPixmap(mMapImage->pixmap());
  map->setZ(-10);
  map->show();
  
  moveCanvasContents(TRUE);
  
  connect(mMapImage, SIGNAL(updateMap()), this, SLOT(updateMap()));
  
} // QgsMapCanvas ctor


QgsMapCanvas::~QgsMapCanvas()
{
  delete mMapTool;
  delete mLastNonZoomMapTool;
  
  delete mCanvas;

  delete mMapImage;
  // mCanvasProperties auto-deleted via std::auto_ptr
  // CanvasProperties struct has its own dtor for freeing resources
  
} // dtor

void QgsMapCanvas::enableAntiAliasing(bool theFlag)
{
  mMapImage->enableAntiAliasing(theFlag);
  if (mMapOverview)
    mMapOverview->mapImage()->enableAntiAliasing(theFlag);
} // anti aliasing

QgsMapImage* QgsMapCanvas::mapImage()
{
  return mMapImage;
}

QgsMapLayer* QgsMapCanvas::getZpos(int index)
{
  QString layer = mMapImage->layers().layerSet()[index];
  return QgsMapLayerRegistry::instance()->mapLayer(layer);
}


void QgsMapCanvas::setCurrentLayer(QgsMapLayer* layer)
{
  mCurrentLayer = layer;
  std::cout << "QgsMapCanvas::setCurrentLayer" << std::endl;
}

double QgsMapCanvas::getScale()
{
  return mMapImage->scale();
} // getScale

void QgsMapCanvas::setDirty(bool dirty)
{
  mDirty = dirty;
}

bool QgsMapCanvas::isDirty() const
{
  return mDirty;
}



bool QgsMapCanvas::isDrawing()
{
  return mDrawing;
} // isDrawing


// return the current coordinate transform based on the extents and
// device size
QgsMapToPixel * QgsMapCanvas::getCoordinateTransform()
{
  return mMapImage->coordXForm();
}

void QgsMapCanvas::setLayerSet(std::deque<QString>& layerSet)
{
  QgsMapLayerSet& layers = mMapImage->layers();
  
  int i;
  for (i = 0; i < layerCount(); i++)
  {
    QObject::disconnect(getZpos(i), SIGNAL(repaintRequested()), this, SLOT(refresh()));
  }
  
  layers.setLayerSet(layerSet);
  
  for (i = 0; i < layerCount(); i++)
  {
    QObject::connect(getZpos(i), SIGNAL(repaintRequested()), this, SLOT(refresh()));
  }

  if (mMapOverview)
  {
    mMapOverview->setLayerSet(layerSet);
    updateOverview();
  }
  
  emit layersChanged();
  
  refresh();

} // addLayer

void QgsMapCanvas::setOverview(QgsMapOverviewCanvas* overview)
{
  mMapOverview = overview;
}


void QgsMapCanvas::updateOverview()
{
  // redraw overview
  if (mMapOverview)
  {
    mMapOverview->refresh();
  }
}


QgsMapLayer* QgsMapCanvas::currentLayer()
{
  return mCurrentLayer;
}


void QgsMapCanvas::refresh()
{
  clear();
  update();
} // refresh


void QgsMapCanvas::render()
{
  
#ifdef QGISDEBUG
  QString msg = mFrozen ? "frozen" : "thawed";
  std::cout << "QgsMapCanvas::render: canvas is " << msg.toLocal8Bit().data() << std::endl;
#endif

  if ((!mFrozen && mDirty))
  {
    if (!mDrawing)
    {
      mDrawing = true;

      ///////////////////////////////////
      // RENDER
      if (mRenderFlag)
      {
        mMapImage->render();

#ifdef QGISDEBUG
        std::cout << "QgsMapCanvas::render: Done rendering...emitting renderComplete(paint)\n";
#endif
 
        QPainter *paint = new QPainter();
        paint->begin(mMapImage->pixmap());
        
        // notifies current map tool
        if (mMapTool)
          mMapTool->renderComplete();
  
        // notify any listeners that rendering is complete
        //note that pmCanvas is not draw to gui yet
        emit renderComplete(paint);
  
        paint->end();
        mDrawing = false;
        delete paint;
        mDirty = false;
        
      }
    }

    updateContents();
  }

} // render


//the format defaults to "PNG" if not specified
void QgsMapCanvas::saveAsImage(QString theFileName, QPixmap * theQPixmap, QString theFormat)
{
  //
  //check if the optional QPaintDevice was supplied
  //
  if (theQPixmap != NULL)
  {
    mMapImage->render();
    *theQPixmap = *mMapImage->pixmap();
    theQPixmap->save(theFileName,theFormat.toLocal8Bit().data());
  }
  else //use the map view
  {
    mMapImage->pixmap()->save(theFileName,theFormat.toLocal8Bit().data());
  }
} // saveAsImage

void QgsMapCanvas::paintEvent(QPaintEvent * ev)
{
#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::paintEvent" << std::endl;
  QRect cr = contentsRect();
  std::cout << "contents rect: " << cr.x() << " " << cr.y() << " " << cr.width() << " " << cr.height() << std::endl;
  QRect vr = viewport()->rect();
  std::cout << "viewport rect: " << vr.x() << " " << vr.y() << " " << vr.width() << " " << vr.height() << std::endl;
#endif
  
  if (mDirty)
    render();
  
  int cx, cy, cw, ch;
  ev->rect().getRect(&cx, &cy, &cw, &ch);
  QPainter p(this);
  drawContents(&p, cx, cy, cw, ch);
}

QgsMapCanvasMapImage* QgsMapCanvas::canvasMapImage()
{
  Q3CanvasItemList list = mCanvas->allItems();
  Q3CanvasItemList::iterator it = list.begin();
  while (it != list.end())
  {
    if ((*it)->rtti() == RTTI_MapImage)
      return (QgsMapCanvasMapImage*) (*it);
    it++;
  }
  return NULL; // we should never get here!
}

QgsRect QgsMapCanvas::extent() const
{
  return mMapImage->extent();
} // extent

QgsRect QgsMapCanvas::fullExtent() const
{
  return mMapImage->layers().fullExtent();
} // extent

void QgsMapCanvas::updateFullExtent()
{
  // projection settings have changed
  
#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::updateFullExtent" << std::endl;
#endif

  mMapImage->layers().updateFullExtent();
  if (mMapOverview)
  {
    mMapOverview->mapImage()->layers().updateFullExtent();
    mMapOverview->reflectChangedExtent();
    updateOverview();
  }
  refresh();
}


void QgsMapCanvas::setExtent(QgsRect const & r)
{
  QgsRect current = extent();
  mMapImage->setExtent(r);
  emit extentsChanged(mMapImage->extent());
  updateScale();
  if (mMapOverview)
    mMapOverview->reflectChangedExtent();
  mLastExtent = current;
} // setExtent
  

void QgsMapCanvas::updateScale()
{
  double scale = mMapImage->scale();
  QString myScaleString("Scale ");
  int thePrecision = 0;
  if (scale == 0)
    myScaleString = "";
  else if (scale >= 1)
    myScaleString += QString("1: ") + QString::number(scale,'f',thePrecision);
  else
    myScaleString += QString::number(1.0/scale, 'f', thePrecision) + QString(": 1");
  emit scaleChanged(myScaleString);
}


void QgsMapCanvas::clear()
{
  // Indicate to the next paint event that we need to rebuild the canvas contents
  setDirty(TRUE);

} // clear



void QgsMapCanvas::zoomFullExtent()
{
  setExtent(fullExtent());
  refresh();

} // zoomFullExtent



void QgsMapCanvas::zoomPreviousExtent()
{
  QgsRect current = extent();
  setExtent(mLastExtent);
  mLastExtent = current;
  refresh();
} // zoomPreviousExtent


bool QgsMapCanvas::projectionsEnabled()
{
  if (QgsProject::instance()->readNumEntry("SpatialRefSys","/ProjectionsEnabled",0)!=0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void QgsMapCanvas::mapUnitsChanged()
{
  // We assume that if the map units have changed, the changed value
  // will be accessible from QgsProject.
  setMapUnits(QgsProject::instance()->mapUnits());
}

void QgsMapCanvas::zoomToSelected()
{
  QgsVectorLayer *lyr = dynamic_cast < QgsVectorLayer * >(mCurrentLayer);

  if (lyr)
  {


    QgsRect rect ;
    if (projectionsEnabled())
    {
      try
      {      
        if ( ! lyr->coordinateTransform() )
        {
#ifdef QGISDEBUG 
          std::cout << "Throwing exception "<< __FILE__ << __LINE__ << std::endl; 
#endif
          throw QgsCsException( string("NO COORDINATE TRANSFORM FOUND FOR LAYER") );
        }

        rect = lyr->coordinateTransform()->transformBoundingBox(lyr->bBoxOfSelected());
      }
      catch (QgsCsException &cse)
      {
        qDebug( "Transform error caught in %s line %d:\n%s", __FILE__, __LINE__, cse.what());
      }
    }
    else
    {
      rect = lyr->bBoxOfSelected();
    }

    // no selected features, only one selected point feature 
    //or two point features with the same x- or y-coordinates
    if(rect.isEmpty())
    {
      return;
    }
    //zoom to an area
    else
    {
      // TODO: why such complicated way? [MD]
      QgsRect r;
      r.setXmin(rect.xMin());
      r.setYmin(rect.yMin());
      r.setXmax(rect.xMax());
      r.setYmax(rect.yMax());
      setExtent(r);
      refresh();
      return;
    }
  }
} // zoomToSelected

void QgsMapCanvas::keyPressEvent(QKeyEvent * e)
{

#ifdef QGISDEBUG
  qDebug("keyPress event at line %d in %s",  __LINE__, __FILE__);
#endif

  if (mCanvasProperties->mouseButtonDown || mCanvasProperties->panSelectorDown)
    return;

  QPainter paint;
  QPen     pen(Qt::gray);
  QgsPoint ll, ur;

  if (! mCanvasProperties->mouseButtonDown )
  {
    // Don't want to interfer with mouse events

    QgsRect currentExtent = mMapImage->extent();
    double dx = fabs((currentExtent.xMax()- currentExtent.xMin()) / 4);
    double dy = fabs((currentExtent.yMax()- currentExtent.yMin()) / 4);

    switch ( e->key() ) 
    {
      case Qt::Key_Left:
#ifdef QGISDEBUG
        std::cout << "Pan left" << std::endl;
#endif

        currentExtent.setXmin(currentExtent.xMin() - dx);
        currentExtent.setXmax(currentExtent.xMax() - dx);
        setExtent(currentExtent);
        refresh();
        break;

      case Qt::Key_Right:
#ifdef QGISDEBUG
        std::cout << "Pan right" << std::endl;
#endif

        currentExtent.setXmin(currentExtent.xMin() + dx);
        currentExtent.setXmax(currentExtent.xMax() + dx);
        setExtent(currentExtent);
        refresh();
        break;

      case Qt::Key_Up:
#ifdef QGISDEBUG
        std::cout << "Pan up" << std::endl;
#endif

        currentExtent.setYmax(currentExtent.yMax() + dy);
        currentExtent.setYmin(currentExtent.yMin() + dy);
        setExtent(currentExtent);
        refresh();
        break;

      case Qt::Key_Down:
#ifdef QGISDEBUG
        std::cout << "Pan down" << std::endl;
#endif
        currentExtent.setYmax(currentExtent.yMax() - dy);
        currentExtent.setYmin(currentExtent.yMin() - dy);
        setExtent(currentExtent);
        refresh();
        break;

      case Qt::Key_Space:
#ifdef QGISDEBUG
        std::cout << "Pressing pan selector" << std::endl;
#endif
        //mCanvasProperties->dragging = true;
        if ( ! e->isAutoRepeat() )
        {
          mCanvasProperties->panSelectorDown = true;
          mCanvasProperties->rubberStartPoint = mCanvasProperties->mouseLastXY;
        }
        break;

      default:
        // Pass it on
        e->ignore();
#ifdef QGISDEBUG
        qDebug("Ignoring key (%d)", e->key());
#endif


    }
  }
} //keyPressEvent()

void QgsMapCanvas::keyReleaseEvent(QKeyEvent * e)
{

#ifdef QGISDEBUG
  qDebug("keyRelease event at line %d in %s",  __LINE__, __FILE__);
#endif

  switch( e->key() )
  {
    case Qt::Key_Space:
      if ( !e->isAutoRepeat() && mCanvasProperties->panSelectorDown)
      {
#ifdef QGISDEBUG
        std::cout << "Releaseing pan selector" << std::endl;
#endif
        mCanvasProperties->panSelectorDown = false;
        panActionEnd(mCanvasProperties->mouseLastXY);
      }
      break;

    default:
      // Pass it on
      e->ignore();
#ifdef QGISDEBUG
      qDebug("Ignoring key release (%d)", e->key());
#endif
  }
} //keyReleaseEvent()


void QgsMapCanvas::contentsMousePressEvent(QMouseEvent * e)
{
  // call handler of current map tool
  if (mMapTool)
    mMapTool->canvasPressEvent(e);  
  
  if (mCanvasProperties->panSelectorDown)
    return;

  mCanvasProperties->mouseButtonDown = true;
  mCanvasProperties->rubberStartPoint = e->pos();

} // mousePressEvent


void QgsMapCanvas::contentsMouseReleaseEvent(QMouseEvent * e)
{
  // call handler of current map tool
  if (mMapTool)
  {
    mMapTool->canvasReleaseEvent(e);
    
    // right button was pressed in zoom tool? return to previous non zoom tool
    QString name = mMapTool->toolName();
    if (e->button() == Qt::RightButton && (name == MapTool_Zoom || name == MapTool_Pan))
    {
#ifdef QGISDEBUG
      std::cout << "Right click in map tool zoom or pan, last tool is " << (mLastNonZoomMapTool ? "not null." : "null.") <<  std::endl;
#endif

      // change to older non-zoom tool
      if (mLastNonZoomMapTool)
      {
        QgsMapTool* t = mLastNonZoomMapTool;
        mLastNonZoomMapTool = NULL;
        setMapTool(t);
      }
      return;
    }
  }

  
  mCanvasProperties->mouseButtonDown = false;

  if (mCanvasProperties->panSelectorDown)
    return;

} // mouseReleaseEvent

void QgsMapCanvas::resizeEvent(QResizeEvent * e)
{
  int width = e->size().width(), height = e->size().height();
  mCanvas->resize(width, height);
  
  canvasMapImage()->setSize(width, height);
  
  mMapImage->setPixmapSize(width, height);
  updateScale();
  refresh();
} // resizeEvent


void QgsMapCanvas::wheelEvent(QWheelEvent *e)
{
  // Zoom the map canvas in response to a mouse wheel event. Moving the
  // wheel forward (away) from the user zooms in by a factor of 2.
  // TODO The scale factor needs to be customizable by the user.
#ifdef QGISDEBUG
  std::cout << "Wheel event delta " << e->delta() << std::endl;
#endif

  // change extent
  zoomWithCenter(e->x(), e->y(), e->delta() > 0);

}

void QgsMapCanvas::zoomWithCenter(int x, int y, bool zoomIn)
{
  zoomByScale(x, y, (zoomIn ? 1/scaleDefaultMultiple : scaleDefaultMultiple));
}


void QgsMapCanvas::contentsMouseMoveEvent(QMouseEvent * e)
{
  mCanvasProperties->mouseLastXY = e->pos();

  if (mCanvasProperties->panSelectorDown)
  {
    panAction(e);
  }

  // call handler of current map tool
  if (mMapTool)
    mMapTool->canvasMoveEvent(e);
    
  // show x y on status bar
  QPoint xy = e->pos();
  QgsPoint coord = getCoordinateTransform()->toMapCoordinates(xy);
  emit xyCoordinates(coord);
} // mouseMoveEvent


/** 
 * Zooms at the given screen x and y by the given scale (< 1, zoom out, > 1, zoom in)
 */
void QgsMapCanvas::zoomByScale(int x, int y, double scaleFactor)
{
  // transform the mouse pos to map coordinates
  QgsPoint center  = getCoordinateTransform()->toMapPoint(x, y);
  QgsRect r = mMapImage->extent();
  r.scale(scaleFactor, &center);
  setExtent(r);
  refresh();
}


/** Sets the map tool currently being used on the canvas */
void QgsMapCanvas::setMapTool(QgsMapTool* tool)
{
  if (mMapTool)
    mMapTool->deactivate();
  
#ifdef QGISDEBUG
  std::cout << "setMapTool: using tool " <<
      (tool ? tool->toolName().toLocal8Bit().data() : "null") << std::endl;
#endif
  
  if (tool && (tool->toolName() == MapTool_Zoom || tool->toolName() == MapTool_Pan))
  {        
    // if zoom or pan tool will be active, save old tool
    // to bring it back on right click
    // (but only if it wasn't also zoom or pan tool)
    if (mMapTool && mMapTool->toolName() != MapTool_Zoom && mMapTool->toolName() != MapTool_Pan)
    {
      delete mLastNonZoomMapTool;
      mLastNonZoomMapTool = mMapTool;
    }
    
  }
  else
  {    
    // if there's already an old tool, delete it
    delete mLastNonZoomMapTool;
    mLastNonZoomMapTool = NULL;
  
    // delete current map tool
    delete mMapTool;
  }
  
  // set new map tool and activate it
  mMapTool = tool;
  if (mMapTool)
    mMapTool->activate();

} // setMapTool


/** Write property of QColor bgColor. */
void QgsMapCanvas::setCanvasColor(const QColor & theColor)
{
  mMapImage->setBgColor(theColor);
  setEraseColor(theColor);
  
  if (mMapOverview)
    mMapOverview->setbgColor(theColor);
} // setbgColor


int QgsMapCanvas::layerCount() const
{
  return mMapImage->layers().layerCount();
} // layerCount



void QgsMapCanvas::layerStateChange()
{
  // called when a layer has changed visibility setting
  
  refresh();

} // layerStateChange



void QgsMapCanvas::freeze(bool frz)
{
  mFrozen = frz;
} // freeze

bool QgsMapCanvas::isFrozen()
{
  return mFrozen;
} // freeze


QPixmap * QgsMapCanvas::canvasPixmap()
{
  return mMapImage->pixmap();
} // canvasPixmap



double QgsMapCanvas::mupp() const
{
  return mMapImage->mupp();
} // mupp


void QgsMapCanvas::setMapUnits(QGis::units u)
{
#ifdef QGISDEBUG
  std::cerr << "Setting map units to " << static_cast<int>(u) << std::endl;
#endif

  mMapImage->setMapUnits(u);
}


QGis::units QgsMapCanvas::mapUnits() const
{
  return mMapImage->mapUnits();
}


void QgsMapCanvas::setRenderFlag(bool theFlag)
{
  mRenderFlag = theFlag;
  // render the map
  if(mRenderFlag)
  {
    refresh();
  }
}

void QgsMapCanvas::connectNotify( const char * signal )
{
#ifdef QGISDEBUG
  std::cerr << "QgsMapCanvas connected to " << signal << "\n";
#endif
} //  QgsMapCanvas::connectNotify( const char * signal )


bool QgsMapCanvas::writeXML(QDomNode & layerNode, QDomDocument & doc)
{
  // Write current view extents
  QDomElement extentNode = doc.createElement("extent");
  layerNode.appendChild(extentNode);

  QDomElement xMin = doc.createElement("xmin");
  QDomElement yMin = doc.createElement("ymin");
  QDomElement xMax = doc.createElement("xmax");
  QDomElement yMax = doc.createElement("ymax");

  QgsRect r = mMapImage->extent();
  QDomText xMinText = doc.createTextNode(QString::number(r.xMin(), 'f'));
  QDomText yMinText = doc.createTextNode(QString::number(r.yMin(), 'f'));
  QDomText xMaxText = doc.createTextNode(QString::number(r.xMax(), 'f'));
  QDomText yMaxText = doc.createTextNode(QString::number(r.yMax(), 'f'));

  xMin.appendChild(xMinText);
  yMin.appendChild(yMinText);
  xMax.appendChild(xMaxText);
  yMax.appendChild(yMaxText);

  extentNode.appendChild(xMin);
  extentNode.appendChild(yMin);
  extentNode.appendChild(xMax);
  extentNode.appendChild(yMax);
  
  std::deque<QString>& layers = mMapImage->layers().layerSet();

  // Iterate over layers in zOrder
  // Call writeXML() on each
  QDomElement projectLayersNode = doc.createElement("projectlayers");
  projectLayersNode.setAttribute("layercount", qulonglong( layers.size() ));

  std::deque<QString>::iterator li = layers.begin();
  while (li != layers.end())
  {
    QgsMapLayer *ml = QgsMapLayerRegistry::instance()->mapLayer(*li);

    if (ml)
    {
      ml->writeXML(projectLayersNode, doc);
    }
    li++;
  }

  layerNode.appendChild(projectLayersNode);

  return true;
}


QgsMapTool* QgsMapCanvas::mapTool()
{
  return mMapTool;
}

void QgsMapCanvas::panActionEnd(QPoint releasePoint)
{
  // move map image and other items to standard position
  moveCanvasContents(TRUE); // TRUE means reset
  //canvasMapImage()->move(0,0);
  
  // use start and end box points to calculate the extent
  QgsPoint start = getCoordinateTransform()->toMapCoordinates(mCanvasProperties->rubberStartPoint);
  QgsPoint end = getCoordinateTransform()->toMapCoordinates(releasePoint);

  double dx = fabs(end.x() - start.x());
  double dy = fabs(end.y() - start.y());

  // modify the extent
  QgsRect r = mMapImage->extent();

  if (end.x() < start.x())
  {
    r.setXmin(r.xMin() + dx);
    r.setXmax(r.xMax() + dx);
  }
  else
  {
    r.setXmin(r.xMin() - dx);
    r.setXmax(r.xMax() - dx);
  }

  if (end.y() < start.y())
  {
    r.setYmax(r.yMax() + dy);
    r.setYmin(r.yMin() + dy);

  }
  else
  {
    r.setYmax(r.yMax() - dy);
    r.setYmin(r.yMin() - dy);

  }
  
  setExtent(r);
  refresh();
}

void QgsMapCanvas::panAction(QMouseEvent * e)
{
#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::panAction: entering." << std::endl;
#endif

  // move all map canvas items
  moveCanvasContents();
  
  // update canvas
  update();
  //canvas()->update();
}


void QgsMapCanvas::moveCanvasContents(bool reset)
{
  QPoint pnt = viewport()->pos();
  if (!reset)
    pnt += mCanvasProperties->mouseLastXY - mCanvasProperties->rubberStartPoint;
  
  Q3CanvasItemList list = mCanvas->allItems();
  Q3CanvasItemList::iterator it = list.begin();
  while (it != list.end())
  {
    Q3CanvasItem* item = *it;
    
    // this will only move items virtually
    item->move(pnt.x(), pnt.y());
    
    // this tells map canvas item to draw with offset
    QgsMapCanvasItem* canvasItem = dynamic_cast<QgsMapCanvasItem*>(item);
    if (canvasItem)
      canvasItem->setPanningOffset(pnt);
  
    it++;
  }
}


void QgsMapCanvas::emitPointEvent(QgsPoint& point, Qt::ButtonState state)
{
  emit xyClickCoordinates(point);
  emit xyClickCoordinates(point,state);
}


void QgsMapCanvas::updateMap()
{
  // XXX updating is not possible since we're already in paint loop
//  mCanvas->update();
//  QApplication::processEvents();
}
