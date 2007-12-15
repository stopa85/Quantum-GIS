/***************************************************************************
                          qgsprojectfile.cpp  -  description
                             -------------------
    begin                : Sun 15 dec 2007
    copyright            : (C) 2007 by Magnus Homann
    email                : magnus at homann.se
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 /* $Id: qgsprojectfile.cpp 6942 2007-05-13 13:40:35Z mhugent $ */


#include "qgsprojectfile.h"
#include "qgsprojectversion.h"
#include "qgslogger.h"
#include <QTextStream>
#include <QDomDocument>

typedef QgsProjectVersion PFV;


QgsProjectFile::transform QgsProjectFile::transformers[] = {
  {PFV(0,8,1), PFV(0,9,0), &QgsProjectFile::transform081to090},
  {PFV(0,9,0), PFV(0,9,1), &QgsProjectFile::transform090to091},
  {PFV(0,9,1), PFV(0,9,2), &QgsProjectFile::transformNull}
};

bool QgsProjectFile::updateRevision(QgsProjectVersion newVersion)
{
  bool returnValue = false;

  if ( ! mDom.isNull() )
  {
    for (int i = 0; i < sizeof(transformers)/sizeof(transform); i++)
    {
      if ( transformers[i].from == mCurrentVersion)
      {
        // Run the transformer, and update the revision in every case
        (this->*(transformers[i].transformFunc))();
        mCurrentVersion = transformers[i].to;
        returnValue = true;
      }
    }
  }
  return returnValue;
};

void QgsProjectFile::dump()
{
  QgsDebugMsg(QString("Current project file version is %1.%2.%3")
              .arg(mCurrentVersion.major())
              .arg(mCurrentVersion.minor())
              .arg(mCurrentVersion.sub()));
  QgsDebugMsg(QString("\n") + mDom.toString(2));
              
}

/*
 *  Transformers below!
 */

void QgsProjectFile::transform081to090()
{
  QgsDebugMsg("entering");
  if ( ! mDom.isNull() )
  {
  }
  return;

};

void QgsProjectFile::transform090to091()
{
  QgsDebugMsg("entering");
  if ( ! mDom.isNull() )
  {
  }
  return;

};

