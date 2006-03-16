/***************************************************************************
    qgsxsdform.cpp  -  A form that carries an QgsXsdFormView
                             -------------------
    begin                : 10 Mar, 2006
    copyright            : (C) 2006 by Brendan Morley
    email                : morb at beagle dot com dot au
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

#include "qgsxsdform.h"

#include <iostream>

#include "../models/xsd/qgsxsddommodel.h"
#include "../views/xsdform/qgsxsdformview.h"

QgsXsdForm::QgsXsdForm(QgisApp * app, QWidget * parent, Qt::WFlags fl)
  : QDialog(parent, fl),
    qgisApp(app)
{
  setupUi(this);
  connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));

  // Setup the embedded view and connect the view to the model
  QgsXsdDomModel *model = new QgsXsdDomModel();
  QgsXsdFormView *view  = new QgsXsdFormView;
  view->setModel(model);

}


QgsXsdForm::~QgsXsdForm()
{
  // NOOP
}

// ENDS
