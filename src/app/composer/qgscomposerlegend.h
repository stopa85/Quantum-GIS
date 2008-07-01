/***************************************************************************
                         qgscomposerlegend.h  -  description
                         -------------------
    begin                : June 2008
    copyright            : (C) 2008 by Marco Hugentobler
    email                : marco dot hugentobler at karto dot baug dot ethz dot ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSCOMPOSERLEGEND_H
#define QGSCOMPOSERLEGEND_H

#include "qgscomposeritem.h"
#include "qgslegendmodel.h"

class QgsComposerLegend: public QgsComposerItem
{
 public:
  QgsComposerLegend(QgsComposition* composition);
  ~QgsComposerLegend();

  /** \brief Reimplementation of QCanvasItem::paint*/
  void paint (QPainter* painter, const QStyleOptionGraphicsItem* itemStyle, QWidget* pWidget);

  /**Sets item box to the whole content*/
  void adjustBoxSize();

  /**Returns pointer to the legend model*/
  QgsLegendModel* model(){return &mLegendModel;}

  void setTitle(const QString& t) {mTitle = t;}
  QString title() const {return mTitle;}

 protected:
  QString mTitle;

  //different fonts for entries
  QFont mTitleFont;
  QFont mLayerFont;
  QFont mItemFont;

  /**Space between item box and contents*/
  double mBoxSpace;
  /**Vertical space between layer entries*/
  double mLayerSpace;
  /**Vertical space between symbol entries*/
  double mSymbolSpace;
  /**Horizontal space between item icon and label*/
  double mIconLabelSpace;
  /**Width of symbol icon*/
  double mSymbolWidth;
  /**Height of symbol icon*/
  double mSymbolHeight;

  QgsLegendModel mLegendModel;


 private:
  QgsComposerLegend(); //forbidden

  /**Draws child items of a layer item
     @param layerItem parent model item (layer)
     @param currentYCoord in/out: current y position of legend item*/
  void drawLayerChildItems(QPainter* p, QStandardItem* layerItem, double& currentYCoord);
};

#endif
