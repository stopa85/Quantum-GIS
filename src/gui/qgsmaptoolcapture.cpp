/***************************************************************************
    qgsmaptoolcapture.cpp  -  map tool for capturing points, lines, polygons
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

#include "qgsmaptoolcapture.h"
#include "qgsmapcanvas.h"
#include "qgsmaptopixel.h"
#include "qgsvectorlayer.h"

#include <QMessageBox>

/*
TODO: make it work :)
*/


QgsMapToolCapture::QgsMapToolCapture(QgsMapCanvas* canvas, enum QGis::MapTools tool)
  : QgsMapTool(canvas), mTool(tool)
{
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
}


void QgsMapToolCapture::mouseReleaseEvent(QMouseEvent * e)
{
  // TODO: if not dragging

  QgsVectorLayer *vlayer = dynamic_cast < QgsVectorLayer * >(mCanvas->currentLayer());
  
  if (!vlayer)
  {
    QMessageBox::information(0,"Not a vector layer","The current layer is not a vector layer",QMessageBox::Ok);
    return;
  }
  
  if (!vlayer->isEditable())
  {
    QMessageBox::information(0,"Layer not editable",
                             "Cannot edit the vector layer. Use 'Start editing' in the legend item menu",
                             QMessageBox::Ok);
    return;
  }
  /*
  // POINT CAPTURING
  if (mTool == CapturePoint)
  {

    QgsPoint  idPoint = getCoordinateTransform()->toMapCoordinates(e->x(), e->y());
    emit xyClickCoordinates(idPoint);

            //only do the rest for provider with feature addition support
            //note that for the grass provider, this will return false since
            //grass provider has its own mechanism of feature addition
    if(vlayer->getDataProvider()->capabilities()&QgsVectorDataProvider::AddFeatures)
    {

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

  // LINE & POLYGON CAPTURING
  if (mTool == CaptureLine || mTool == CapturePolygon)
  {

    if (mRubberStartPoint != mRubberStopPoint)
    {
              // TODO: Qt4 will have to do this a different way, using QRubberBand ...
  #if QT_VERSION < 0x040000
              // XOR-out the old line
              paint.drawLine(mRubberStartPoint, mRubberStopPoint);
  #endif
    }
  
    mRubberStopPoint = e->pos();
  
    //prevent clearing of the line between the first and the second polygon vertex
    //during the next mouse move event
    if(mCaptureList.size() == 1 && mMapTool == QGis::CapturePolygon)
    {
      QPainter paint(mMapImage->pixmap());
      drawLineToDigitisingCursor(&paint);
    }
  
    mDigitMovePoint.setX(e->x());
    mDigitMovePoint.setY(e->y());
    mDigitMovePoint=getCoordinateTransform()->toMapCoordinates(e->x(), e->y());
    QgsPoint digitisedpoint=getCoordinateTransform()->toMapCoordinates(e->x(), e->y());
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
        QgsPoint lastpoint = getCoordinateTransform()->transform(it->x(),it->y());
        QgsPoint endpoint = getCoordinateTransform()->transform(digitisedpoint.x(),digitisedpoint.y());
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
  
      mCapturing = FALSE;
  
              //create QgsFeature with wkb representation
      QgsFeature* f=new QgsFeature(0,"WKBLineString");
      unsigned char* wkb;
      int size;
      if(mMapTool==QGis::CaptureLine)
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
      mCapturing = TRUE;
    }
    break;
  }  
  */
} // mouseReleaseEvent



void QgsMapToolCapture::mouseMoveEvent(QMouseEvent * e)
{
  /*
  if (mCapturing)
  {
      // show the rubber-band from the last click

    QPainter paint;
    QPen pen(Qt::gray);

    // TODO: Qt4 will have to do this a different way, using QRubberBand ...
#if QT_VERSION < 0x040000
    paint.begin(this);
    paint.setPen(pen);
    paint.setRasterOp(Qt::XorROP);

    if (mRubberStartPoint != mRubberStopPoint)
    {
      // XOR-out the old line
      paint.drawLine(mRubberStartPoint, mRubberStopPoint);
    }  

    mRubberStopPoint = e->pos();

    paint.drawLine(mRubberStartPoint, mRubberStopPoint);
    paint.end();
#endif

  }

  // draw a line to the cursor position in line/polygon editing mode
  if (mTool == CaptureLine || mTool == CapturePolygon)
  {
    if(mCaptureList.size()>0)
    {
      QPainter paint(mMapImage->pixmap());
      QPainter paint2(this);
  
      drawLineToDigitisingCursor(&paint);
      drawLineToDigitisingCursor(&paint2);
      if(mMapTool == QGis::CapturePolygon && mCaptureList.size()>1)
      {
        drawLineToDigitisingCursor(&paint, false);
        drawLineToDigitisingCursor(&paint2, false);
      }
      QgsPoint digitmovepoint(e->pos().x(), e->pos().y());
      mDigitMovePoint=getCoordinateTransform()->toMapCoordinates(e->pos().x(), e->pos().y());
  
      drawLineToDigitisingCursor(&paint);
      drawLineToDigitisingCursor(&paint2);
      if(mMapTool == QGis::CapturePolygon && mCaptureList.size()>1)
      {
        drawLineToDigitisingCursor(&paint, false);
        drawLineToDigitisingCursor(&paint2, false);
      }
    }
  }
  */
} // mouseMoveEvent

//////////////////

void QgsMapToolCapture::mousePressEvent(QMouseEvent * e)
{
}
