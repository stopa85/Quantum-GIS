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
#include "qgsmarkersymbol.h"
#include "qgspolygonsymbol.h"
#include "qgsproject.h"
#include "qgsmaplayerregistry.h"
#include "qgsmapoverviewcanvas.h"
#include "qgsmeasure.h"

#include "qgsmapcanvas.h"
#include "qgsmapcanvasproperties.h"


// But the static members must be initialised outside the class! (or GCC 4 dies)
const double QgsMapCanvas::scaleDefaultMultiple = 2.0;


/** note this is private and so shouldn't be accessible */
QgsMapCanvas::QgsMapCanvas()
{}

  QgsMapCanvas::QgsMapCanvas(QWidget * parent, const char *name)
: QWidget(parent, name),
  mCanvasProperties( new CanvasProperties(width(), height()) ),
  mLineEditing(false), mPolygonEditing(false),
  mUserInteractionAllowed(true) // by default we allow a user to interact with the canvas
{
  mCurrentLayer = NULL;
  mMapOverview = NULL;
  
  // by default, the canvas is rendered
  mRenderFlag = true;

  setMouseTracking(true);
  setFocusPolicy(Qt::StrongFocus);

  mMeasure = 0;

  mMapImage = new QgsMapImage(10,10);
  setbgColor(Qt::white);
  mCanvasProperties->coordXForm = mMapImage->coordXForm();
  
} // QgsMapCanvas ctor


QgsMapCanvas::~QgsMapCanvas()
{
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

void QgsMapCanvas::setDirty(bool _dirty)
{
  mCanvasProperties->dirty = _dirty;
} // setDirty



bool QgsMapCanvas::isDirty() const
{
  return mCanvasProperties->dirty;
} // isDirty



bool QgsMapCanvas::isDrawing()
{
  return mCanvasProperties->drawing;
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
    clear();
    // For Qt4, deprecate direct calling of render().  Let render() be called by the 
    // paint event loop of this widget.
          render();
    update();
  }
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
  QString msg = mCanvasProperties->frozen ? "frozen" : "thawed";
  std::cout << ".............................." << std::endl;
  std::cout << "...........Rendering.........." << std::endl;
  std::cout << ".............................." << std::endl;
  std::cout << name() << " canvas is " << msg.toLocal8Bit().data() << std::endl;
#endif

  if ((!mCanvasProperties->frozen && mCanvasProperties->dirty))
  {
    if (!mCanvasProperties->drawing)
    {
      mCanvasProperties->drawing = true;

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
      mCanvasProperties->drawing = false;
      delete paint;
    }
    mCanvasProperties->dirty = false;

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
  //  std::cout << "QgsMapCanvas::paintEvent: entering." << std::endl;
#endif

  // If a map is in the process of being panned, panAction() will end up calling this through repaint().
  // Test for this and paint quickly using a pixmap copy rather than going back to the source dataset.
  if (mCanvasProperties->panning)
  {
#ifdef QGISDEBUG
    //   std::cout << "QgsMapCanvas::paintEvent: about to drawPixmap with " << mCanvasProperties->pan_dx << 
    //                                                              " and " << mCanvasProperties->pan_dy << "." << std::endl;
#endif

    QPainter paint(this);
    paint.drawPixmap(mCanvasProperties->pan_delta, *mMapImage->pixmap());

#ifdef QGISDEBUG
    //  std::cout << "QgsMapCanvas::paintEvent: finished drawPixmap." << std::endl;
#endif
  }
  else if (!mCanvasProperties->drawing)
  {

    if (mCanvasProperties->dirty)
    {
      // rebuild, from source datasets, the pixmap we want to copy onto the canvas.
      render();
      // mCanvasProperties->dirty should generally be reset by this point.
    }

    if (!mCanvasProperties->dirty)
    {
      QPainter paint(this);
      paint.drawPixmap(ev->rect().topLeft(), *mMapImage->pixmap(), ev->rect());
      //    // just bit blit the image to the canvas
      //    bitBlt(this, ev->rect().topLeft(), mCanvasProperties->pmCanvas, ev->rect());
    }
  }
#ifdef QGISDEBUG
  //  std::cout << "QgsMapCanvas::paintEvent: exiting." << std::endl;
#endif
} // paintEvent



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
  mCanvasProperties->dirty = TRUE;

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

  if (!mUserInteractionAllowed || mCanvasProperties->mouseButtonDown
      || mCanvasProperties->panSelectorDown)
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

  if (!mUserInteractionAllowed)
    return;
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
  if (!mUserInteractionAllowed || mCanvasProperties->panSelectorDown)
    return;

  // right button was pressed in zoom tool, return to previous non zoom tool
  if ( e->button() == Qt::RightButton &&
      ( mCanvasProperties->mapTool == QGis::ZoomIn || mCanvasProperties->mapTool == QGis::ZoomOut
        || mCanvasProperties->mapTool == QGis::Pan ) )
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

  switch (mCanvasProperties->mapTool)
  {
    case QGis::Select:
    case QGis::ZoomIn:
    case QGis::ZoomOut:
      mCanvasProperties->zoomBox.setRect(0, 0, 0, 0);
      break;
    case QGis::Distance:
      //              distanceEndPoint = e->pos();
      break;

    case QGis::AddVertex:
      {
        // Find the closest line segment to the mouse position
        // Then set up the rubber band to its endpoints

        //QgsPoint segStart;
        //QgsPoint segStop;
        QgsGeometryVertexIndex beforeVertex;
        int atFeatureId;
        QgsGeometry atGeometry;
        double x1, y1;
        double x2, y2;



        QgsPoint point = mCanvasProperties->coordXForm->toMapCoordinates(e->x(), e->y());

        QgsVectorLayer *vlayer = dynamic_cast < QgsVectorLayer * >(mCurrentLayer);


#ifdef QGISDEBUG
        std::cout << "QgsMapCanvas::mousePressEvent: QGis::AddVertex." << std::endl;
#endif

        // TODO: Find nearest segment of the selected line, move that node to the mouse location
        if (!vlayer->snapSegmentWithContext(
              point,
              beforeVertex,
              atFeatureId,
              atGeometry,
              QgsProject::instance()->readDoubleEntry("Digitizing","/Tolerance",0)
              )
           )
        {
          QMessageBox::warning(0, "Error", "Could not snap segment. Have you set the tolerance?",
              QMessageBox::Ok, Qt::NoButton);
        }
        else
        {

#ifdef QGISDEBUG
          std::cout << "QgsMapCanvas::mousePressEvent: QGis::AddVertex: Snapped to segment fid " 
            << atFeatureId 
            //                << " and beforeVertex " << beforeVertex
            << "." << std::endl;
#endif

          // Save where we snapped to
          mCanvasProperties->snappedBeforeVertex = beforeVertex;
          mCanvasProperties->snappedAtFeatureId  = atFeatureId;
          mCanvasProperties->snappedAtGeometry   = atGeometry;

          // Get the endpoint of the snapped-to segment
          atGeometry.vertexAt(x2, y2, beforeVertex);

          // Get the startpoint of the snapped-to segment
          beforeVertex.decrement_back();
          atGeometry.vertexAt(x1, y1, beforeVertex);


#ifdef QGISDEBUG
          std::cout << "QgsMapCanvas::mousePressEvent: QGis::AddVertex: Snapped to segment "
            << x1 << ", " << y1 << "; "
            << x2 << ", " << y2
            << "." << std::endl;
#endif

          // Convert to canvas screen coordinates for rubber band
          mCanvasProperties->coordXForm->transformInPlace(x1, y1);
          mCanvasProperties->rubberStartPoint.setX( static_cast<int>( round(x1) ) );
          mCanvasProperties->rubberStartPoint.setY( static_cast<int>( round(y1) ) );

          mCanvasProperties->rubberMidPoint = e->pos();

          mCanvasProperties->coordXForm->transformInPlace(x2, y2);
          mCanvasProperties->rubberStopPoint.setX( static_cast<int>( round(x2) ) );
          mCanvasProperties->rubberStopPoint.setY( static_cast<int>( round(y2) ) );


#ifdef QGISDEBUG
          std::cout << "QgsMapCanvas::mousePressEvent: QGis::AddVertex: Transformed to widget "
            << x1 << ", " << y1 << "; "
            << x2 << ", " << y2
            << "." << std::endl;
#endif


          // TODO: Qt4 will have to do this a different way, using QRubberBand ...
#if QT_VERSION < 0x040000
          // Draw initial rubber band
          paint.begin(this);
          paint.setPen(pen);
          paint.setRasterOp(Qt::XorROP);

          paint.drawLine(mCanvasProperties->rubberStartPoint, mCanvasProperties->rubberMidPoint);
          paint.drawLine(mCanvasProperties->rubberMidPoint, mCanvasProperties->rubberStopPoint);

          paint.end();
#endif
        } // if snapSegmentWithContext

        break;
      }

    case QGis::MoveVertex:
      {
#ifdef QGISDEBUG
        std::cout << "QgsMapCanvas::mousePressEvent: QGis::MoveVertex." << std::endl;
#endif

        // TODO: Find nearest node of the selected line, move that node to the mouse location

        // Find the closest line segment to the mouse position
        // Then set up the rubber band to its endpoints

        QgsGeometryVertexIndex atVertex;
        int atFeatureId;
        QgsGeometry atGeometry;
        double x1, y1;
        double x2, y2;

        QgsPoint point = mCanvasProperties->coordXForm->toMapCoordinates(e->x(), e->y());

        QgsVectorLayer *vlayer = dynamic_cast < QgsVectorLayer * >(mCurrentLayer);

#ifdef QGISDEBUG
        std::cout << "QgsMapCanvas::mousePressEvent: QGis::MoveVertex." << std::endl;
#endif

        // TODO: Find nearest segment of the selected line, move that node to the mouse location
        if (!vlayer->snapVertexWithContext(
              point,
              atVertex,
              atFeatureId,
              atGeometry,
              QgsProject::instance()->readDoubleEntry("Digitizing","/Tolerance",0)
              )
           )
        {
          QMessageBox::warning(0, "Error", "Could not snap vertex. Have you set the tolerance?",
              QMessageBox::Ok, Qt::NoButton);
        }
        else
        {

#ifdef QGISDEBUG
          std::cout << "QgsMapCanvas::mousePressEvent: QGis::MoveVertex: Snapped to segment fid " 
            << atFeatureId 
            //                << " and beforeVertex " << beforeVertex
            << "." << std::endl;
#endif

          // Save where we snapped to
          mCanvasProperties->snappedAtVertex     = atVertex;
          mCanvasProperties->snappedAtFeatureId  = atFeatureId;
          mCanvasProperties->snappedAtGeometry   = atGeometry;

          // Get the startpoint of the rubber band, as the previous vertex to the snapped-to one.
          atVertex.decrement_back();
          mCanvasProperties->rubberStartPointIsValid = atGeometry.vertexAt(x1, y1, atVertex);

          // Get the endpoint of the rubber band, as the following vertex to the snapped-to one.
          atVertex.increment_back();
          atVertex.increment_back();
          mCanvasProperties->rubberStopPointIsValid = atGeometry.vertexAt(x2, y2, atVertex);


#ifdef QGISDEBUG
          std::cout << "QgsMapCanvas::mousePressEvent: QGis::MoveVertex: Snapped to vertex "
            << "(valid = " << mCanvasProperties->rubberStartPointIsValid << ") " << x1 << ", " << y1 << "; "
            << "(valid = " << mCanvasProperties->rubberStopPointIsValid  << ") " << x2 << ", " << y2
            << "." << std::endl;
#endif

          // Convert to canvas screen coordinates for rubber band
          if (mCanvasProperties->rubberStartPointIsValid)
          {
            mCanvasProperties->coordXForm->transformInPlace(x1, y1);
            mCanvasProperties->rubberStartPoint.setX( static_cast<int>( round(x1) ) );
            mCanvasProperties->rubberStartPoint.setY( static_cast<int>( round(y1) ) );
          }

          mCanvasProperties->rubberMidPoint = e->pos();

          if (mCanvasProperties->rubberStopPointIsValid)
          {
            mCanvasProperties->coordXForm->transformInPlace(x2, y2);
            mCanvasProperties->rubberStopPoint.setX( static_cast<int>( round(x2) ) );
            mCanvasProperties->rubberStopPoint.setY( static_cast<int>( round(y2) ) );
          }

#ifdef QGISDEBUG
          std::cout << "QgsMapCanvas::mousePressEvent: QGis::MoveVertex: Transformed to widget "
            << x1 << ", " << y1 << "; "
            << x2 << ", " << y2
            << "." << std::endl;
#endif

          // TODO: Qt4 will have to do this a different way, using QRubberBand ...
#if QT_VERSION < 0x040000
          // Draw initial rubber band
          paint.begin(this);
          paint.setPen(pen);
          paint.setRasterOp(Qt::XorROP);

          if (mCanvasProperties->rubberStartPointIsValid)
          {
            paint.drawLine(mCanvasProperties->rubberStartPoint, mCanvasProperties->rubberMidPoint);
          }
          if (mCanvasProperties->rubberStopPointIsValid)
          {
            paint.drawLine(mCanvasProperties->rubberMidPoint, mCanvasProperties->rubberStopPoint);
          }

          paint.end();
#endif
        } // if snapVertexWithContext


        break;
      }

    case QGis::DeleteVertex:
      {
#ifdef QGISDEBUG
        std::cout << "QgsMapCanvas::mousePressEvent: QGis::DeleteVertex." << std::endl;
#endif

        // TODO: Find nearest node of the selected line, show a big X symbol

        QgsGeometryVertexIndex atVertex;
        int atFeatureId;
        QgsGeometry atGeometry;
        double x1, y1;

        QgsPoint point = mCanvasProperties->coordXForm->toMapCoordinates(e->x(), e->y());

        QgsVectorLayer *vlayer = dynamic_cast < QgsVectorLayer * >(mCurrentLayer);

#ifdef QGISDEBUG
        std::cout << "QgsMapCanvas::mousePressEvent: QGis::DeleteVertex." << std::endl;
#endif

        // TODO: Find nearest segment of the selected line, move that node to the mouse location
        if (!vlayer->snapVertexWithContext(
              point,
              atVertex,
              atFeatureId,
              atGeometry,
              QgsProject::instance()->readDoubleEntry("Digitizing","/Tolerance",0)
              )
            // TODO: What if there is no snapped vertex?
           )
        {
          QMessageBox::warning(0, "Error", "Could not snap vertex. Have you set the tolerance?",
              QMessageBox::Ok, Qt::NoButton);
        }
        else
        {

#ifdef QGISDEBUG
          std::cout << "QgsMapCanvas::mousePressEvent: QGis::DeleteVertex: Snapped to segment fid " 
            << atFeatureId 
            //                << " and beforeVertex " << beforeVertex
            << "." << std::endl;
#endif

          // Save where we snapped to
          mCanvasProperties->snappedAtVertex     = atVertex;
          mCanvasProperties->snappedAtFeatureId  = atFeatureId;
          mCanvasProperties->snappedAtGeometry   = atGeometry;

          // Get the point of the snapped-to vertex
          atGeometry.vertexAt(x1, y1, atVertex);

#ifdef QGISDEBUG
          std::cout << "QgsMapCanvas::mousePressEvent: QGis::DeleteVertex: Snapped to vertex "
            << x1 << ", " << y1
            << "." << std::endl;
#endif

          // Convert to canvas screen coordinates
          mCanvasProperties->coordXForm->transformInPlace(x1, y1);
          mCanvasProperties->rubberMidPoint.setX( static_cast<int>( round(x1) ) );
          mCanvasProperties->rubberMidPoint.setY( static_cast<int>( round(y1) ) );

#ifdef QGISDEBUG
          std::cout << "QgsMapCanvas::mousePressEvent: QGis::DeleteVertex: Transformed to widget "
            << x1 << ", " << y1
            << "." << std::endl;
#endif

          // TODO: Qt4 will have to do this a different way, using QRubberBand ...
#if QT_VERSION < 0x040000
          // Draw X symbol - people can feel free to pretty this up if they like
          paint.begin(this);
          paint.setPen(pen);
          paint.setRasterOp(Qt::XorROP);

          // TODO: Make the following a static member or something
          int crossSize = 10;

          paint.drawLine( mCanvasProperties->rubberMidPoint.x() - crossSize,
              mCanvasProperties->rubberMidPoint.y() - crossSize,
              mCanvasProperties->rubberMidPoint.x() + crossSize,
              mCanvasProperties->rubberMidPoint.y() + crossSize  );

          paint.drawLine( mCanvasProperties->rubberMidPoint.x() - crossSize,
              mCanvasProperties->rubberMidPoint.y() + crossSize,
              mCanvasProperties->rubberMidPoint.x() + crossSize,
              mCanvasProperties->rubberMidPoint.y() - crossSize  );

          paint.end();
#endif
        } // if snapVertexWithContext


        break;
      }

    case QGis::EmitPoint: 
      {
        QgsPoint  idPoint = mCanvasProperties->coordXForm->
          toMapCoordinates(e->x(), e->y());
        emit xyClickCoordinates(idPoint);
        emit xyClickCoordinates(idPoint,e->button());
        break;
      }

    case QGis::MeasureDist:
    case QGis::MeasureArea:
      {
        if (mMeasure && e->button() == Qt::LeftButton)
        {
          QgsPoint  idPoint = mCanvasProperties->coordXForm->
            toMapCoordinates(e->x(), e->y());
          mMeasure->mousePress(idPoint);
        }
        break;
      }
  }
} // mousePressEvent


void QgsMapCanvas::mouseReleaseEvent(QMouseEvent * e)
{

  mCanvasProperties->mouseButtonDown = false;

  if (!mUserInteractionAllowed || mCanvasProperties->panSelectorDown)
    return;

  // right button was pressed in zoom tool, return to previous non zoom tool
  if ( e->button() == Qt::RightButton &&
      ( mCanvasProperties->mapTool == QGis::ZoomIn || mCanvasProperties->mapTool == QGis::ZoomOut
        || mCanvasProperties->mapTool == QGis::Pan ) )
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

    switch (mCanvasProperties->mapTool)
    {
      case QGis::ZoomIn:
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
          // store the rectangle
          mCanvasProperties->zoomBox.setRight(e->pos().x());
          mCanvasProperties->zoomBox.setBottom(e->pos().y());
          // set the extent to the zoomBox
  
          ll = mCanvasProperties->coordXForm->toMapCoordinates(mCanvasProperties->zoomBox.left(), mCanvasProperties->zoomBox.bottom());
          ur = mCanvasProperties->coordXForm->toMapCoordinates(mCanvasProperties->zoomBox.right(), mCanvasProperties->zoomBox.top());
          
          
          QgsRect r;
          r.setXmin(ll.x());
          r.setYmin(ll.y());
          r.setXmax(ur.x());
          r.setYmax(ur.y());
          r.normalize();
          setExtent(r);
          refresh();
        }
        break;
      case QGis::ZoomOut:
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
          // store the rectangle
          mCanvasProperties->zoomBox.setRight(e->pos().x());
          mCanvasProperties->zoomBox.setBottom(e->pos().y());
          // scale the extent so the current view fits inside the zoomBox
          ll = mCanvasProperties->coordXForm->toMapCoordinates(mCanvasProperties->zoomBox.left(), mCanvasProperties->zoomBox.bottom());
          ur = mCanvasProperties->coordXForm->toMapCoordinates(mCanvasProperties->zoomBox.right(), mCanvasProperties->zoomBox.top());
          
          QgsRect r;
          r.setXmin(ll.x());
          r.setYmin(ll.y());
          r.setXmax(ur.x());
          r.setYmax(ur.y());
          r.normalize();

          QgsPoint cer = r.center();

          /* std::cout << "Current extent rectangle is " << tempRect << std::endl;
             std::cout << "Center of zoom out rectangle is " << cer << std::endl;
             std::cout << "Zoom out rectangle should have ll of " << ll << " and ur of " << ur << std::endl;
             std::cout << "Zoom out rectangle is " << mCanvasProperties->currentExtent << std::endl;
             */
          double sf;
          if (mCanvasProperties->zoomBox.width() > mCanvasProperties->zoomBox.height())
          {
            sf = mMapImage->extent().width() / r.width();
          }
          else
          {
            sf = mMapImage->extent().height() / r.height();
          }
          
          r.expand(sf);
          
#ifdef QGISDEBUG
          std::cout << "Extent scaled by " << sf << " to " << r << std::endl;
          std::cout << "Center of currentExtent after scaling is " << r.center() << std::endl;
#endif

          setExtent(r);
          refresh();
        }
        break;

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

            ll = mCanvasProperties->coordXForm->toMapCoordinates(mCanvasProperties->zoomBox.left(), mCanvasProperties->zoomBox.bottom());
            ur = mCanvasProperties->coordXForm->toMapCoordinates(mCanvasProperties->zoomBox.right(), mCanvasProperties->zoomBox.top());

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
    switch (mCanvasProperties->mapTool)
    {

      case QGis::ZoomIn:
        {
          // change to zoom in by the default multiple
          zoomByScale(e->x(), e->y(), (1/scaleDefaultMultiple) );
          break;
        }

      case QGis::ZoomOut:
        {
          // change to zoom out by the default multiple
          zoomByScale(e->x(), e->y(), scaleDefaultMultiple );
          break;
        }

      case QGis::Identify:
        {
          // call identify method for selected layer

          if (mCurrentLayer)
          {

            // create the search rectangle
            double searchRadius = extent().width() * calculateSearchRadiusValue();
            QgsRect * search = new QgsRect();
            // convert screen coordinates to map coordinates
            QgsPoint idPoint = mCanvasProperties->coordXForm->toMapCoordinates(e->x(), e->y());
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

      case QGis::CapturePoint:
        {
          QgsVectorLayer *vlayer = dynamic_cast < QgsVectorLayer * >(mCurrentLayer);

          if (vlayer)
          {

            QgsPoint  idPoint = mCanvasProperties->coordXForm->toMapCoordinates(e->x(), e->y());
            emit xyClickCoordinates(idPoint);

            //only do the rest for provider with feature addition support
            //note that for the grass provider, this will return false since
            //grass provider has its own mechanism of feature addition
            if(vlayer->getDataProvider()->capabilities()&QgsVectorDataProvider::AddFeatures)
            {
              if(!vlayer->isEditable() )
              {
                QMessageBox::information(0,"Layer not editable",
                    "Cannot edit the vector layer. Use 'Start editing' in the legend item menu",
                    QMessageBox::Ok);
                break;
              }

              //snap point to points within the vector layer snapping tolerance
              vlayer->snapPoint(idPoint,QgsProject::instance()->readDoubleEntry("Digitizing","/Tolerance",0));

              QgsFeature* f = new QgsFeature(0,"WKBPoint");
              int size=5+2*sizeof(double);
              unsigned char *wkb = new unsigned char[size];
              int wkbtype=QGis::WKBPoint;
              QgsPoint savePoint = maybeInversePoint(idPoint, "adding point");
              double x = savePoint.x();
              double y = savePoint.y();
              memcpy(&wkb[1],&wkbtype, sizeof(int));
              memcpy(&wkb[5], &x, sizeof(double));
              memcpy(&wkb[5]+sizeof(double), &y, sizeof(double));
              f->setGeometryAndOwnership(&wkb[0],size);

              //add the fields to the QgsFeature
              std::vector<QgsField> fields=vlayer->fields();
              for(std::vector<QgsField>::iterator it=fields.begin();it!=fields.end();++it)
              {
                f->addAttribute((*it).name(), vlayer->getDefaultValue(it->name(),f));
              }

              //show the dialog to enter attribute values
              if(f->attributeDialog())
              {
                vlayer->addFeature(f);
              }
              refresh();
            }
          }
          else
          {
            QMessageBox::information(0,"Not a vector layer","The current layer is not a vector layer",QMessageBox::Ok);
          }

          break;
        }  

      case QGis::CaptureLine:
      case QGis::CapturePolygon:
        {

          if (mCanvasProperties->rubberStartPoint != mCanvasProperties->rubberStopPoint)
          {
            // TODO: Qt4 will have to do this a different way, using QRubberBand ...
#if QT_VERSION < 0x040000
            // XOR-out the old line
            paint.drawLine(mCanvasProperties->rubberStartPoint, mCanvasProperties->rubberStopPoint);
#endif
          }

          mCanvasProperties->rubberStopPoint = e->pos();


          QgsVectorLayer *vlayer = dynamic_cast < QgsVectorLayer * >(mCurrentLayer);

          if(vlayer)
          {
            if(!vlayer->isEditable())// && (vlayer->providerType().lower() != "grass"))
            {
              QMessageBox::information(0,"Layer not editable",
                  "Cannot edit the vector layer. Use 'Start editing' in the legend item menu",
                  QMessageBox::Ok);
              break;
            }
          }
          else
          {
            QMessageBox::information(0,"Not a vector layer",
                "The current layer is not a vector layer",
                QMessageBox::Ok);
            return;
          }

          //prevent clearing of the line between the first and the second polygon vertex
          //during the next mouse move event
          if(mCaptureList.size() == 1 && mCanvasProperties->mapTool == QGis::CapturePolygon)
          {
            QPainter paint(mMapImage->pixmap());
            drawLineToDigitisingCursor(&paint);
          }

          mDigitMovePoint.setX(e->x());
          mDigitMovePoint.setY(e->y());
          mDigitMovePoint=mCanvasProperties->coordXForm->toMapCoordinates(e->x(), e->y());
          QgsPoint digitisedpoint=mCanvasProperties->coordXForm->toMapCoordinates(e->x(), e->y());
          vlayer->snapPoint(digitisedpoint,QgsProject::instance()->readDoubleEntry("Digitizing","/Tolerance",0));
          mCaptureList.push_back(digitisedpoint);
          if(mCaptureList.size()>1)
          {
            QPainter paint(this);
            QColor digitcolor(QgsProject::instance()->readNumEntry("Digitizing","/LineColorRedPart",255),
                QgsProject::instance()->readNumEntry("Digitizing","/LineColorGreenPart",0),
                QgsProject::instance()->readNumEntry("Digitizing","/LineColorBluePart",0));
            paint.setPen(QPen(digitcolor,QgsProject::instance()->readNumEntry("Digitizing","/LineWidth",1),Qt::SolidLine));
            std::list<QgsPoint>::iterator it=mCaptureList.end();
            --it;
            --it;

            try
            {
              QgsPoint lastpoint = mCanvasProperties->coordXForm->transform(it->x(),it->y());
              QgsPoint endpoint = mCanvasProperties->coordXForm->transform(digitisedpoint.x(),digitisedpoint.y());
              paint.drawLine(static_cast<int>(lastpoint.x()),static_cast<int>(lastpoint.y()),
                  static_cast<int>(endpoint.x()),static_cast<int>(endpoint.y()));
            }
            catch(QgsException &e)
            {
              // ignore this 
              // we need it to keep windows quiet
            }
            repaint();
          }

          if (e->button() == Qt::RightButton)
          {
            // End of string

            mCanvasProperties->capturing = FALSE;

            //create QgsFeature with wkb representation
            QgsFeature* f=new QgsFeature(0,"WKBLineString");
            unsigned char* wkb;
            int size;
            if(mCanvasProperties->mapTool==QGis::CaptureLine)
            {
              size=1+2*sizeof(int)+2*mCaptureList.size()*sizeof(double);
              wkb= new unsigned char[size];
              int wkbtype=QGis::WKBLineString;
              int length=mCaptureList.size();
              memcpy(&wkb[1],&wkbtype, sizeof(int));
              memcpy(&wkb[5],&length, sizeof(int));
              int position=1+2*sizeof(int);
              double x,y;
              for(std::list<QgsPoint>::iterator it=mCaptureList.begin();it!=mCaptureList.end();++it)
              {
                QgsPoint savePoint = maybeInversePoint(*it, "adding line");
                x = savePoint.x();
                y = savePoint.y();

                memcpy(&wkb[position],&x,sizeof(double));
                position+=sizeof(double);

                memcpy(&wkb[position],&y,sizeof(double));
                position+=sizeof(double);
              }
            }
            else//polygon
            {
              size=1+3*sizeof(int)+2*(mCaptureList.size()+1)*sizeof(double);
              wkb= new unsigned char[size];
              int wkbtype=QGis::WKBPolygon;
              int length=mCaptureList.size()+1;//+1 because the first point is needed twice
              int numrings=1;
              memcpy(&wkb[1],&wkbtype, sizeof(int));
              memcpy(&wkb[5],&numrings,sizeof(int));
              memcpy(&wkb[9],&length, sizeof(int));
              int position=1+3*sizeof(int);
              double x,y;
              std::list<QgsPoint>::iterator it;
              for(it=mCaptureList.begin();it!=mCaptureList.end();++it)
              {
                QgsPoint savePoint = maybeInversePoint(*it, "adding poylgon");
                x = savePoint.x();
                y = savePoint.y();

                memcpy(&wkb[position],&x,sizeof(double));
                position+=sizeof(double);

                memcpy(&wkb[position],&y,sizeof(double));
                position+=sizeof(double);
              }
              //close the polygon
              it=mCaptureList.begin();
              QgsPoint savePoint = maybeInversePoint(*it, "closing polygon");
              x = savePoint.x();
              y = savePoint.y();

              memcpy(&wkb[position],&x,sizeof(double));
              position+=sizeof(double);

              memcpy(&wkb[position],&y,sizeof(double));
            }
            f->setGeometryAndOwnership(&wkb[0],size);

            //add the fields to the QgsFeature
            std::vector<QgsField> fields=vlayer->fields();
            for(std::vector<QgsField>::iterator it=fields.begin();it!=fields.end();++it)
            {
              f->addAttribute((*it).name(),vlayer->getDefaultValue(it->name(), f));
            }

            if(f->attributeDialog())
            {
              vlayer->addFeature(f);
            }

            // delete the elements of mCaptureList
            mCaptureList.clear();
            refresh();

          }
          else if (e->button() == Qt::LeftButton)
          {
            mCanvasProperties->capturing = TRUE;
          }
          break;
        }  

        /*      case QGis::Measure:
                {
                QgsPoint point = mCanvasProperties->coordXForm->toMapCoordinates(e->x(), e->y());

                if ( !mMeasure ) {
                mMeasure = new QgsMeasure(this, topLevelWidget() );
                }
                mMeasure->addPoint(point);
                mMeasure->show();
                break;
                }*/

    } // switch mapTool

  } // if dragging / else

  // map tools that don't care if clicked or dragged
  switch (mCanvasProperties->mapTool)
  {
    case QGis::AddVertex:
      {
        QgsPoint point = mCanvasProperties->coordXForm->toMapCoordinates(e->x(), e->y());

        QgsVectorLayer *vlayer = dynamic_cast < QgsVectorLayer * >(mCurrentLayer);


#ifdef QGISDEBUG
        std::cout << "QgsMapCanvas::mouseReleaseEvent: QGis::AddVertex." << std::endl;
#endif

        // TODO: Find nearest line portion of the selected line, add a node at the mouse location

        // TODO: Qt4 will have to do this a different way, using QRubberBand ...
#if QT_VERSION < 0x040000
        // Undraw rubber band
        paint.begin(this);
        paint.setPen(pen);
        paint.setRasterOp(Qt::XorROP);

        // XOR-out the old line
        paint.drawLine(mCanvasProperties->rubberStartPoint, mCanvasProperties->rubberMidPoint);
        paint.drawLine(mCanvasProperties->rubberMidPoint, mCanvasProperties->rubberStopPoint);
        paint.end();
#endif

        // Add the new vertex

#ifdef QGISDEBUG
        std::cout << "QgsMapCanvas::mouseReleaseEvent: About to vlayer->insertVertexBefore." << std::endl;
#endif

        if (vlayer)
        {

          // only do the rest for provider with geometry modification support
          // TODO: Move this test earlier into the workflow, maybe by triggering just after the user selects "Add Vertex" or even by graying out the menu option.
          if (vlayer->getDataProvider()->capabilities() & QgsVectorDataProvider::ChangeGeometries)
          {
            if ( !vlayer->isEditable() )
            {
              QMessageBox::information(0,"Layer not editable",
                  "Cannot edit the vector layer. Use 'Start editing' in the legend item menu",
                  QMessageBox::Ok);
              break;
            }

            vlayer->insertVertexBefore(
                point.x(), point.y(), 
                mCanvasProperties->snappedAtFeatureId,
                mCanvasProperties->snappedBeforeVertex);


#ifdef QGISDEBUG
            std::cout << "QgsMapCanvas::mouseReleaseEvent: Completed vlayer->insertVertexBefore." << std::endl;
#endif
          }
        }

        // TODO: Redraw?  
        break;
      }  

    case QGis::MoveVertex:
      {
#ifdef QGISDEBUG
        std::cout << "QgsMapCanvas::mouseReleaseEvent: QGis::MoveVertex." << std::endl;
#endif
        QgsPoint point = mCanvasProperties->coordXForm->toMapCoordinates(e->x(), e->y());

        QgsVectorLayer *vlayer = dynamic_cast < QgsVectorLayer * >(mCurrentLayer);

        // TODO: Qt4 will have to do this a different way, using QRubberBand ...
#if QT_VERSION < 0x040000
        // Undraw rubber band
        paint.begin(this);
        paint.setPen(pen);
        paint.setRasterOp(Qt::XorROP);

        // XOR-out the old line
        paint.drawLine(mCanvasProperties->rubberStartPoint, mCanvasProperties->rubberMidPoint);
        paint.drawLine(mCanvasProperties->rubberMidPoint, mCanvasProperties->rubberStopPoint);
        paint.end();
#endif

        // Move the vertex

#ifdef QGISDEBUG
        std::cout << "QgsMapCanvas::mouseReleaseEvent: About to vlayer->moveVertexAt." << std::endl;
#endif

        if (vlayer)
        {
          // TODO: Move this test earlier into the workflow, maybe by triggering just after the user selects "Move Vertex" or even by graying out the menu option.
          if (vlayer->getDataProvider()->capabilities() & QgsVectorDataProvider::ChangeGeometries)
          {
            if ( !vlayer->isEditable() )
            {
              QMessageBox::information(0,"Layer not editable",
                  "Cannot edit the vector layer. Use 'Start editing' in the legend item menu",
                  QMessageBox::Ok);
              break;
            }

            vlayer->moveVertexAt(
                point.x(), point.y(),
                mCanvasProperties->snappedAtFeatureId,
                mCanvasProperties->snappedAtVertex);

#ifdef QGISDEBUG
            std::cout << "QgsMapCanvas::mouseReleaseEvent: Completed vlayer->moveVertexAt." << std::endl;
#endif
          }
        }
        // TODO: Redraw?  

        break;
      }  

    case QGis::DeleteVertex:
      {
#ifdef QGISDEBUG
        std::cout << "QgsMapCanvas::mouseReleaseEvent: QGis::DeleteVertex." << std::endl;
#endif
        QgsVectorLayer *vlayer = dynamic_cast < QgsVectorLayer * >(mCurrentLayer);

        // TODO: Qt4 will have to do this a different way, using QRubberBand ...
#if QT_VERSION < 0x040000
        // Undraw X symbol - people can feel free to pretty this up if they like
        paint.begin(this);
        paint.setPen(pen);
        paint.setRasterOp(Qt::XorROP);

        // TODO: Make the following a static member or something
        int crossSize = 10;

        paint.drawLine( mCanvasProperties->rubberMidPoint.x() - crossSize,
            mCanvasProperties->rubberMidPoint.y() - crossSize,
            mCanvasProperties->rubberMidPoint.x() + crossSize,
            mCanvasProperties->rubberMidPoint.y() + crossSize  );

        paint.drawLine( mCanvasProperties->rubberMidPoint.x() - crossSize,
            mCanvasProperties->rubberMidPoint.y() + crossSize,
            mCanvasProperties->rubberMidPoint.x() + crossSize,
            mCanvasProperties->rubberMidPoint.y() - crossSize  );

        paint.end();
#endif

        if (vlayer)
        {
          // TODO: Move this test earlier into the workflow, maybe by triggering just after the user selects "Delete Vertex" or even by graying out the menu option.
          if (vlayer->getDataProvider()->capabilities() & QgsVectorDataProvider::ChangeGeometries)
          {
            if ( !vlayer->isEditable() )
            {
              QMessageBox::information(0,"Layer not editable",
                  "Cannot edit the vector layer. Use 'Start editing' in the legend item menu",
                  QMessageBox::Ok);
              break;
            }

            vlayer->deleteVertexAt(
                mCanvasProperties->snappedAtFeatureId,
                mCanvasProperties->snappedAtVertex);

#ifdef QGISDEBUG
            std::cout << "QgsMapCanvas::mouseReleaseEvent: Completed vlayer->deleteVertexAt." << std::endl;
#endif
          }
        }
        // TODO: Redraw?  

        break;
      }

    case QGis::MeasureDist:
    case QGis::MeasureArea:
      {

        QgsPoint point = mCanvasProperties->coordXForm->toMapCoordinates(e->x(), e->y());

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
  mMapImage->setPixmapSize(e->size().width(), e->size().height());
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
  double scaleFactor = scaleDefaultMultiple;
  if(e->delta() > 0)
  {
    scaleFactor = 1/scaleFactor;
  }

  zoomByScale(e->x(), e->y(), scaleFactor);

}

void QgsMapCanvas::mouseMoveEvent(QMouseEvent * e)
{
  if (!mUserInteractionAllowed)
    return;

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

    switch (mCanvasProperties->mapTool)
    {
      case QGis::Select:
      case QGis::ZoomIn:
      case QGis::ZoomOut:
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
          QgsPoint point = mCanvasProperties->coordXForm->toMapCoordinates(e->pos().x(), e->pos().y());
          mMeasure->mouseMove(point);
        }
        break;

      case QGis::AddVertex:
      case QGis::MoveVertex:

        // TODO: Redraw rubber band

        // TODO: Qt4 will have to do this a different way, using QRubberBand ...
#if QT_VERSION < 0x040000
        paint.begin(this);
        paint.setPen(pen);
        paint.setRasterOp(Qt::XorROP);

        // XOR-out the old line
        paint.drawLine(mCanvasProperties->rubberStartPoint, mCanvasProperties->rubberMidPoint);
        paint.drawLine(mCanvasProperties->rubberMidPoint, mCanvasProperties->rubberStopPoint);

        mCanvasProperties->rubberMidPoint = e->pos();

        // XOR-in the new line
        paint.drawLine(mCanvasProperties->rubberStartPoint, mCanvasProperties->rubberMidPoint);
        paint.drawLine(mCanvasProperties->rubberMidPoint, mCanvasProperties->rubberStopPoint);

        paint.end();
#endif

        break;

    } // case
  } // if left button

  // Some tools require us to do some stuff whether we are dragging or not

  switch (mCanvasProperties->mapTool)
  {
    case QGis::CapturePoint:
    case QGis::CaptureLine:
    case QGis::CapturePolygon:

      if (mCanvasProperties->capturing)
      {

        // show the rubber-band from the last click

        QPainter paint;
        QPen pen(Qt::gray);

        // TODO: Qt4 will have to do this a different way, using QRubberBand ...
#if QT_VERSION < 0x040000
        paint.begin(this);
        paint.setPen(pen);
        paint.setRasterOp(Qt::XorROP);

        if (mCanvasProperties->rubberStartPoint != mCanvasProperties->rubberStopPoint)
        {
          // XOR-out the old line
          paint.drawLine(mCanvasProperties->rubberStartPoint, mCanvasProperties->rubberStopPoint);
        }  

        mCanvasProperties->rubberStopPoint = e->pos();

        paint.drawLine(mCanvasProperties->rubberStartPoint, mCanvasProperties->rubberStopPoint);
        paint.end();
#endif

      }

      break;
  }          

  //draw a line to the cursor position in line/polygon editing mode
  if ( mCanvasProperties->mapTool == QGis::CaptureLine || mCanvasProperties->mapTool == QGis::CapturePolygon )
  {
    if(mCaptureList.size()>0)
    {
      QPainter paint(mMapImage->pixmap());
      QPainter paint2(this);

      drawLineToDigitisingCursor(&paint);
      drawLineToDigitisingCursor(&paint2);
      if(mCanvasProperties->mapTool == QGis::CapturePolygon && mCaptureList.size()>1)
      {
        drawLineToDigitisingCursor(&paint, false);
        drawLineToDigitisingCursor(&paint2, false);
      }
      QgsPoint digitmovepoint(e->pos().x(), e->pos().y());
      mDigitMovePoint=mCanvasProperties->coordXForm->toMapCoordinates(e->pos().x(), e->pos().y());

      drawLineToDigitisingCursor(&paint);
      drawLineToDigitisingCursor(&paint2);
      if(mCanvasProperties->mapTool == QGis::CapturePolygon && mCaptureList.size()>1)
      {
        drawLineToDigitisingCursor(&paint, false);
        drawLineToDigitisingCursor(&paint2, false);
      }
    }
}

  // show x y on status bar
  QPoint xy = e->pos();
  QgsPoint coord = mCanvasProperties->coordXForm->toMapCoordinates(xy);
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
  QgsPoint lastpoint = mCanvasProperties->coordXForm->transform(it->x(),it->y());
  QgsPoint digitpoint = mCanvasProperties->coordXForm->transform(mDigitMovePoint.x(), mDigitMovePoint.y());
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
  QgsPoint center  = mCanvasProperties->coordXForm->toMapPoint(x, y);
  QgsRect r = mMapImage->extent();
  r.scale(scaleFactor, &center);
  setExtent(r);
  refresh();
}


/** Sets the map tool currently being used on the canvas */
void QgsMapCanvas::setMapTool(int tool)
{
  mCanvasProperties->mapTool = tool;
  if ( tool == QGis::EmitPoint ) {
    setCursor ( Qt::CrossCursor );
  }
  if(tool == QGis::CapturePoint)
  {
    mLineEditing=false;
    mPolygonEditing=false;
  }
  else if (tool == QGis::CaptureLine)
  {
    mLineEditing=true;
    mPolygonEditing=false;
  }
  else if (tool == QGis::CapturePolygon)
  {
    mLineEditing=false;
    mPolygonEditing=true;
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
  
  if (!mCanvasProperties->frozen)
    refresh();

} // layerStateChange



void QgsMapCanvas::freeze(bool frz)
{
  mCanvasProperties->frozen = frz;
} // freeze

bool QgsMapCanvas::isFrozen()
{
  return mCanvasProperties->frozen ;
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
  return mCanvasProperties->mapTool;
}

void QgsMapCanvas::panActionEnd(QPoint releasePoint)
{
  mCanvasProperties->panning = FALSE;

  // use start and end box points to calculate the extent
  QgsPoint start = mCanvasProperties->coordXForm->toMapCoordinates(mCanvasProperties->rubberStartPoint);
  QgsPoint end = mCanvasProperties->coordXForm->toMapCoordinates(releasePoint);

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
  //  std::cout << "QgsMapCanvas::panAction: entering." << std::endl;
#endif

  mCanvasProperties->panning = TRUE;

  // bitBlt the pixmap on the screen, offset by the
  // change in mouse coordinates
  //  double dx = e->pos().x() - mCanvasProperties->rubberStartPoint.x();
  //  double dy = e->pos().y() - mCanvasProperties->rubberStartPoint.y();
  //  mCanvasProperties->pan_dx = e->pos().x() - mCanvasProperties->rubberStartPoint.x();
  //  mCanvasProperties->pan_dy = e->pos().y() - mCanvasProperties->rubberStartPoint.y();
  mCanvasProperties->pan_delta = e->pos() - mCanvasProperties->rubberStartPoint;

  /*
     Do we still need this in Qt4?
  //erase only the necessary parts to avoid flickering
  if (dx > 0)
  {
  erase(0, 0, (int)dx, height());
  }
  else
  {
  erase(width() + (int)dx, 0, -(int)dx, height());
  }
  if (dy > 0)
  {
  erase(0, 0, width(), (int)dy);
  }
  else
  {
  erase(0, height() + (int)dy, width(), -(int)dy);
  }
  */

#ifdef QGISDEBUG
  //  std::cout << "QgsMapCanvas::panAction: about to update with " << mCanvasProperties->pan_dx << 
  //                                                        " and " << mCanvasProperties->pan_dy << "." << std::endl;
#endif

  // void bitBlt ( QPaintDevice * dst, int dx, int dy, const QPaintDevice * src, int sx, int sy, int sw, int sh, Qt::RasterOp rop, bool ignoreMask ) 

  //  bitBlt(this, (int)dx, (int)dy, mCanvasProperties->pmCanvas);

  /*
     Qt4 prefers:
     void QPainter::drawPixmap ( const QRect & targetRect, const QPixmap & pixmap, const QRect & sourceRect )
     This is an overloaded member function, provided for convenience. It behaves essentially like the above function.
     Draws the rectangular portion sourceRect of the pixmap pixmap in the rectangle targetRect.
     */

  //  QPainter paint(this);
  //  paint.drawPixmap( (int)dx, (int)dy, *(mCanvasProperties->pmCanvas));
  //  paint.end();

  // Since in Qt4 you normally cannot paint outside of a paint event,
  // let the widget fire one off by calling repaint() 
  // and let the subsequent paintEvent() call do all the heavy lifting there.
  repaint();

#ifdef QGISDEBUG
  //  std::cout << "QgsMapCanvas::panAction: exiting." << std::endl;
#endif

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
