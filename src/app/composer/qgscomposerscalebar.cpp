/***************************************************************************
                           qgscomposerscalebar.cpp
                             -------------------
    begin                : March 2005
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

#include "qgscomposerscalebar.h"
#include "qgscomposermap.h"
#include "qgsscalebarstyle.h"
#include "qgsdoubleboxscalebarstyle.h"
#include "qgsnumericscalebarstyle.h"
#include "qgssingleboxscalebarstyle.h"
#include "qgsticksscalebarstyle.h"
#include "qgsrect.h"
#include <QFontMetricsF>
#include <QPainter>
#include <cmath>

QgsComposerScaleBar::QgsComposerScaleBar(QgsComposition* composition): QgsComposerItem(composition), mComposerMap(0), mStyle(0), mSegmentMM(0.0)
{
  applyDefaultSettings();
}

QgsComposerScaleBar::~QgsComposerScaleBar()
{
  delete mStyle;
}

void QgsComposerScaleBar::paint (QPainter* painter, const QStyleOptionGraphicsItem* itemStyle, QWidget* pWidget)
{
  if(!mStyle)
    {
      return;
    }

  //calculate half of first label width as labels are drawn centered
  QFontMetricsF fontMetrics(mFont);
  QString firstLabel = firstLabelString();

  mStyle->draw(painter, fontMetrics.width(firstLabel) / 2);
  
  //draw frame and selection boxes if necessary
  drawFrame(painter);
  if(isSelected())
    {
      drawSelectionBoxes(painter);
    }
}

void QgsComposerScaleBar::setNumUnitsPerSegment(double units)
{
  mNumUnitsPerSegment = units;
  refreshSegmentMM();
}

void QgsComposerScaleBar::setComposerMap(const QgsComposerMap* map)
{
  disconnect(mComposerMap, SIGNAL(extentChanged()), this, SLOT(updateSegmentSize()));
  disconnect(mComposerMap, SIGNAL(destroyed(QObject*)), this, SLOT(invalidateCurrentMap()));
  mComposerMap = map;

  if(!map)
    {
      return;
    }

  connect(mComposerMap, SIGNAL(extentChanged()), this, SLOT(updateSegmentSize()));
  connect(mComposerMap, SIGNAL(destroyed(QObject*)), this, SLOT(invalidateCurrentMap()));
  
  refreshSegmentMM();
}

void QgsComposerScaleBar::invalidateCurrentMap()
{
  disconnect(mComposerMap, SIGNAL(extentChanged()), this, SLOT(updateSegmentSize()));
  disconnect(mComposerMap, SIGNAL(destroyed(QObject*)), this, SLOT(invalidateCurrentMap()));
  mComposerMap = 0;
}

void QgsComposerScaleBar::refreshSegmentMM()
{
  if(mComposerMap)
    {
      //get extent of composer map
      QgsRect composerMapRect = mComposerMap->extent();

      //get mm dimension of composer map
      QRectF composerItemRect = mComposerMap->rect();

      //calculate size depending on mNumUnitsPerSegment
      mSegmentMM = composerItemRect.width() / composerMapRect.width() * mNumUnitsPerSegment;
    }
}

void QgsComposerScaleBar::applyDefaultSettings()
{
  mNumSegments = 2;
  mNumSegmentsLeft = 0;

  mNumMapUnitsPerScaleBarUnit = 1.0;

  //style
  delete mStyle;
  mStyle = new QgsSingleBoxScaleBarStyle(this);

  mHeight = 5;

  mPen = QPen(QColor(0, 0, 0));
  mPen.setWidthF(1.0);

  mBrush.setColor(QColor(0, 0, 0));
  mBrush.setStyle(Qt::SolidPattern);

  mFont.setPointSizeF(4);

  mLabelBarSpace = 3.0;
  mBoxContentSpace = 1.0;

  if(mComposerMap)
    {
      //calculate mNumUnitsPerSegment
      QRectF composerItemRect = mComposerMap->rect();
      QgsRect composerMapRect = mComposerMap->extent();

      double proposedScaleBarLength = composerMapRect.width() /4;
      int powerOf10 = int (pow(10.0, int (log(proposedScaleBarLength) / log(10.0)))); // from scalebar plugin
      int nPow10 = proposedScaleBarLength / powerOf10;
      mNumSegments = 2;
      mNumUnitsPerSegment = (nPow10 / 2) * powerOf10;
    }

  refreshSegmentMM();
  adjustBoxSize();
}

void QgsComposerScaleBar::adjustBoxSize()
{
  if(!mStyle)
    {
      return;
    }
  
  QRectF box = mStyle->calculateBoxSize();
  setSceneRect(box);
}

void QgsComposerScaleBar::update()
{
  adjustBoxSize();
  QgsComposerItem::update();
}

double QgsComposerScaleBar::fontHeight() const
{
  QFontMetricsF labelFontMetrics(mFont);
  return labelFontMetrics.ascent();
}

void QgsComposerScaleBar::updateSegmentSize()
{
  refreshSegmentMM();
  update();
}

void QgsComposerScaleBar::segmentPositions(QList<QPair<double, double> >& posWidthList) const
{
  posWidthList.clear();
  double mCurrentXCoord = mPen.widthF() + mBoxContentSpace;
  
  //left segments
  for(int i = 0; i < mNumSegmentsLeft; ++i)
    {
      posWidthList.push_back(qMakePair(mCurrentXCoord, mSegmentMM / mNumSegmentsLeft));
      mCurrentXCoord += mSegmentMM / mNumSegmentsLeft;
    }

  //right segments
  for(int i = 0; i < mNumSegments; ++i)
    {
      posWidthList.push_back(qMakePair(mCurrentXCoord, mSegmentMM));
      mCurrentXCoord += mSegmentMM;
    }
}

void QgsComposerScaleBar::setStyle(const QString& styleName)
{
  delete mStyle;
  mStyle = 0;

  //switch depending on style name
  if(styleName == tr("Single Box"))
    {
      mStyle = new QgsSingleBoxScaleBarStyle(this);
    }
  else if(styleName == tr("Double Box"))
    {
      mStyle = new QgsDoubleBoxScaleBarStyle(this);
    }
  else if(styleName == tr("Line Ticks Middle") || styleName == tr("Line Ticks Down") || styleName == tr("Line Ticks Up"))
    {
      QgsTicksScaleBarStyle* tickStyle = new QgsTicksScaleBarStyle(this);
      if(styleName == tr("Line Ticks Middle"))
	{
	  tickStyle->setTickPosition(QgsTicksScaleBarStyle::MIDDLE);
	}
      else if(styleName == tr("Line Ticks Down"))
	{
	  tickStyle->setTickPosition(QgsTicksScaleBarStyle::DOWN);
	}
      else if(styleName == tr("Line Ticks Up"))
	{
	  tickStyle->setTickPosition(QgsTicksScaleBarStyle::UP);
	}
      mStyle = tickStyle;
    } 
  else if(styleName == tr("Numeric"))
    {
      mStyle = new QgsNumericScaleBarStyle(this);
    }
}

QString QgsComposerScaleBar::firstLabelString() const
{
  if(mNumSegmentsLeft > 0)
    {
      return QString::number(mNumUnitsPerSegment / mNumMapUnitsPerScaleBarUnit);
    }
  else
    {
      return "0";
    }
}

bool QgsComposerScaleBar::writeXML(QDomElement& elem, QDomDocument & doc)
{
  return true;
}


