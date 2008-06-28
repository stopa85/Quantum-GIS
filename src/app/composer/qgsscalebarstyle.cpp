/***************************************************************************
                            qgsscalebarstyle.cpp
                            --------------------
    begin                : June 2008
    copyright            : (C) 2008 by Marco Hugentobler
    email                : marco.hugentobler@karto.baug.ethz.ch
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsscalebarstyle.h"
#include "qgscomposerscalebar.h"
#include <QFontMetricsF>
#include <QPainter>

QgsScaleBarStyle::QgsScaleBarStyle(const QgsComposerScaleBar* bar):  mScaleBar(bar)
{

}

QgsScaleBarStyle::QgsScaleBarStyle(): mScaleBar(0)
{

}
 
QgsScaleBarStyle::~QgsScaleBarStyle()
{

}

void QgsScaleBarStyle::drawLabels(QPainter* p) const
{
  if(!p || !mScaleBar)
    {
      return;
    }

  p->save();

  p->setFont(mScaleBar->font());

  //double mCurrentXCoord = mScaleBar->pen().widthF() + mScaleBar->boxContentSpace();
  QList<QPair<double, double> > segmentInfo;
  mScaleBar->segmentPositions(segmentInfo);

  double currentLabelNumber = 0.0;

  int nSegmentsLeft = mScaleBar->numSegmentsLeft();
  int segmentCounter = 0;

  QList<QPair<double, double> >::const_iterator segmentIt = segmentInfo.constBegin();
  for(; segmentIt != segmentInfo.constEnd(); ++segmentIt)
    {
      if(segmentCounter == 0 && nSegmentsLeft > 0)
	{
	  //label first left segment
	  p->drawText(QPointF(segmentIt->first, mScaleBar->fontHeight() + mScaleBar->boxContentSpace()), QString::number(mScaleBar->numUnitsPerSegment() / mScaleBar->numMapUnitsPerScaleBarUnit()));
	}

      if(segmentCounter >= nSegmentsLeft)
	{
	  p->drawText(QPointF(segmentIt->first, mScaleBar->fontHeight() + mScaleBar->boxContentSpace()), QString::number(currentLabelNumber / mScaleBar->numMapUnitsPerScaleBarUnit()));
	  currentLabelNumber += mScaleBar->numUnitsPerSegment();
	}
      ++segmentCounter;
    }

  //also draw the last label
  if(!segmentInfo.isEmpty())
    {
      p->drawText(QPointF(segmentInfo.last().first + mScaleBar->segmentMM(), mScaleBar->fontHeight() + mScaleBar->boxContentSpace()), QString::number(currentLabelNumber / mScaleBar->numMapUnitsPerScaleBarUnit()) + " " + mScaleBar->unitLabeling()); 
    }

  p->restore();
}

QRectF QgsScaleBarStyle::calculateBoxSize() const
{
  if(!mScaleBar)
    {
      return QRectF();
    }

  //consider width of largest label
  double largestLabelNumber = mScaleBar->numSegments() * mScaleBar->numUnitsPerSegment() / mScaleBar->numMapUnitsPerScaleBarUnit();
  QString largestLabel = QString::number(largestLabelNumber) + " " + mScaleBar->unitLabeling();
  double largestLabelWidth = QFontMetricsF(mScaleBar->font()).width(largestLabel);

  //add all the segment length
  double totalBarLength = 0;
  
  QList< QPair<double, double> > segmentList;
  mScaleBar->segmentPositions(segmentList);

  QList< QPair<double, double> >::const_iterator segmentIt = segmentList.constBegin();
  for(; segmentIt != segmentList.constEnd(); ++segmentIt)
    {
      totalBarLength += segmentIt->second;
    }

  double width =  totalBarLength + 2 * mScaleBar->pen().widthF() + largestLabelWidth + 2 * mScaleBar->boxContentSpace();
  double height = mScaleBar->height() + mScaleBar->labelBarSpace() + 2 * mScaleBar->boxContentSpace() + mScaleBar->fontHeight();

  return QRectF(mScaleBar->transform().dx(), mScaleBar->transform().dy(), width, height);
}
