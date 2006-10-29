/***************************************************************************
    qgspythonutils.h - routines for interfacing Python
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

#include <QString>

// forward declaration for PyObject
#ifndef PyObject_HEAD
struct _object;
typedef _object PyObject;
#endif

class QgisInterface;

/**
 All calls to Python functions in QGIS come here.
 
 All functions here are static - it's not needed to create an instance of this class

 For now, default path for python plugins is QgsApplication::pluginPath() + "/python"

 TODO: use native python calls instead of using PyRun_SimpleString
 TODO: add error checking and reporting
 */
class QgsPythonUtils
{
  public:
    
    /* general purpose functions */

    //! initialize python and import bindings
    static void initPython(QgisInterface* interface);
    
    //! close python interpreter
    static void exitPython();

    //! run a python statement (wrapper for PyRun_SimpleString)
    static void runSimpleString(QString command);
    
    /* python console related functions */
    
    //! change displayhook and excepthook
    //! our hooks will just save the result to special variables
    //! and those can be used in the program
    static void installConsoleHooks();
    
    //! get back to the original settings (i.e. write output to stdout)
    static void uninstallConsoleHooks();
    
    //! run a statement (wrapper for PyRun_String)
    //! this command is more advanced as enables error checking etc.
    //! @return true if no error occured
    static bool runString(QString command);
    
    //! get information about error to the supplied arguments
    //! @return false if there was no python error
    static bool getError(QString& errorClassName, QString& errorText);
    
    //! get result from the last statement as a string
    static QString getResult();

    /* plugins related functions */
    
    //! change path
    static void setPluginsPath(QString path);
  
    //! return current path for python plugins
    static QString pluginsPath();
    
    //! load python plugin (import)
    static void loadPlugin(QString packageName);
    
    //! start plugin: add to active plugins and call initGui()
    static void startPlugin(QString packageName);
    
    //! helper function to get some information about plugin
    //! @param function one of these strings: name, tpye, version, description
    static QString getPluginMetadata(QString pluginName, QString function);

    //! unload plugin
    static void unloadPlugin(QString packageName);

  protected:
    
    //! path where 
    static QString mPluginsPath;
    
    //! reference to module __main__
    static PyObject* mMainModule;
    
    //! dictionary of module __main__
    static PyObject* mMainDict;
};
