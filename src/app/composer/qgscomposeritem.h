/***************************************************************************
                         qgscomposeritem.h
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
#ifndef QGSCOMPOSERITEM_H
#define QGSCOMPOSERITEM_H

#include "qgscomposition.h"
#include <QGraphicsRectItem>

class QWidget;
class QDomNode;
class QDomDocument;

class QqsComposition;

class QgsComposerItem: public QGraphicsRectItem
{

public:

  /**Describes the action (move or resize in different directon) to be done during mouse move*/
  enum mouseMoveAction
    {
      moveItem,
      resizeUp,
      resizeDown,
      resizeLeft,
      resizeRight,
      resizeDLeftUp,
      resizeDRightUp,
      resizeDLeftDown,
      resizeDRightDown
    };

    QgsComposerItem(QGraphicsItem* parent = 0);
    QgsComposerItem(qreal x, qreal y, qreal width, qreal height,QGraphicsItem* parent = 0); 
    virtual ~QgsComposerItem(); 

    /** \brief Set plot style */
    void setPlotStyle ( QgsComposition::PlotStyle p );

    /** \brief get plot style */
    QgsComposition::PlotStyle plotStyle ( void );

    /** \brief Set selected, selected item should be highlighted */
    virtual void setSelected( bool s );

    /** \brief Is selected */
    virtual bool selected( void );
    
    /** item id */
    int id ( void );

    /** Update otions and return pointer to options widget */
    virtual QWidget *options ( void );

    /** Create option controls
     * @param parent widget where options are created
     */
    virtual void showOptions ( QWidget * parent );

    /** stores state in project */
    virtual bool writeSettings ( void );

    /** read state from project */
    virtual bool readSettings ( void );

    /** delete settings from project file  */
    virtual bool removeSettings( void );

    /** resizes an item in x- and y direction (canvas coordinates)*/
    virtual void resize(double dx, double dy){}

    /** stores state in DOM node
     * @param node is DOM node corresponding to '???' tag
     * @param temp write template file
     */
    virtual bool writeXML( QDomNode & node, QDomDocument & doc, bool templ = false );

    /** sets state from DOM document
     * @param node is DOM node corresponding to '???' tag
     */
    virtual bool readXML( QDomNode & node );

protected:
    QgsComposition::PlotStyle mPlotStyle;
    bool mSelected;
    int mId;
    QgsComposerItem::mouseMoveAction mCurrentMouseMoveAction;
    /**Start point of the last mouse move action (in scene coordinates)*/
    QPointF mMouseMoveStartPos;
    /**Position of the last mouse move event (in item coordinates)*/
    QPointF mLastMouseEventPos;

    /**Rectangle used during move and resize actions*/
    QGraphicsRectItem* mBoundingResizeRectangle;
 
    //event handlers
    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
    
    virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
    virtual void hoverMoveEvent ( QGraphicsSceneHoverEvent * event );

    /**Finds out the appropriate cursor for the current mouse position in the widget (e.g. move in the middle, resize at border)*/
    Qt::CursorShape cursorForPosition(const QPointF& itemCoordPos);

    /**Finds out which mouse move action to choose depending on the cursor position inside the widget*/
    QgsComposerItem::mouseMoveAction mouseMoveActionForPosition(const QPointF& itemCoordPos);
    
    /**Calculate rectangle changes according to mouse move (dx, dy) and the current mouse move action
       @param dx x-coordinate move of cursor
       @param dy y-coordinate move of cursor
       @param mx out: rectangle should be moved by mx in x-direction
       @param my out: rectangle should be moved by my in y-direction
       @param rx out: width of rectangle should be resized by rx
       @param ry out: height of rectangle should be resized by ry*/
    void rectangleChange(double dx, double dy, double& mx, double& my, double& rx, double& ry) const;
};

#endif
