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
  
  Py_Initialize();
  
  PyRun_SimpleString("from qgis.core import *\nfrom qgis.gui import *");
  QString import = \
      "print \"Welcome to QGIS console\"\n"
      "from sip import wrapinstance\n"
      "iface = wrapinstance(" + QString::number((unsigned int) pIface) + "L, QgisInterface)\n";
  PyRun_SimpleString(import.toLocal8Bit().data());

  module = PyImport_AddModule("__main__");
  dict = PyModule_GetDict(module);
}

QgsPythonDialog::~QgsPythonDialog()
{
  Py_Finalize();
}

void QgsPythonDialog::on_edtCmdLine_returnPressed()
{
  PyObject* obj;
  PyObject* obj_str;
  
  QString command = edtCmdLine->text();
  QString output;
  
  obj = PyRun_String(command.toLocal8Bit().data(), Py_eval_input, dict, dict);
  if (PyErr_Occurred())
  {
    PyErr_Print(); // TODO: PyErr_Fetch(...)
    output = "ERROR";
    PyErr_Clear();
  }
  else
  {
    obj_str = PyObject_Str(obj);
    if (obj_str != NULL)
      output = PyString_AsString(obj_str);
    Py_XDECREF(obj);
    Py_XDECREF(obj_str);
  }
  
  QString str = "<b>>>></b> " + command + "<br>" + output + "<br>";
  txtHistory->setText(txtHistory->text() + str);
  edtCmdLine->setText("");
  
  txtHistory->moveCursor(QTextCursor::End);
  txtHistory->ensureCursorVisible();
}
