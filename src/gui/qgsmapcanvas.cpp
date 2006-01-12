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

/*

TODO:
- bring back disabled stuff
  - render progress
  - acetate layer... or do we still need it?
  - digitizing stuff used in render()
*/

#include <QtGlobal>
#include <Q3Canvas>
#include <Q3CanvasRectangle>
#include <Q3ListView>
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
#include <QRubberBand>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QWheelEvent>

#include <iosfwd>
#include <cmath>
#include <cfloat>

#include "qgis.h"
#include "qgsrect.h"
#include "qgsacetatelines.h"
#include "qgsacetaterectangle.h"
#include "qgsattributedialog.h"
#include "qgsfeature.h"
#include "qgslegend.h"
#include "qgslegendlayerfile.h"
#include "qgslegenditem.h"
#include "qgsline.h"
#include "qgslinesymbol.h"
#include "qgsmapimage.h"
#include "qgsmaplayer.h"
#include "qgsmaplayerinterface.h"
#include "qgsmaptool.h"
#include "qgsmaptoolcapture.h"
#include "qgsmaptoolvertexedit.h"
#include "qgsmaptoolzoom.h"
#include "qgsmaptopixel.h"
#include "qgsmarkersymbol.h"
#include "qgspolygonsymbol.h"
#include "qgsproject.h"
#include "qgsmaplayerregistry.h"
#include "qgsmapoverviewcanvas.h"
#include "qgsmeasure.h"

#include "qgsmapcanvas.h"
#include "qgsmapcanvasproperties.h"


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
  mMapToolPtr = NULL;
  
  mDrawing = false;
  mFrozen = false;
  mDirty = true;
  
  // by default, the canvas is rendered
  mRenderFlag = true;

  setMouseTracking(true);
  setFocusPolicy(Qt::StrongFocus);
  
  setMapTool(QGis::NoTool);

  mMeasure = 0;

  mMapImage = new QgsMapImage(10,10);
  setbgColor(Qt::white);
  
  // create map canvas item which will show the map
  QgsMapCanvasMapImage* map = new QgsMapCanvasMapImage(mCanvas);
  map->setPixmap(mMapImage->pixmap());
  map->show();
      
} // QgsMapCanvas ctor


QgsMapCanvas::~QgsMapCanvas()
{
  delete mMapToolPtr;
  
  delete mCanvas;

  delete mMapImage;
  // mCanvasProperties auto-deleted via std::auto_ptr
  // CanvasProperties struct has its own dtor for freeing resources
  
} // dtor


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
  std::cout << "QgsMapCanvas::setCurrentLayer: " << layer << std::endl;
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


void QgsMapCanvas::addAcetateObject(QString key, QgsAcetateObject *obj)
{
  // since we are adding pointers, check to see if the object
  // referenced by key already exists and if so, delete it prior
  // to adding the new object with the same key
  QgsAcetateObject *oldObj = mCanvasProperties->acetateObjects[key];
  if(oldObj)
  {
    delete oldObj;
  }

  mCanvasProperties->acetateObjects[key] = obj;
}

void QgsMapCanvas::removeAcetateObject(const QString& key)
{
  std::map< QString, QgsAcetateObject *>::iterator it=mCanvasProperties->acetateObjects.find(key);
  if(it!=mCanvasProperties->acetateObjects.end())
  {
    QgsAcetateObject* toremove=it->second;
    mCanvasProperties->acetateObjects.erase(it->first);
    delete toremove;
  }
}

void QgsMapCanvas::removeDigitizingLines(bool norepaint)
{
  // TODO: to be moved to appopriate place
  /*
  bool rpaint = false;
  if(!norepaint)
  {
    rpaint = (mCaptureList.size()>0) ? true : false;
  }
  mCaptureList.clear();
  mLineEditing=false;
  mPolygonEditing=false;
  if(rpaint)
  {
    refresh();
  }
  */
}

QgsMapLayer* QgsMapCanvas::currentLayer()
{
  return mCurrentLayer;
}


void QgsMapCanvas::refresh()
{
  clear();
  // For Qt4, deprecate direct calling of render().  Let render() be called by the 
  // paint event loop of this widget.
    render();
  update();
} // refresh

// The painter device parameter is optional - if ommitted it will default
// to the pmCanvas (ie the gui map display). The idea is that you can pass
// an alternative device such as one that will be used for printing or
// saving a map view as an image file.
void QgsMapCanvas::render()
{
  
#ifdef QGISDEBUG
  QString msg = mFrozen ? "frozen" : "thawed";
  std::cout << ".............................." << std::endl;
  std::cout << "...........Rendering.........." << std::endl;
  std::cout << ".............................." << std::endl;
  std::cout << name() << " canvas is " << msg.toLocal8Bit().data() << std::endl;
#endif

  if ((!mFrozen && mDirty))
  {
    if (!mDrawing)
    {
      mDrawing = true;

      ///////////////////////////////////
      // RENDER
      if (mRenderFlag)
        mMapImage->render();

      //make verys sure progress bar arrives at 100%!
      emit setProgress(1,1);
#ifdef QGISDEBUG

      std::cout << "QgsMapCanvas::render: Done rendering map labels...emitting renderComplete(paint)\n";
#endif
 
      QPainter *paint = new QPainter();
      paint->begin(mMapImage->pixmap());

      // notify any listeners that rendering is complete
      //note that pmCanvas is not draw to gui yet
      emit renderComplete(paint);

      paint->end();
      mDrawing = false;
      delete paint;
    }
    mDirty = false;

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


void QgsMapCanvas::drawContents(QPainter * p, int cx, int cy, int cw, int ch)
{
#ifdef QGISDEBUG
  std::cout << "QgsMapCanvas::drawContents" << std::endl;
#endif
  
  if (mDirty)
  {
    //mapImage()->setVisible(false);
    render();
  }

  Q3CanvasView::drawContents(p,cx,cy,cw,ch);
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

void QgsMapCanvas::setExtent(QgsRect const & r)
{
  mMapImage->setExtent(r);
  emit extentsChanged(mMapImage->extent());
  updateScale();
  if (mMapOverview)
    mMapOverview->reflectChangedExtent();
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
  // TODO:
  std::cout << "------------> zoomPreviusExtent - stub" << std::endl;
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
        // mCanvasProperties->dragging = false;
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

void QgsMapCanvas::mousePressEvent(QMouseEvent * e)
{
  //////////////////////////
  // NEW MAP TOOLS
  if (mMapTool == QGis::ZoomIn || mMapTool == QGis::ZoomOut ||
      mMapTool == QGis::CapturePoint || mMapTool == QGis::CaptureLine || mMapTool == QGis::CapturePolygon ||
      mMapTool == QGis::AddVertex || mMapTool == QGis::MoveVertex || mMapTool == QGis::DeleteVertex)
  {
    mMapToolPtr->mousePressEvent(e);
    return;
  }
  
  if (mCanvasProperties->panSelectorDown)
    return;

  // right button was pressed in zoom tool, return to previous non zoom tool
  if ( e->button() == Qt::RightButton &&
      ( mMapTool == QGis::ZoomIn || mMapTool == QGis::ZoomOut || mMapTool == QGis::Pan ) )
  {
    //emit stopZoom();
    return;
  }

  mCanvasProperties->mouseButtonDown = true;
  mCanvasProperties->rubberStartPoint = e->pos();

#if QT_VERSION < 0x040000
  QPainter paint;
  QPen pen(Qt::gray);
#endif

  switch (mMapTool)
  {
    case QGis::Select:
      mCanvasProperties->zoomBox.setRect(0, 0, 0, 0);
      break;
    case QGis::Distance:
      //              distanceEndPoint = e->pos();
      break;

    case QGis::EmitPoint: 
      {
        QgsPoint  idPoint = getCoordinateTransform()->toMapCoordinates(e->x(), e->y());
        emit xyClickCoordinates(idPoint);
        emit xyClickCoordinates(idPoint,e->button());
        break;
      }

    case QGis::MeasureDist:
    case QGis::MeasureArea:
      {
        if (mMeasure && e->button() == Qt::LeftButton)
        {
          QgsPoint  idPoint = getCoordinateTransform()->toMapCoordinates(e->x(), e->y());
          mMeasure->mousePress(idPoint);
        }
        break;
      }
  }
} // mousePressEvent


void QgsMapCanvas::mouseReleaseEvent(QMouseEvent * e)
{
  //////////////////////////
  // NEW MAP TOOLS
  if (mMapTool == QGis::ZoomIn || mMapTool == QGis::ZoomOut ||
      mMapTool == QGis::CapturePoint || mMapTool == QGis::CaptureLine || mMapTool == QGis::CapturePolygon ||
      mMapTool == QGis::AddVertex || mMapTool == QGis::MoveVertex || mMapTool == QGis::DeleteVertex)
  {
    mMapToolPtr->mouseReleaseEvent(e);
    return;
  }

  mCanvasProperties->mouseButtonDown = false;

  if (mCanvasProperties->panSelectorDown)
    return;

  // right button was pressed in zoom tool, return to previous non zoom tool
  if ( e->button() == Qt::RightButton &&
      ( mMapTool == QGis::ZoomIn || mMapTool == QGis::ZoomOut || mMapTool == QGis::Pan ) )
  {
    emit stopZoom();
    return;
  }

#if QT_VERSION < 0x040000
  QPainter paint;
  QPen     pen(Qt::gray);
#endif
  QgsPoint ll, ur;

  if (mCanvasProperties->dragging)
  {
    mCanvasProperties->dragging = false;

    switch (mMapTool)
    {
      case QGis::Pan:
        panActionEnd(e->pos());
        break;

      case QGis::Select:
        {
          // TODO: Qt4 will have to do this a different way, using QRubberBand ...
#if QT_VERSION < 0x040000
          // erase the rubber band box
          paint.begin(this);
          paint.setPen(pen);
          paint.setRasterOp(Qt::XorROP);
          paint.drawRect(mCanvasProperties->zoomBox);
          paint.end();
#else
          delete mRubberBand;
#endif

          if (mCurrentLayer)
          {
            QgsPoint ll, ur;

            // store the rectangle
            mCanvasProperties->zoomBox.setRight(e->pos().x());
            mCanvasProperties->zoomBox.setBottom(e->pos().y());

            ll = getCoordinateTransform()->toMapCoordinates(mCanvasProperties->zoomBox.left(), mCanvasProperties->zoomBox.bottom());
            ur = getCoordinateTransform()->toMapCoordinates(mCanvasProperties->zoomBox.right(), mCanvasProperties->zoomBox.top());

            QgsRect *search = new QgsRect(ll.x(), ll.y(), ur.x(), ur.y());

            if ( e->state() == (Qt::LeftButton + Qt::ControlModifier) )
            {
              mCurrentLayer->select(search, true);
            } else
            {
              mCurrentLayer->select(search, false);
            }
            delete search;
          }
          else
          {
            QMessageBox::warning(this,
                tr("No active layer"),
                tr("To select features, you must choose an layer active by clicking on its name in the legend"));
          }
        }
        break;
    }
  }
  else // not dragging
  {
    // map tools that rely on a click not a drag
    switch (mMapTool)
    {

      case QGis::Identify:
        {
          // call identify method for selected layer

          if (mCurrentLayer)
          {

            // create the search rectangle
            double searchRadius = extent().width() * calculateSearchRadiusValue();
            QgsRect * search = new QgsRect();
            // convert screen coordinates to map coordinates
            QgsPoint idPoint = getCoordinateTransform()->toMapCoordinates(e->x(), e->y());
            search->setXmin(idPoint.x() - searchRadius);
            search->setXmax(idPoint.x() + searchRadius);
            search->setYmin(idPoint.y() - searchRadius);
            search->setYmax(idPoint.y() + searchRadius);

            mCurrentLayer->identify(search);

            delete search;
          }
          else
          {
            QMessageBox::warning(this,
                tr("No active layer"),
                tr("To identify features, you must choose an layer active by clicking on its name in the legend"));
          }
        }
        break;

    } // switch mMapTool

  } // if dragging / else

  // map tools that don't care if clicked or dragged
  switch (mMapTool)
  {
    case QGis::MeasureDist:
    case QGis::MeasureArea:
      {

        QgsPoint point = getCoordinateTransform()->toMapCoordinates(e->x(), e->y());

        if(e->button()==Qt::RightButton && (e->state() & Qt::LeftButton) == 0) // restart
        {
          if ( mMeasure )
          {   
            mMeasure->restart();
          }
        } 
        else if (e->button() == Qt::LeftButton)
        {
          mMeasure->addPoint(point);
          mMeasure->show();
        }
        break;
      }

  }
} // mouseReleaseEvent

void QgsMapCanvas::resizeEvent(QResizeEvent * e)
{
  int width = e->size().width(), height = e->size().height();
  mCanvas->resize(width, height);
  
  canvasMapImage()->setSize(width, height);
  
  mMapImage->setPixmapSize(width, height);
  updateScale();
  clear();
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


void QgsMapCanvas::mouseMoveEvent(QMouseEvent * e)
{
  //////////////////////////
  // NEW MAP TOOLS
  if (mMapTool == QGis::ZoomIn || mMapTool == QGis::ZoomOut ||
      mMapTool == QGis::CapturePoint || mMapTool == QGis::CaptureLine || mMapTool == QGis::CapturePolygon ||
      mMapTool == QGis::AddVertex || mMapTool == QGis::MoveVertex || mMapTool == QGis::DeleteVertex)
  {
    mMapToolPtr->mouseMoveEvent(e);
    return;
  }
  
  mCanvasProperties->mouseLastXY = e->pos();

  if (mCanvasProperties->panSelectorDown)
  {
    panAction(e);
  }
  else if (e->state() == Qt::LeftButton || e->state() == 513)
    // XXX magic numbers BAD -- 513?
  {
    // this is a drag-type operation (zoom, pan or other maptool)

#if QT_VERSION < 0x040000
    QPainter paint;
    QPen pen(Qt::gray);
#endif

    switch (mMapTool)
    {
      case QGis::Select:
        // draw the rubber band box as the user drags the mouse
#if QT_VERSION < 0x040000
        mCanvasProperties->dragging = true;

        // TODO: Qt4 will have to do this a different way, using QRubberBand ...
        paint.begin(this);
        paint.setPen(pen);
        paint.setRasterOp(Qt::XorROP);
        paint.drawRect(mCanvasProperties->zoomBox);

        mCanvasProperties->zoomBox.setLeft(mCanvasProperties->rubberStartPoint.x());
        mCanvasProperties->zoomBox.setTop(mCanvasProperties->rubberStartPoint.y());
        mCanvasProperties->zoomBox.setRight(e->pos().x());
        mCanvasProperties->zoomBox.setBottom(e->pos().y());

        paint.drawRect(mCanvasProperties->zoomBox);
        paint.end();
#else
        if (!mCanvasProperties->dragging)
        {
          mCanvasProperties->dragging = true;
          mRubberBand = new QRubberBand(QRubberBand::Rectangle, this);
          mCanvasProperties->zoomBox.setTopLeft(mCanvasProperties->rubberStartPoint);
        }
        mCanvasProperties->zoomBox.setBottomRight(e->pos());
        mRubberBand->setGeometry(mCanvasProperties->zoomBox);
        mRubberBand->show();
#endif

        break;
      case QGis::Pan:
        // show the pmCanvas as the user drags the mouse
        mCanvasProperties->dragging = true;
        panAction(e);

        break;

      case QGis::MeasureDist:
      case QGis::MeasureArea:
        if (mMeasure && (e->state() & Qt::LeftButton))
        {
          QgsPoint point = getCoordinateTransform()->toMapCoordinates(e->pos().x(), e->pos().y());
          mMeasure->mouseMove(point);
        }
        break;

    }
  } // if left button

  // show x y on status bar
  QPoint xy = e->pos();
  QgsPoint coord = getCoordinateTransform()->toMapCoordinates(xy);
  emit xyCoordinates(coord);
} // mouseMoveEvent

void QgsMapCanvas::drawLineToDigitisingCursor(QPainter* paint, bool last)
{
  QColor digitcolor(QgsProject::instance()->readNumEntry("Digitizing","/LineColorRedPart",255),\
      QgsProject::instance()->readNumEntry("Digitizing","/LineColorGreenPart",0),\
      QgsProject::instance()->readNumEntry("Digitizing","/LineColorBluePart",0));
  paint->setPen(QPen(digitcolor,QgsProject::instance()->readNumEntry("Digitizing","/LineWidth",1),Qt::SolidLine));

  // TODO: Qt4 will have to do this a different way, using QRubberBand ...
#if QT_VERSION < 0x040000
  paint->setRasterOp(Qt::XorROP);
  std::list<QgsPoint>::iterator it;
  if(last)
  {
    it=mCaptureList.end();
    --it;
  }
  else
  {
    it=mCaptureList.begin();
  }
  QgsPoint lastpoint = getCoordinateTransform()->transform(it->x(),it->y());
  QgsPoint digitpoint = getCoordinateTransform()->transform(mDigitMovePoint.x(), mDigitMovePoint.y());
  paint->drawLine(static_cast<int>(lastpoint.x()),static_cast<int>(lastpoint.y()),\
      static_cast<int>(digitpoint.x()), static_cast<int>(digitpoint.y()));
#endif

}

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
void QgsMapCanvas::setMapTool(int tool)
{
  mMapTool = tool;
  
  ///////////////////////
  // NEW MAP TOOLS
  delete mMapToolPtr;
  mMapToolPtr = NULL;
  if (tool == QGis::ZoomIn || tool == QGis::ZoomOut)
  {
    mMapToolPtr = new QgsMapToolZoom(this, (tool == QGis::ZoomOut));
  }
  else if (tool == QGis::CaptureLine || tool == QGis::CapturePoint || tool == QGis::CapturePolygon)
  {
    mMapToolPtr = new QgsMapToolCapture(this, (enum QGis::MapTools) tool);
  }
  else if (tool == QGis::AddVertex || tool == QGis::MoveVertex || tool == QGis::DeleteVertex)
  {
    mMapToolPtr = new QgsMapToolVertexEdit(this, (enum QGis::MapTools) tool);
  }


  if ( tool == QGis::EmitPoint ) {
    setCursor ( Qt::CrossCursor );
  }
  else if (tool == QGis::MeasureDist || tool == QGis::MeasureArea)
  {
    bool measureArea = (tool == QGis::MeasureArea);
    if (!mMeasure)
    {
      mMeasure = new QgsMeasure(measureArea, this, topLevelWidget());
    }
    else if (mMeasure && mMeasure->measureArea() != measureArea)
    {
      // tell window that the tool has been changed
      mMeasure->setMeasureArea(measureArea);
    }
  }
} // setMapTool


/** Write property of QColor bgColor. */
void QgsMapCanvas::setbgColor(const QColor & _newVal)
{
  mMapImage->setBgColor(_newVal);
  setEraseColor(_newVal);
  
  if (mMapOverview)
    mMapOverview->setbgColor(_newVal);
} // setbgColor


int QgsMapCanvas::layerCount() const
{
  return mMapImage->layers().layerCount();
} // layerCount



void QgsMapCanvas::layerStateChange()
{
  // called when a layer has changed visibility setting
  
  if (!mFrozen)
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




/* Calculates the search radius for identifying features
 * using the radius value stored in the users settings
 */
double QgsMapCanvas::calculateSearchRadiusValue()
{
  QSettings settings;

  int identifyValue = settings.readNumEntry("/qgis/map/identifyRadius", QGis::DEFAULT_IDENTIFY_RADIUS);

  return(identifyValue/1000.0);

} // calculateSearchRadiusValue


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


int QgsMapCanvas::mapTool()
{
  return mMapTool;
}

void QgsMapCanvas::panActionEnd(QPoint releasePoint)
{
  // move map image to standard position
  canvasMapImage()->move(0,0);
  
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

  // move map image...
  QPoint pnt = e->pos() - mCanvasProperties->rubberStartPoint;
  canvasMapImage()->move(pnt.x(), pnt.y());
  
  // update canvas
  update();
}


QgsPoint QgsMapCanvas::maybeInversePoint(QgsPoint point, const char whenmsg[])
{
  QgsVectorLayer *vlayer = dynamic_cast < QgsVectorLayer * >(mCurrentLayer);
  QgsPoint transformedPoint;

  if( projectionsEnabled() )
  {
    // Do reverse transformation before saving. If possible!
    try
    {
      transformedPoint = vlayer->coordinateTransform()->transform(point, QgsCoordinateTransform::INVERSE);
    }
    catch(QgsCsException &cse)
    {
      //#ifdef QGISDEBUG
      std::cout << "Caught transform error when " << whenmsg <<"." 
        << "Setting untransformed values." << std::endl;
      //#endif	
      // Transformation failed,. Bail out with original rectangle.
      return point;
    }
    return transformedPoint;
  }
  return point;
}
