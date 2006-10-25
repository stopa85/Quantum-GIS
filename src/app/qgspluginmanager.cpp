/***************************************************************************
                          qgspluginmanager.cpp  -  description
                             -------------------
    begin                : Someday 2003
    copyright            : (C) 2003 by Gary E.Sherman
    email                : sherman at mrcc.com
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* $Id$ */

#include "qgsconfig.h"

#ifdef HAVE_PYTHON
#include <Python.h>
#endif

#include <iostream>
#include <QApplication>
#include <QFileDialog>
#include <QLineEdit>
#include <Q3ListView>
#include <QMessageBox>
#include <QLibrary>
#include <QSettings>
#include "qgisplugin.h"
#include "qgslogger.h"
#include "qgspluginmanager.h"
#include "qgspluginitem.h"
#include "qgsproviderregistry.h"
#include "qgspluginregistry.h"

#define TESTLIB
#ifdef TESTLIB
// This doesn't work on WIN32 and causes problems with plugins
// on OS X (the code doesn't cause a problem but including dlfcn.h
// renders plugins unloadable)
#if !defined(WIN32) && !defined(Q_OS_MACX) 
#include <dlfcn.h>
#endif
#endif
QgsPluginManager::QgsPluginManager(QWidget * parent, Qt::WFlags fl)
: QDialog(parent, fl)
{
  setupUi(this);
  // set the default lib dir to the qgis install directory/lib (this info is
  // available from the provider registry so we use it here)
  QgsProviderRegistry *pr = QgsProviderRegistry::instance();
  /*  char **argv = qApp->argv();
     QString appDir = argv[0];
     int bin = appDir.findRev("/bin", -1, false);
     QString baseDir = appDir.left(bin);
     QString libDir = baseDir + "/lib"; */

  txtPluginDir->setText(pr->libraryDirectory().path());
  getPluginDescriptions();
#ifdef HAVE_PYTHON
  // initialize python
  Py_Initialize();
  getPythonPluginDescriptions();
#endif
}


QgsPluginManager::~QgsPluginManager()
{
#ifdef HAVE_PYTHON
  Py_Finalize();
#endif
}

void QgsPluginManager::on_btnBrowse_clicked()
{
  QString s = QFileDialog::getExistingDirectory(this, tr("Choose a directory"));
  txtPluginDir->setText(s);
  getPluginDescriptions();
}

#ifdef HAVE_PYTHON

QString QgsPluginManager::getPythonPluginMetadata(QString pluginName, QString function)
{
  PyObject* module = PyImport_AddModule("__main__");
  PyObject* dict = PyModule_GetDict(module);
  QString command = pluginName + "." + function + "()";
  QString retval = "???";
  
  PyObject* obj = PyRun_String(command.toLocal8Bit().data(), Py_eval_input, dict, dict);
  if (PyErr_Occurred())
  {
    PyErr_Print(); // TODO: PyErr_Fetch(...)
    std::cout << "Python ERROR!" << std::endl;
    PyErr_Clear();
  }
  else if (PyString_Check(obj))
  {
    retval = PyString_AS_STRING(obj);
  }
  else
  {
    std::cout << "Bad python return value!" << std::endl;
  }
  Py_XDECREF(obj);
  return retval;
}

void QgsPluginManager::getPythonPluginDescriptions()
{
  // TODO: have own path for python plugins
  QString strPythonDir = txtPluginDir->text() + "/python";

  // look in (plugin_dir)/python for directories
  QDir pluginDir(strPythonDir, "*", QDir::Name | QDir::IgnoreCase, QDir::Dirs | QDir::NoDotAndDotDot);
  
  // alter sys.path to search for packages & modules in (plugin_dir)/python
  QString strInit = "import sys\n"
                    "sys.path.insert(0, '" + strPythonDir + "')";
  
  PyRun_SimpleString(strInit.toLocal8Bit().data());

  //PyRun_SimpleString("from qgis.core import *\nfrom qgis.gui import *");

  for (uint i = 0; i < pluginDir.count(); i++)
  {
    QString pluginName = pluginDir[i];
    
    // import plugin's package
    QString command = "import " + pluginName;
    PyRun_SimpleString(command.toLocal8Bit().data());
    
    // get information from the plugin
    QString name = getPythonPluginMetadata(pluginName, "name");
    QString description = getPythonPluginMetadata(pluginName, "description");
    QString version = getPythonPluginMetadata(pluginName, "version");
    
    if (name == "???" || description == "???" || version == "???")
      continue;
    
    // add to the list box
    Q3CheckListItem *pl = new Q3CheckListItem(lstPlugins, "[P] " + name, Q3CheckListItem::CheckBox);
    pl->setText(1, version);
    pl->setText(2, description);
    pl->setText(3, pluginDir[i]);
  }
  
}
#endif


void QgsPluginManager::getPluginDescriptions()
{
QString sharedLibExtension;
#ifdef WIN32
sharedLibExtension = "*.dll";
#else
sharedLibExtension = "*.so*";
#endif

// check all libs in the current plugin directory and get name and descriptions
  QDir pluginDir(txtPluginDir->text(), sharedLibExtension, QDir::Name | QDir::IgnoreCase, QDir::Files | QDir::NoSymLinks);

  if (pluginDir.count() == 0)
  {
      QMessageBox::information(this, tr("No Plugins"), tr("No QGIS plugins found in ") + txtPluginDir->text());
      return;
  }

  QgsDebugMsg("PLUGIN MANAGER:");
  for (uint i = 0; i < pluginDir.count(); i++)
  {
    QString lib = QString("%1/%2").arg(txtPluginDir->text()).arg(pluginDir[i]);

#ifdef TESTLIB
          // This doesn't work on WIN32 and causes problems with plugins
          // on OS X (the code doesn't cause a problem but including dlfcn.h
          // renders plugins unloadable)
#if !defined(WIN32) && !defined(Q_OS_MACX)
          // test code to help debug loading problems
          // This doesn't work on WIN32 and causes problems with plugins
          // on OS X (the code doesn't cause a problem but including dlfcn.h
          // renders plugins unloadable)

//          void *handle = dlopen((const char *) lib, RTLD_LAZY);
    void *handle = dlopen(lib.toLocal8Bit().data(), RTLD_LAZY | RTLD_GLOBAL);
    if (!handle)
    {
      QgsDebugMsg("Error in dlopen: ");
      QgsDebugMsg(dlerror());
    }
    else
    {
      QgsDebugMsg("dlopen suceeded for " + lib);
      dlclose(handle);
    }
#endif //#ifndef WIN32 && Q_OS_MACX
#endif //#ifdef TESTLIB

    QgsDebugMsg("Examining: " + lib);
    QLibrary *myLib = new QLibrary(lib);
    bool loaded = myLib->load();
    if (!loaded)
    {
      QgsDebugMsg("Failed to load: " + myLib->library());
      delete myLib;
      continue;
    }

    QgsDebugMsg("Loaded library: " + myLib->library());

    // Don't bother with libraries that are providers
    //if(!myLib->resolve("isProvider"))

    //MH: Replaced to allow for plugins that are linked to providers
    //type is only used in non-provider plugins 
    if (!myLib->resolve("type"))
    {
      delete myLib;
      continue;
    }
    
    // resolve the metadata from plugin
    name_t *pName = (name_t *) myLib->resolve("name");
    description_t *pDesc = (description_t *) myLib->resolve("description");
    version_t *pVersion = (version_t *) myLib->resolve("version");

    // show the values (or lack of) for each function
    if(pName){
      QgsDebugMsg("Plugin name: " + pName());
    }else{
      QgsDebugMsg("Plugin name not returned when queried\n");
    }
    if(pDesc){
      QgsDebugMsg("Plugin description: " + pDesc());
    }else{
      QgsDebugMsg("Plugin description not returned when queried\n");
    }
    if(pVersion){
      QgsDebugMsg("Plugin version: " + pVersion());
    }else{
      QgsDebugMsg("Plugin version not returned when queried\n");
    }

    if (!pName || !pDesc || !pVersion)
    {
      QgsDebugMsg("Failed to get name, description, or type for " + myLib->library());
      delete myLib;
      continue;
    }

    Q3CheckListItem *pl = new Q3CheckListItem(lstPlugins, pName(), Q3CheckListItem::CheckBox); //, pDesc(), pluginDir[i])
    pl->setText(1, pVersion());
    pl->setText(2, pDesc());
    pl->setText(3, pluginDir[i]);

    QgsDebugMsg("Getting an instance of the QgsPluginRegistry");

    // check to see if the plugin is loaded and set the checkbox accordingly
    QgsPluginRegistry *pRegistry = QgsPluginRegistry::instance();

    // get the library using the plugin description
    QString libName = pRegistry->library(pName());
    if (libName.length() == 0)
    {
      QgsDebugMsg("Couldn't find library name in the registry");
    }
    else
    {
      QgsDebugMsg("Found library name in the registry");
      if (libName == myLib->library())
      {
        // set the checkbox
        pl->setOn(true);
      }
    }

    delete myLib;
  }
}


void QgsPluginManager::on_btnOk_clicked()
{
  unload();
  accept();
}

void QgsPluginManager::unload()
{
  QSettings settings;
#ifdef QGISDEBUG
  std::cout << "Checking for plugins to unload" << std::endl;
#endif
  Q3CheckListItem *lvi = (Q3CheckListItem *) lstPlugins->firstChild();
  while (lvi != 0)
    {
      if (!lvi->isOn())
        {
          // its off -- see if it is loaded and if so, unload it
          QgsPluginRegistry *pRegistry = QgsPluginRegistry::instance();
#ifdef QGISDEBUG
          std::cout << "Checking to see if " << lvi->text(0).toLocal8Bit().data() << " is loaded" << std::endl;
#endif
          QgisPlugin *plugin = pRegistry->plugin(lvi->text(0));
          if (plugin)
          {
            plugin->unload();
            // remove the plugin from the registry
            pRegistry->removePlugin(lvi->text(0));
            //disable it to the qsettings file [ts]
            settings.writeEntry("/Plugins/" + lvi->text(0), false);
          }
        }
      lvi = (Q3CheckListItem *) lvi->nextSibling();
    }
}

std::vector < QgsPluginItem > QgsPluginManager::getSelectedPlugins()
{
  std::vector < QgsPluginItem > pis;
  Q3CheckListItem *lvi = (Q3CheckListItem *) lstPlugins->firstChild();
  while (lvi != 0)
  {
    if (lvi->isOn())
    {
      QString pluginName = lvi->text(0);
      
      // python plugins have prefx [P] in the list
      bool pythonic = (pluginName.indexOf("[P] ") == 0);
      if (pythonic)
        pluginName = pluginName.mid(4);
      
      pis.push_back(QgsPluginItem(pluginName, lvi->text(2), txtPluginDir->text() + "/" + lvi->text(3), 0, pythonic));
    }
    lvi = (Q3CheckListItem *) lvi->nextSibling();
  }
  return pis;
}

void QgsPluginManager::on_btnSelectAll_clicked()
{
  // select all plugins
  Q3CheckListItem *child = dynamic_cast<Q3CheckListItem *>(lstPlugins->firstChild());
  while(child)
  {
    child->setOn(true);
    child = dynamic_cast<Q3CheckListItem *>(child->nextSibling());
  }

}

void QgsPluginManager::on_btnClearAll_clicked()
{
  // clear all selection checkboxes 
  Q3CheckListItem *child = dynamic_cast<Q3CheckListItem *>(lstPlugins->firstChild());
  while(child)
  {
    child->setOn(false);
    child = dynamic_cast<Q3CheckListItem *>(child->nextSibling());
  }
}

void QgsPluginManager::on_btnClose_clicked()
{
  reject();
}
