//Added by qt3to4:
#include <QCloseEvent>
/***************************************************************************
                              qgsgrasstools.h 
                             -------------------
    begin                : March, 2005
    copyright            : (C) 2005 by Radim Blazek
    email                : blazek@itc.it
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QGSGRASSTOOLS_H
#define QGSGRASSTOOLS_H

class QString;
class QDomNode;
class QDomElement;
class QSize;

class QgisInterface;

#include "ui_qgsgrasstoolsbase.h"

//
// For filtering and model/view 
//
#include <QStandardItemModel>
#include <QSortFilterProxyModel>



/*! \class QgsGrassTools
 *  \brief Interface to GRASS modules.
 *
 */
class QgsGrassTools: public QWidget, private Ui::QgsGrassToolsBase
{
  Q_OBJECT;

  public:
  //! Constructor
  QgsGrassTools ( QgisInterface *iface, QWidget * parent = 0, 
      const char * name = 0, Qt::WFlags f = 0 );

  //! Destructor
  ~QgsGrassTools();


  public slots:
    //! Load configuration from file
    bool loadConfig(QString filePath);
  //! Recursively add sections and modules to the list view
  //  If parent is 0, the modules are added to mModulesListView root
  void addModules (QDomElement &element );
  //! Update the regex used to filter the modules list (autoconnect to ui)
  void on_mpFilterEdit_textChanged(const QString theText);
  //! view deteails for a module when its entry is clicked in the list view  (autoconnect to ui)
  void on_mpListView_activated(const QModelIndex &theIndex );
  //! Run a module when its entry is clicked in the list view (autoconnect to ui)
  void on_mpListView_doubleClicked(const QModelIndex &theIndex );

signals:
  // void regionChanged();

  private:
  //! Pointer to the QGIS interface object
  QgisInterface *mIface;
  //!model for tools list view
  QStandardItemModel * mModelTools;
  //!model proxy for sorting / filtering tools list view
  QSortFilterProxyModel * mModelProxy;

};

#endif // QGSGRASSTOOLS_H
