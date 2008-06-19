/***************************************************************************
                         qgscomposerlabel.h
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
#ifndef QGSCOMPOSERLABEL_H
#define QGSCOMPOSERLABEL_H

#include "qgscomposeritem.h"

class QgsComposerLabel: public QgsComposerItem
{
 public:
  QgsComposerLabel( QgsComposition *composition);
  ~QgsComposerLabel();

  /** \brief Reimplementation of QCanvasItem::paint*/
  void paint (QPainter* painter, const QStyleOptionGraphicsItem* itemStyle, QWidget* pWidget);

  /**resizes the widget such that the text fits to the item. Keeps top left point*/
  void adjustSizeToText();

  QString text() {return mText;}
  void setText(const QString& text){mText = text;}
  QFont font() {return mFont;}
  void setFont(const QFont& f){mFont = f;}
  void setFontSize(double size);
  double margin(){return mMargin;}
  void setMargin(double m){mMargin = m;}
 
 private:
  // Text 
    QString mText;

    // Font
    QFont mFont;

    // Border between text and fram (in mm)
    double mMargin;
};

#endif 


