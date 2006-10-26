/***************************************************************************
    qgspythondialog.h - dialog with embedded python console
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

#include <Python.h>

#include "qgspythondialog.h"

#include <iostream> // %%%

static PyObject* module;
static PyObject* dict;


QgsPythonDialog::QgsPythonDialog(QgisInterface* pIface, QWidget *parent)
  : QDialog(parent)
{
  setupUi(this);
  mIface = pIface;
  
  // change displayhook and excepthook
  // our hooks will just save the result to special variables
  // and those can be used in the program
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
  PyRun_SimpleString(command);
  
  module = PyImport_AddModule("__main__");
  dict = PyModule_GetDict(module);
  
}

QgsPythonDialog::~QgsPythonDialog()
{
  // get back to the original settings (i.e. write output to stdout)
  const char* command = \
      "sys.displayhook = sys.__displayhook__\n"
      "sys.excepthook = sys.__excepthook__\n";
  PyRun_SimpleString(command);
}

void QgsPythonDialog::on_edtCmdLine_returnPressed()
{
  PyObject* obj;
  PyObject* obj_str;
  PyObject* err_type;
  PyObject* err_value;
  PyObject* err_tb;
  
  QString command = edtCmdLine->text();
  QString output;
  
  // when using Py_single_input the return value will be always null
  // we're using custom hooks for output and exceptions to show output in console
  PyRun_String(command.toLocal8Bit().data(), Py_single_input, dict, dict);
  if (PyErr_Occurred())
  {
    // get the exception information
    PyErr_Fetch(&err_type, &err_value, &err_tb);
    
    // get exception's class name
    QString className = PyString_AS_STRING(((PyClassObject*)err_type)->cl_name);
    
    // get exception's text
    QString errorText;
    if (err_value != NULL && err_value != Py_None)
    {
      obj_str = PyObject_Str(err_value); // new reference
      errorText = PyString_AS_STRING(obj_str);
      Py_XDECREF(obj_str);
    }
    
    output = "<font color=\"red\">" + className + ": " + errorText + "</font><br>";
    
    // clear exception
    PyErr_Clear();
  }
  else
  {
    // get the result
    obj = PyDict_GetItemString(dict, "__result"); // obj is borrowed reference
    
    if (obj != NULL && obj != Py_None)
    {
      obj_str = PyObject_Str(obj); // obj_str is new reference
      if (obj_str != NULL && obj_str != Py_None)
      {
        output = PyString_AsString(obj_str);
        output += "<br>";
      }
      Py_XDECREF(obj_str);
    }
    
    // erase result
    PyDict_SetItemString(dict, "__result", Py_None);
  }
  
  QString str = "<b><font color=\"green\">>>></font> " + command + "</b><br>" + output;
  txtHistory->setText(txtHistory->text() + str);
  edtCmdLine->setText("");
  
  txtHistory->moveCursor(QTextCursor::End);
  txtHistory->ensureCursorVisible();
}
