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
  QgsDebugMsg("Entering...");
  if ( ! mDom.isNull() )
  {
    // Start with inserting a mapcanvas element and populate it

    QDomElement mapCanvas; // A null element.

    QDomNodeList qgisList = mDom.elementsByTagName("qgis");
    if (qgisList.count())
    {
      QgsDebugMsg("Populating new mapcanvas");

      // there should only be one, so zeroth element ok
      QDomNode qgis = qgisList.item(0);  
      
      // Create a mapcanvas
      mapCanvas = mDom.createElement("mapcanvas");
      // Append mapcanvas to parent 'qgis'.
      qgis.appendChild(mapCanvas);
      // Re-parent units
      mapCanvas.appendChild(qgis.namedItem("units"));
      // Re-parent extent
      mapCanvas.appendChild(qgis.namedItem("extent"));

      // See if we can find if projection is on.
      
      QDomNode properties = qgis.namedItem("properties");
      QDomNode spatial = properties.namedItem("SpatialRefSys");
      QDomNode projectionsEnabled = spatial.namedItem("ProjectionsEnabled"); 
      // Type is 'int', and '1' if on.
      QDomElement projection = mDom.createElement("projection");
      QgsDebugMsg(projectionsEnabled.toText().data());
      if (projectionsEnabled.toText().data() == QString("1") )
      {
        QgsDebugMsg("Projection is turned on!");
        projection.appendChild(mDom.createTextNode("1"));
      }
      else
      {
        QgsDebugMsg("Projection is turned off!");
        projection.appendChild(mDom.createTextNode("0"));
      }
      mapCanvas.appendChild(projection);

    }


    // Transforming coordinate-transforms
    // Create a list of all map layers
    QDomNodeList mapLayers = mDom.elementsByTagName("maplayer");
    bool doneDestination = false;
    for (int i = 0; i < mapLayers.count(); i++)
    {
      QDomNode mapLayer = mapLayers.item(i);
      // Find the coordinatetransform
      QDomNode coordinateTransform = mapLayer.namedItem("coordinatetransform");
      // Find the sourcesrs
      QDomNode sourceSRS = coordinateTransform.namedItem("sourcesrs");
      // Rename to srs
      sourceSRS.toElement().setTagName("srs");
      // Re-parent to maplayer
      mapLayer.appendChild(sourceSRS);
      // Re-move coordinatetransform
      // Take the destination SRS of the first layer and use for mapcanvas projection
      if (! doneDestination)
      {
        // Use destination SRS from the last layer
        QDomNode destinationSRS = coordinateTransform.namedItem("destinationsrs");
        // Re-parent the destination SRS to the mapcanvas
        // If mapcanvas wasn't set, nothing will happen.
        mapCanvas.appendChild(destinationSRS);
        // Only do this once
        doneDestination = true;
      }
      mapLayer.removeChild(coordinateTransform);
      //QDomNode id = mapLayer.namedItem("id");
      //QgsDebugMsg(QString("Found maplayer ") + id.toElement().text());
      
    }

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

