/***************************************************************************
                         qgslegendmodel.h  -  description
                         -----------------
    begin                : June 2008
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

#ifndef QGSLEGENDMODEL_H
#define QGSLEGENDMODEL_H

#include <QStandardItemModel>
#include <QStringList>

class QgsMapLayer;

/**A model that provides layers as root items. The classification items are children of the layer items*/
class QgsLegendModel: public QStandardItemModel
{
 public:
  QgsLegendModel();
  ~QgsLegendModel();

  void setLayerSet(const QStringList& layerIds);

 private:
  /**Adds classification items of vector layers
   @return 0 in case of success*/
  int addVectorLayerItems(QStandardItem* layerItem, QgsMapLayer* vlayer);

  /**Adds item of raster layer
   @return 0 in case of success*/
  int addRasterLayerItem(QStandardItem* layerItem, QgsMapLayer* rlayer);

 protected:
  QStringList mLayerIds;
};

#endif
