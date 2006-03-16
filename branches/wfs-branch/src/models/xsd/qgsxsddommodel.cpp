/***************************************************************************
  qgsxsddommodel.cpp  -  Qt4 Model for an XML Document with an XML Schema
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

#include "qgsxsddommodel.h"

#include <fstream>
#include <iostream>


QgsXsdDomModel::QgsXsdDomModel(QIODevice * xmlInstance, QIODevice * xmlSchema)
  : QAbstractItemModel()
{
#ifdef QGISDEBUG
  std::cout << "QgsXsdDomModel: constructing." << std::endl;
#endif


#ifdef QGISDEBUG
  std::cout << "QgsXsdDomModel: exiting constructor." << std::endl;
#endif
}

QgsXsdDomModel::~QgsXsdDomModel()
{
#ifdef QGISDEBUG
  std::cout << "QgsXsdDomModel: deconstructing." << std::endl;
#endif
}


int QgsXsdDomModel::columnCount(const QModelIndex & parent) const
{
  // TODO
  return 0;
}

QVariant QgsXsdDomModel::data(const QModelIndex & index, int role) const
{
  // TODO
  return QVariant();
}

QModelIndex QgsXsdDomModel::index(int row, int column, const QModelIndex & parent) const
{
  // TODO
  return QModelIndex();
}

QModelIndex QgsXsdDomModel::parent(const QModelIndex & index) const
{
  // TODO
  return QModelIndex();
}

int QgsXsdDomModel::rowCount(const QModelIndex & parent) const
{
  // TODO
  return 0;
}


// ENDS
