/***************************************************************************
                         qgscomposermapwidget.h
                         ----------------------
    begin                : May 26, 2008
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

#ifndef QGSCOMPOSERMAPWIDGET_H
#define QGSCOMPOSERMAPWIDGET_H

#include "ui_qgscomposermapwidgetbase.h"

class QgsComposerMap;

/**Input widget for the configuration of QgsComposerMap*/
class QgsComposerMapWidget: public QWidget, private Ui::QgsComposerMapWidgetBase
{
  Q_OBJECT

    public:

  QgsComposerMapWidget(QgsComposerMap* composerMap);
  ~QgsComposerMapWidget();

    private:
  QgsComposerMap* mComposerMap;
};

#endif
