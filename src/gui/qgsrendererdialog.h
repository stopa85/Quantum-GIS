/***************************************************************************
                         qgsrendererdialog.h  -  description
                             -------------------
    begin                : April 2006
    copyright            : (C) 2006 by Marco Hugentobler
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

#ifndef QGSRENDERERDIALOG_H
#define QGSRENDERERDIALOG_H

#include <QDialog>
class QgsVectorLayer;

/**Interface class for dialogs taking input for vector renderers.
Every renderer dialog needs to overwrite the apply method where it
generates a concrete renderer class and attaches it to mVectorLayer*/
class QgsRendererDialog: public QDialog
{
 public:
  QgsRendererDialog(): QDialog(), mVectorLayer(0) {}
  QgsRendererDialog(QgsVectorLayer* vlayer): QDialog(), mVectorLayer(vlayer){}
  virtual void apply() = 0;
 protected:
  QgsVectorLayer* mVectorLayer;
};

#endif
