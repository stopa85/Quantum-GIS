/***************************************************************************
  qgsxsddelegate.cpp  -  Qt4 Delegate for an XML Document with an XML Schema
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

#include "qgsxsddelegate.h"

#include <fstream>
#include <iostream>


QgsXsdDelegate::QgsXsdDelegate()
  : QItemDelegate()
{
#ifdef QGISDEBUG
  std::cout << "QgsXsdDelegate: constructing." << std::endl;
#endif


#ifdef QGISDEBUG
  std::cout << "QgsXsdDelegate: exiting constructor." << std::endl;
#endif
}


QgsXsdDelegate::~QgsXsdDelegate()
{
#ifdef QGISDEBUG
  std::cout << "QgsXsdDelegate: deconstructing." << std::endl;
#endif
}


// ENDS
