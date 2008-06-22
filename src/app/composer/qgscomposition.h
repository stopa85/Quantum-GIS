/***************************************************************************
                              qgscomposition.h 
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
#ifndef QGSCOMPOSITION_H
#define QGSCOMPOSITION_H

#include <QGraphicsScene>

class QgsComposerItem;
class QgsComposerMap;
class QGraphicsRectItem;
class QgsMapCanvas;

/**Graphics scene for map printing. It manages the paper item which always is the item in the back (z-value 0)*/
class QgsComposition: public QGraphicsScene
{
 public:

  /** \brief Plot type */
  enum PlotStyle 
    {
      Preview = 0, // Use cache etc
      Print,       // Render well
      Postscript   // Fonts need different scaling!
    };

  QgsComposition(QgsMapCanvas* mapCanvas);
  ~QgsComposition();

  /**Changes size of paper item*/
  void setPaperSize(double width, double height);

  /**Returns height of paper item*/
  double paperHeight() const;

  /**Returns width of paper item*/
  double paperWidth() const;

  /**Returns the topmose composer item. Ignores mPaperItem*/
  QgsComposerItem* composerItemAt(const QPointF & position);

  QList<QgsComposerItem*> selectedComposerItems();

  /**Returns pointers to all composer maps in the scene*/
  QList<const QgsComposerMap*> composerMapItems() const;

  /**Returns the composer map with specified id
   @return id or 0 pointer if the composer map item does not exist*/
  const QgsComposerMap* getComposerMapById(int id) const;

  /**Returns pointer to qgis map canvas*/
  QgsMapCanvas* mapCanvas(){return mMapCanvas;}

  QgsComposition::PlotStyle plotStyle() const {return mPlotStyle;}
  void setPlotStyle(QgsComposition::PlotStyle style) {mPlotStyle = style;}

 private:
  QgsMapCanvas* mMapCanvas;
  QgsComposition::PlotStyle mPlotStyle;
  QGraphicsRectItem* mPaperItem;

  QgsComposition(); //default constructor is forbidden
};

#endif 



