
#include <QPainter>
#include <QPainterPath>

#include "positionmarker.h"


PositionMarker::PositionMarker(QgsMapCanvas* mapCanvas)
  : QgsMapCanvasItem(mapCanvas)
{
  mIconSize = 20;
  mHasPosition = false;
  mAngle = 0;
  setZ(100); // be on top
}

void PositionMarker::setIcon(bool hasPosition)
{
  mHasPosition = hasPosition;
}

void PositionMarker::setAngle(double angle)
{    
  mAngle = angle;
}

/*    
void PositionMarker::setIconSize(int iconSize)
{
  mIconSize = iconSize;
}
*/
  
void PositionMarker::setPosition(const QgsPoint& point)
{
  if (mPosition != point)
  {
    mPosition = point;
    updatePosition();
    updateCanvas();
  }
}


void PositionMarker::drawShape(QPainter & p)
{
  QPoint pt = toCanvasCoords(mPosition);
  int x = pt.x(), y = pt.y();
  
  
  // draw cross
  p.setPen(QColor(255,0,0));
  //if (mHasPosition)
  {
    QPainterPath path;
    path.moveTo(0,-10);
    path.lineTo(10,10);
    path.lineTo(0,5);
    path.lineTo(-10,10);
    path.lineTo(0,-10);
    
    // render position with angle
    p.save();
    p.setRenderHint(QPainter::Antialiasing);
    if (mHasPosition)
      p.setBrush(QBrush(QColor(0,0,0)));
    else
      p.setBrush(QBrush(QColor(200,200,200)));
    p.setPen(QColor(255,255,255));
    p.translate(x,y);
    p.rotate(mAngle);
    p.drawPath(path);
    p.restore();
  }
/*  else
  {
    // TODO: draw '?' to show that we don't have position
}*/
}
    

void PositionMarker::updatePosition()
{
  QPoint pt = toCanvasCoords(mPosition);
  int s = (mIconSize - 1) / 2;
  move(pt.x() - s, pt.y() - s);
  setSize(mIconSize, mIconSize);
  show();
}
