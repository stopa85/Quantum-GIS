/***************************************************************************
    qgsxsdform.h  -  A form that carries an QgsXsdFormView
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

#ifndef QGSXSDFORM_H
#define QGSXSDFORM_H
#include "ui_qgsxsdformbase.h"
#include "qgisgui.h"

class QgisApp;

/*!
 * \brief   A form that carries an QgsXsdFormView - experimental for WFS implementation
 *
 */
class QgsXsdForm : public QDialog, private Ui::QgsXsdFormBase
{
  Q_OBJECT

public:

  //! Constructor
  QgsXsdForm(QgisApp *app, QWidget *parent = 0, Qt::WFlags fl = QgisGui::ModalDialogFlags);

  //! Destructor
  ~QgsXsdForm();


public slots:


private:

  //! Pointer to the qgis application mainwindow
  QgisApp *qgisApp;



};


#endif // QGSXSDFORM_H

// ENDS
