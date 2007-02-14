/***************************************************************************
                         qgsdiagramoverlayplugin.h  -  description
                         -------------------------
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

#ifndef QGSDIAGRAMOVERLAYPLUGIN_H
#define QGSDIAGRAMOVERLAYPLUGIN_H

#include "qgsvectoroverlayplugin.h"

class QgisApp;
class QgisIface;
class QgsVectorOverlayDialog;

/**A plugin for placing diagrams on vector layers. The plugin provides a widget that can be embedded into the
vector layer properties dialog and is able to create and configure a diagram overlay layer*/
class QgsDiagramOverlayPlugin: public QgsVectorOverlayPlugin
{
 public:
  QgsDiagramOverlayPlugin(QgisApp* app, QgisIface* iface);
  ~QgsDiagramOverlayPlugin();
  QgsVectorOverlayDialog* dialog(QgsVectorLayer* vl) const;
  void initGui(){}
  void unload(){}
};

#endif
