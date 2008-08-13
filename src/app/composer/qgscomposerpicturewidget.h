/***************************************************************************
                         qgscomposerpicturewidget.h
                         --------------------------
    begin                : August 13, 2008
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

#ifndef QGSCOMPOSERPICTUREWIDGET_H
#define QGSCOMPOSERPICTUREWIDGET_H

#include "ui_qgscomposerpicturewidgetbase.h"

class QgsComposerPicture;

class QgsComposerPictureWidget: public QWidget, private Ui::QgsComposerPictureWidgetBase
{
  Q_OBJECT
   
 public:
  QgsComposerPictureWidget(QgsComposerPicture* picture);
  ~QgsComposerPictureWidget();

 public slots:
  void on_mPictureBrowseButton_clicked();
  void on_mPictureLineEdit_editingFinished();
  void on_mRotationSpinBox_valueChanged(double d);

 private:
  QgsComposerPicture* mPicture;
};

#endif
