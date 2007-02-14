/***************************************************************************
                         qgsdiagramoverlayplugin.cpp  -  description
                         ---------------------------
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

#include "qgsdiagramoverlayplugin.h"
#include "qgsdiagramdialog.h"

#ifdef WIN32
#define QGISEXTERN extern "C" __declspec( dllexport )
#else
#define QGISEXTERN extern "C"
#endif

static const QString pluginName = "diagram overlay";
static const QString pluginDescription = "A plugin for placing diagrams on vector layers";
static const QString pluginVersion = "Version 0.0.1";

QgsDiagramOverlayPlugin::QgsDiagramOverlayPlugin(QgisApp* app, QgisIface* iface): QgsVectorOverlayPlugin(pluginName, pluginDescription, pluginVersion)
{

}

QgsDiagramOverlayPlugin::~QgsDiagramOverlayPlugin()
{

}

QgsVectorOverlayDialog* QgsDiagramOverlayPlugin::dialog(QgsVectorLayer* vl) const
{
  return new QgsDiagramDialog(vl);
}

QGISEXTERN QgisPlugin* classFactory(QgisApp* app, QgisIface* iface)
{
  return new QgsDiagramOverlayPlugin(app, iface);
}

QGISEXTERN QString name()
{
  return pluginName;
}

QGISEXTERN QString description()
{
  return pluginDescription;
}

QGISEXTERN QString version()
{
  return pluginVersion;
}

QGISEXTERN int type()
{
  return QgisPlugin::VECTOR_OVERLAY;
}

QGISEXTERN void unload(QgisPlugin* theQgsDiagramOverlayPluginPointer)
{
  delete theQgsDiagramOverlayPluginPointer;
}
