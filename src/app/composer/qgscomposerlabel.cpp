/***************************************************************************
                         qgscomposerlabel.cpp
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

#include "qgscomposerlabel.h"
#include <QPainter>

QgsComposerLabel::QgsComposerLabel( QgsComposition *composition): QgsComposerItem(composition), mMargin(0.0)
{
}

QgsComposerLabel::~QgsComposerLabel()
{
}

void QgsComposerLabel::paint(QPainter* painter, const QStyleOptionGraphicsItem* itemStyle, QWidget* pWidget)
{
  if(!painter)
    {
      return;
    }
  
  //scale font considering resolution of output device and 1 point equals 0.376 mm
  QFont outputFont(mFont);
  outputFont.setPointSizeF(mFont.pointSizeF() * fontPointScaleFactor(painter));

  painter->setFont(outputFont);
  painter->drawText(QRectF(mMargin, mMargin, rect().width(), rect().height()), Qt::AlignLeft, mText, 0);

  drawFrame(painter);
  if(isSelected())
    {
      drawSelectionBoxes(painter);
    }
}

void QgsComposerLabel::setFontSize(double size)
{
  mFont.setPointSizeF(size);
}

void QgsComposerLabel::adjustSizeToText()
{
  //find out width and hight mText and set bounding box to it
  QFont scaledFont(mFont);
  scaledFont.setPointSizeF(mFont.pointSizeF() * (25.4 / 120 / 0.376)); //how to find out the 120 dpi of QGraphicsView?


  QFontMetricsF fontInfo(scaledFont);
  setSceneRect(QRectF(transform().dx(), transform().dy(), fontInfo.width(mText) + 2 * mMargin, fontInfo.ascent() + 2 * mMargin));
}


