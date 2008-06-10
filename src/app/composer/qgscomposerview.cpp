/***************************************************************************
                         qgscomposerview.cpp
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
#include <iostream>

#include <QMatrix>
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>

#include "qgsrect.h"
#include "qgscomposer.h"
#include "qgscomposeritem.h"
#include "qgscomposerview.h"
#include "qgscomposermap.h"

// Note: |WRepaintNoErase|WResizeNoErase|WStaticContents doeen't make it faster
QgsComposerView::QgsComposerView( QgsComposer *composer, QWidget* parent, const char* name, Qt::WFlags f) :
  QGraphicsView(parent), mShiftKeyPressed(false)
//,name,f|Qt::WNoAutoErase|Qt::WResizeNoErase|Qt::WStaticContents
{
    mComposer = composer;
    setResizeAnchor ( QGraphicsView::AnchorViewCenter );
    setMouseTracking(true);  
    viewport()->setMouseTracking ( true );
}

void QgsComposerView::mousePressEvent(QMouseEvent* e)
{
  mComposer->composition()->mousePressEvent(e, mShiftKeyPressed);
  if(mComposer->composition()->tool() == QgsComposition::Select)
    {
      QGraphicsView::mousePressEvent(e);
    }
}

void QgsComposerView::mouseReleaseEvent(QMouseEvent* e)
{
    mComposer->composition()->mouseReleaseEvent(e);
    if(mComposer->composition()->tool() == QgsComposition::Select)
      {
	QGraphicsView::mouseReleaseEvent(e);
      }
}

void QgsComposerView::mouseMoveEvent(QMouseEvent* e)
{
  qWarning("QgsComposerView::mouseMoveEvent");
  mComposer->composition()->mouseMoveEvent(e);
  if(mComposer->composition()->tool() == QgsComposition::Select)
    {
      QGraphicsView::mouseMoveEvent(e);
    }
}

void QgsComposerView::keyPressEvent ( QKeyEvent * e )
{
  if(e->key() == Qt::Key_Shift)
    {
      mShiftKeyPressed = true;
    }
  mComposer->composition()->keyPressEvent ( e );
}

void QgsComposerView::keyReleaseEvent ( QKeyEvent * e )
{
  if(e->key() == Qt::Key_Shift)
    {
      mShiftKeyPressed = false;
    }
}

void QgsComposerView::resizeEvent ( QResizeEvent *  )
{
#ifdef QGISDEBUG
  std::cout << "QgsComposerView::resizeEvent()" << std::endl;
#endif

/* BUG: When QT adds scrollbars because we're zooming in, it causes a resizeEvent.
 *  If we call zoomFull(), we reset the view size, which keeps us from zooming in.
 *  Really, we should do something like re-center the window.
*/
    //mComposer->zoomFull();
}

//TODO: add mouse wheel event forwarding

