/***************************************************************************
    qgspythonutils.cpp - routines for interfacing Python
    ---------------------
    begin                : October 2006
    copyright            : (C) 2006 by Martin Dobias
    email                : wonder.sk at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* $Id$ */

// python should be first include
// otherwise issues some warnings
#include <Python.h>

#include "qgspythonutils.h"

#include "qgsapplication.h"


#include <iostream>


QString QgsPythonUtils::mPluginsPath;
PyObject* QgsPythonUtils::mMainModule;
PyObject* QgsPythonUtils::mMainDict;


void QgsPythonUtils::initPython(QgisInterface* interface)
{
  // initialize python
  Py_Initialize();

  // import sip, qgis bindings, initialize 'iface' object
  QString strInit = "import sys\n"
      "from sip import wrapinstance, unwrapinstance\n"
      "from qgis.core import *\n"
      "from qgis.gui import *\n\n"
      "iface = wrapinstance(" + QString::number((unsigned long) interface) + ", QgisInterface)\n"
      "plugins = {}\n";
  runSimpleString(strInit);

  setPluginsPath(QgsApplication::pluginPath() + "/python");

  mMainModule = PyImport_AddModule("__main__"); // borrowed reference
  mMainDict = PyModule_GetDict(mMainModule); // borrowed reference
}

void QgsPythonUtils::exitPython()
{  
  Py_Finalize();
  mMainModule = NULL;
  mMainDict = NULL;
}


void QgsPythonUtils::runSimpleString(QString command)
{
  //std::cout << command.toLocal8Bit().data() << std::endl;
  PyRun_SimpleString(command.toLocal8Bit().data());
}


void QgsPythonUtils::installConsoleHooks()
{
  const char* command = \
      "import __main__\n"
      "\n"
      "def console_display_hook(obj):\n"
      "  __main__.__result = obj\n"
      "\n"
      "def console_except_hook(type, value, tb):\n"
      "  __main__.__error_type = type\n"
      "  __main__.__error_value = value\n"
      "  __main__.__error_tb = tb\n"
      "\n"
      "sys.displayhook = console_display_hook\n"
      "sys.excepthook = console_except_hook\n";
  runSimpleString(command);
}

void QgsPythonUtils::uninstallConsoleHooks()
{
  const char* command = \
      "sys.displayhook = sys.__displayhook__\n"
      "sys.excepthook = sys.__excepthook__\n";
  runSimpleString(command);
}


bool QgsPythonUtils::runString(QString command)
{
  //std::cout << command.toLocal8Bit().data() << std::endl;
  PyRun_String(command.toLocal8Bit().data(), Py_single_input, mMainDict, mMainDict);
  return (PyErr_Occurred() == 0);
}


bool QgsPythonUtils::getError(QString& errorClassName, QString& errorText)
{
  if (!PyErr_Occurred())
    return false;
  
  PyObject* obj_str;
  PyObject* err_type;
  PyObject* err_value;
  PyObject* err_tb;
  
  // get the exception information
  PyErr_Fetch(&err_type, &err_value, &err_tb);
    
  // get exception's class name
  errorClassName = PyString_AS_STRING(((PyClassObject*)err_type)->cl_name);
    
  // get exception's text
  if (err_value != NULL && err_value != Py_None)
  {
    obj_str = PyObject_Str(err_value); // new reference
    errorText = PyString_AS_STRING(obj_str);
    Py_XDECREF(obj_str);
  }
  else
    errorText.clear();
  
  // clear exception
  PyErr_Clear();
  
  return true;
}

QString QgsPythonUtils::getResult()
{
  PyObject* obj;
  PyObject* obj_str;
  
  QString output;
  
  // get the result
  obj = PyDict_GetItemString(mMainDict, "__result"); // obj is borrowed reference
    
  if (obj != NULL && obj != Py_None)
  {
    obj_str = PyObject_Str(obj); // obj_str is new reference
    if (obj_str != NULL && obj_str != Py_None)
    {
      output = PyString_AsString(obj_str);
    }
    Py_XDECREF(obj_str);
  }
    
  // erase result
  PyDict_SetItemString(mMainDict, "__result", Py_None);

  return output;
}



void QgsPythonUtils::setPluginsPath(QString path)
{
  mPluginsPath = path;
  
  // alter sys.path to search for packages & modules in (plugin_dir)/python
  runSimpleString("sys.path.insert(0, '" + path + "')");
}

QString QgsPythonUtils::pluginsPath()
{
  return mPluginsPath;
}

QString QgsPythonUtils::getPluginMetadata(QString pluginName, QString function)
{
  QString command = pluginName + "." + function + "()";
  QString retval = "???";
  
  PyObject* obj = PyRun_String(command.toLocal8Bit().data(), Py_eval_input, mMainDict, mMainDict);
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


void QgsPythonUtils::loadPlugin(QString packageName)
{
  // TODO: error checking if the plugin has been loaded
  
  // load plugin's package and ensure that plugin is reloaded when changed
  runSimpleString("import " + packageName + "\n"
                  "reload(" + packageName + ")");
}


void QgsPythonUtils::startPlugin(QString packageName)
{
  QString pluginPythonVar = "plugins['" + packageName + "']";
  
  // create an instance of the plugin
  runSimpleString(pluginPythonVar + " = " + packageName + ".classFactory(iface)");
  
  // initGui
  runSimpleString(pluginPythonVar + ".initGui()");
}


void QgsPythonUtils::unloadPlugin(QString packageName)
{
  // unload and delete plugin!
  QString varName = "plugins['" + packageName + "']";
  QString command = \
      varName + ".unload()\n"
      "del " + varName;
  runSimpleString(command);
}
