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

QgsComposerLabel::QgsComposerLabel( QgsComposition *composition, int id): QgsComposerItem(0)
{
  mId = id;
}

QgsComposerLabel::~QgsComposerLabel()
{
  //todo
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
  painter->drawText(rect(), Qt::AlignLeft, mText, 0);

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
  QFontMetricsF fontInfo(mFont);
  setSceneRect(QRectF(transform().dx(), transform().dy(), fontInfo.width(mText), fontInfo.xHeight()));
}


