/***************************************************************************
  qgsxsddommodel.h  -  Qt4 Model for an XML Document with an XML Schema
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

#ifndef QGSXSDDOMMODEL_H
#define QGSXSDDOMMODEL_H

#include <QAbstractItemModel>
#include <QIODevice>

/**

  \brief  Qt4 Model for an XML Document with an XML Schema.

  \todo   Make it work

*/

class QgsXsdDomModel : public QAbstractItemModel
{
  
  Q_OBJECT

public:
  /**
   * \brief Constructor.
   *
   * \param xmlInstance  The XML Instance Document that contains the user data
   * \param xmlSchema    The XML Schema Document that should be used to interpret xmlInstance
   *
   * \todo  Get rid of = 0 once views of this model work out how to supply Documents -
   *        the = 0 measure is only to get things compiling in the first place
   */
  QgsXsdDomModel(QIODevice * xmlInstance = 0, QIODevice * xmlSchema = 0);

  //! Destructor
  virtual ~QgsXsdDomModel();

  //! Required implementation of QAbstractItemModel - to be filled out later
  virtual int columnCount(const QModelIndex & parent = QModelIndex() ) const;

  //! Required implementation of QAbstractItemModel - to be filled out later
  virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

  //! Required implementation of QAbstractItemModel - to be filled out later
  virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex() ) const;

  //! Required implementation of QAbstractItemModel - to be filled out later
  virtual QModelIndex parent(const QModelIndex & index) const;

  //! Required implementation of QAbstractItemModel - to be filled out later
  virtual int rowCount(const QModelIndex & parent = QModelIndex() ) const;


public slots:


signals:


private:


};


#endif

// ENDS
