/***************************************************************************
  ogr.cpp 
  The ogr plugin gives access to spatial databases via ogr
  -------------------
         begin                : [PluginDate]
         copyright            : [(C) Your Name and Date]
         email                : [Your Email]

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/*  $Id: plugin.cpp 6935 2007-05-07 14:29:51Z wonder $ */

//
// QGIS Specific includes
//

#include <qgisinterface.h>
#include <qgisgui.h>

#include "qgsogrsourceselect.h"

#include "ogr.h"
//#include "ogrgui.h"

//
// Qt4 Related Includes
//

#include <QAction>
#include <QToolBar>



#ifdef WIN32
#define QGISEXTERN extern "C" __declspec( dllexport )
#else
#define QGISEXTERN extern "C"
#endif

static const char * const sIdent = "$Id: plugin.cpp 6935 2007-05-07 14:29:51Z wonder $";
static const QString sName = QObject::tr("Ogr Plugin");
static const QString sDescription = QObject::tr("The ogr plugin gives access to spatial databases via ogr");
static const QString sPluginVersion = QObject::tr("Version 0.1");
static const QgisPlugin::PLUGINTYPE sPluginType = QgisPlugin::UI;

//////////////////////////////////////////////////////////////////////
//
// THE FOLLOWING METHODS ARE MANDATORY FOR ALL PLUGINS
//
//////////////////////////////////////////////////////////////////////

/**
 * Constructor for the plugin. The plugin is passed a pointer 
 * an interface object that provides access to exposed functions in QGIS.
 * @param theQGisInterface - Pointer to the QGIS interface object
 */
ogr::ogr(QgisInterface * theQgisInterface):
                 QgisPlugin(sName,sDescription,sPluginVersion,sPluginType),
                 mQGisIface(theQgisInterface)
{
}

ogr::~ogr()
{

}

/*
 * Initialize the GUI interface for the plugin - this is only called once when the plugin is 
 * added to the plugin registry in the QGIS application.
 */
void ogr::initGui()
{

  // Create the action for tool
  mQActionPointer = new QAction(QIcon(":/ogr/ogr.png"),"&Load OGR Layer", this);
  connect(mQActionPointer, SIGNAL(activated()), this, SLOT(showSourceDialog()));  

  
  // Set the what's this text
  //mQActionPointer->setWhatsThis(tr("Replace this with a short description of the what the plugin does"));
  // Connect the action to the run
  //connect(mQActionPointer, SIGNAL(activated()), this, SLOT(run()));
  // Add the toolbar
  mToolBarPointer = new QToolBar(mQGisIface->getMainWindow(), "ogr");
  mToolBarPointer->setLabel("Ogr Plugin");
  // Add the icon to the toolbar
  mQGisIface->addToolBarIcon(mQActionPointer);
  mQGisIface->addPluginMenu("&ogr", mQActionPointer);

}
//method defined in interface
void ogr::help()
{
  //implement me!
}

// Slot called when the menu item is activated
// If you created more menu items / toolbar buttons in initiGui, you should 
// create a separate handler for each action - this single run() method will
// not be enough
void ogr::run()
{
  //ogrGui *myPluginGui=new ogrGui(mQGisIface->getMainWindow(), QgisGui::ModalDialogFlags);
  //myPluginGui->setAttribute(Qt::WA_DeleteOnClose);
  
  //myPluginGui->show();
}

// Unload the plugin by cleaning up the GUI
void ogr::unload()
{
  // remove the GUI
  mQGisIface->removePluginMenu("&ogr",mQActionPointer);
  mQGisIface->removeToolBarIcon(mQActionPointer);
  delete mQActionPointer;
}

void ogr::showSourceDialog()
{
  QgsOGRSourceSelect *serverDialog=new QgsOGRSourceSelect(mQGisIface,mQGisIface->getMainWindow(), QgisGui::ModalDialogFlags);
  serverDialog->setAttribute(Qt::WA_DeleteOnClose);
  
  serverDialog->show();    
}

/** 
 * Required extern functions needed  for every plugin 
 * These functions can be called prior to creating an instance
 * of the plugin class
 */
// Class factory to return a new instance of the plugin class
QGISEXTERN QgisPlugin * classFactory(QgisInterface * theQgisInterfacePointer)
{
  return new ogr(theQgisInterfacePointer);
}
// Return the name of the plugin - note that we do not user class members as
// the class may not yet be insantiated when this method is called.
QGISEXTERN QString name()
{
  return sName;
}

// Return the description
QGISEXTERN QString description()
{
  return sDescription;
}

// Return the type (either UI or MapLayer plugin)
QGISEXTERN int type()
{
  return sPluginType;
}

// Return the version number for the plugin
QGISEXTERN QString version()
{
  return sPluginVersion;
}

// Delete ourself
QGISEXTERN void unload(QgisPlugin * thePluginPointer)
{
  delete thePluginPointer;
}

