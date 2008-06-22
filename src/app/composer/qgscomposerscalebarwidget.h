/***************************************************************************
                            qgscomposerscalebarwidget.h
                            ---------------------------
    begin                : 11 June 2008
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

#ifndef QGSCOMPOSERSCALEBARWIDGET_H
#define QGSCOMPOSERSCALEBARWIDGET_H

#include "ui_qgscomposerscalebarwidgetbase.h"

class QgsComposerScaleBar;

class QgsComposerScaleBarWidget: public QWidget, private Ui::QgsComposerScaleBarWidgetBase
{
  Q_OBJECT

 public:
  QgsComposerScaleBarWidget(QgsComposerScaleBar* scaleBar);
  ~QgsComposerScaleBarWidget();

  public slots:
  void on_mMapComboBox_activated(const QString& text);
  void on_mLineWidthSpinBox_valueChanged(double d);
  void on_mSegmentSizeSpinBox_valueChanged(double d);
  void on_mSegmentsLeftSpinBox_valueChanged(int i);
  void on_mNumberOfSegmentsSpinBox_valueChanged(int i);

 protected:
  void showEvent ( QShowEvent * event );
 
 private:
  QgsComposerScaleBar* mComposerScaleBar;

  void refreshMapComboBox();
  void setGuiElements();
};

#endif //QGSCOMPOSERSCALEBARWIDGET_H
