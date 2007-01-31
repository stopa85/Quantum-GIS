/***************************************************************************
                         qgsvectoroverlaydialog.h  -  description
                         ------------------------
    begin                : January 2007
    copyright            : (C) 2007 by Marco Hugentobler
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

#ifndef QGSVECTOROVERLAYDIALOG_H
#define QGSVECTOROVERLAYDIALOG_H

#include <QDialog>

class QgsVectorLayer;

/**An interface for dialogs provided by vector overlay plugins. This is that the vector layer properties dialog can call 'apply' for all overlay layers*/
class QgsVectorOverlayDialog: public QDialog
{
 public:
  QgsVectorOverlayDialog(QgsVectorLayer* vl): QDialog(), mVectorLayer(vl){}
    ~QgsVectorOverlayDialog(){}
  virtual void apply() const = 0;
 protected:
  QgsVectorLayer* mVectorLayer;
};

#endif
