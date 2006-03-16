/***************************************************************************
  qgsxsddelegate.h  -  Qt4 Delegate for an XML Document with an XML Schema
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

#ifndef QGSXSDDELEGATE_H
#define QGSXSDDELEGATE_H

#include <QItemDelegate>

/**

  \brief  Qt4 Delegate for an XML Document with an XML Schema.

  \todo   Make it work

*/

class QgsXsdDelegate : public QItemDelegate
{
  
  Q_OBJECT

public:
  /**
  * \brief Constructor.
  */
  QgsXsdDelegate( );

  //! Destructor
  virtual ~QgsXsdDelegate();


public slots:


signals:


protected:


private:


};


#endif

// ENDS
