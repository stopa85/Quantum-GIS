/***************************************************************************
                         qgscomposeritemgroup.h
                         ----------------------
    begin                : 2nd June 2008
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

#include "qgscomposeritem.h"
#include <QSet>
class QgsComposition;

class QgsComposerItemGroup: public QgsComposerItem
{
 public:
  QgsComposerItemGroup(QgsComposition* c, QGraphicsItem* parent = 0);
  ~QgsComposerItemGroup();
  /**Adds an item to the group. All the group members are deleted 
   if the group is deleted*/
  void addItem(QgsComposerItem* item);
  /**Removes the items but does not delete them*/
  void removeItems();
  /**Draw outline and ev. selection handles*/
  void paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
  /**Moves all contained items by dx/dy*/
  void move(double dx, double dy);
  /**Sets this items bound in scene coordinates such that 1 item size units
     corresponds to 1 scene size unit*/
  void setSceneRect(const QRectF& rectangle);
  /** resizes an item in x- and y direction (scene coordinates)*/
  void resize(double dx, double dy);

 protected:
  void drawFrame(QPainter* p);

 private:
  QSet<QgsComposerItem*> mItems;
  QgsComposition* mComposition;
  QRectF mSceneBoundingRectangle;
};


