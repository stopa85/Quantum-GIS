/***************************************************************************
                         qgsdiagramdialog.h  -  description
                         ------------------
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

#ifndef QGSDIAGRAMDIALOG_H
#define QGSDIAGRAMDIALOG_H

#include "ui_qgsdiagramdialogbase.h"
#include "qgsvectoroverlaydialog.h"

class QgsVectorLayer;

/**Dialog to enter options for diagram symbolisation*/
class QgsDiagramDialog: public QgsVectorOverlayDialog, private Ui::QgsDiagramDialogBase
{
  Q_OBJECT
 public:
  QgsDiagramDialog(QgsVectorLayer* vl);
  ~QgsDiagramDialog();
  void apply() const;

  private slots:
  /**Adds name of the attribute combo box into the tree widget*/
  void addAttribute();
  /**Removes the current attribute from the tree widget*/
  void removeAttribute();
  void changeClassificationType(const QString& newType);
  void changeClassificationAttribute(const QString& newAttribute);
  /**Calls the color dialog if column == 1*/
  void handleItemDoubleClick(QTreeWidgetItem * item, int column);

 private:
  QgsDiagramDialog();
  /**Helper function that returns the attribute index from an attribute name*/
  int indexFromAttributeName(const QString& name) const;
};

#endif
