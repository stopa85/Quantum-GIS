/***************************************************************************
  roadgraphplugin.h
  --------------------------------------
  Date                 : 2010-11-13
  Copyright            : (C) 2010 by Yakushev Sergey
  Email                : YakushevS@gmail.com
****************************************************************************
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
*                                                                          *
***************************************************************************/

#ifndef ROADGRAPH_RASTERLAYERSETTINGSWIDGET_H
#define ROADGRAPH_RASTERLAYERSETTINGSWIDGET_H

#include <QWidget>

class RgRasterLayerSettings;

// forward declaration QT-classes
class QComboBox;

// forward declaration Qgis-classes
class QgsRasterLayer;

/**
@author Sergey Yakushev
*/
/**
* \class RgLineVectorLayerSettingsWidget
* \brief
*/
class RgRasterLayerSettingsWidget : public QWidget
{
    Q_OBJECT
  public:
    RgRasterLayerSettingsWidget( RgRasterLayerSettings *s, QWidget* parent = 0 );

  private slots:
    void on_mcbLayers_selectItem();

  private:
    QgsRasterLayer * selectedLayer();

  public:
    /**
     * list of possible layers
     */
    QComboBox *mcbLayers;

    /**
     * list of speed band
     */
    QComboBox *mcbSpeed;
    /**
     * list of speed unit
     */
    QComboBox *mcbUnitOfSpeed;
};
#endif
