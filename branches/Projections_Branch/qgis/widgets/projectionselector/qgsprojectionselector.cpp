/***************************************************************************
 *   Copyright (C) 2005 by Tim Sutton                                      *
 *   tim@linfiniti.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "qgsprojectionselector.h"

//standard includes
#include <iostream>
#include <cassert>

//qgis includes
#include "qgsspatialreferences.h"
#include "qgscsexception.h"
#include "qgsconfig.h"

//qt includes
#include <qapplication.h>
#include <qfile.h>
#include <qtextedit.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qprogressdialog.h> 
#include <qapplication.h>

//stdc++ includes
#include <iostream>
#include <cstdlib>

//gdal and ogr includes
#include <ogr_api.h>
#include <ogr_spatialref.h>
#include <cpl_error.h>

// set the default coordinate system
static const char* defaultWktKey = "4326";

QgsProjectionSelector::QgsProjectionSelector( QWidget* parent , const char* name , WFlags fl  )
    : QgsProjectionSelectorBase( parent, "Projection Selector", fl )
{

  // Populate the projection list view
  getProjList();
}

QgsProjectionSelector::~QgsProjectionSelector()
{
}

void QgsProjectionSelector::setSelectedWKT(QString theWKT)
{

}

void QgsProjectionSelector::setSelectedSRID(QString theSRID)
{

}

QString QgsProjectionSelector::getCurrentWKT()
{
  return QString("");
}

QString QgsProjectionSelector::getCurrentSRID()
{
  return QString("");
}

void QgsProjectionSelector::getProjList()
{
  // setup the nodes for the list view
  //
  // Geographic coordinate system node
  geoList = new QListViewItem(lstCoordinateSystems,"Geographic Coordinate System");
  // Projected coordinate system node
  projList = new QListViewItem(lstCoordinateSystems,"Projected Coordinate System");

  // Get the instance of the spaital references object that
  // contains a collection (map) of all QgsSpatialRefSys objects
  // for every coordinate system in the resources/spatial_ref_sys.txt
  // file
  QgsSpatialReferences *srs = QgsSpatialReferences::instance();

  // Determine the current project projection so we can select the 
  // correct entry in the combo

  //
  // TODO ! Implemente this properly!!!! ts
  //
  QString currentSrid = ""; 
  QListViewItem * mySelectedItem = 0;
  // get the reference to the map
  projectionWKTMap_t mProjectionsMap = srs->getMap();
  // get an iterator for the map
  projectionWKTMap_t::iterator myIterator;
  //find out how many entries in the map
  int myEntriesCount = mProjectionsMap.size();
  std::cout << "Srs map has " << myEntriesCount << " entries " << std::endl;
  int myProgress = 1;
  QProgressDialog myProgressBar( "Building Projections List...", "Cancel", myEntriesCount,
          this, "progress", TRUE );
  myProgressBar.setProgress(myProgress);
  // now add each key to our list view
  QListViewItem *newItem;
  for ( myIterator = mProjectionsMap.begin(); myIterator != mProjectionsMap.end(); ++myIterator ) 
  {
    myProgressBar.setProgress(myProgress++);
    qApp->processEvents();
    //std::cout << "Widget map has: " <<myIterator.key().ascii() << std::endl;
    //cboProjection->insertItem(myIterator.key());

    QgsSpatialRefSys *srs = *myIterator;
    assert(srs != 0);
    //XXX Add to the tree view
    if(srs->isGeographic())
    {
      // this is a geographic coordinate system
      // Add it to the tree
      newItem = new QListViewItem(geoList, srs->name());
      //    std::cout << "Added " << getWKTShortName(srs->srText()) << std::endl; 
      // display the spatial reference id in the second column of the list view
      newItem->setText(1,srs->srid());
      if (myIterator.key()==currentSrid)
      {
        // this is the selected item -- store it for future use
        mySelectedItem = newItem;
      }
    }
    else
    {
      // coordinate system is projected...
      QListViewItem *node; // node that we will add this cs to...

      // projected coordinate systems are stored by projection type
      QStringList projectionInfo = QStringList::split(" - ", srs->name());
      if(projectionInfo.size() == 2)
      {
        // Get the projection name and strip white space from it so we
        // don't add empty nodes to the tree
        QString projName = projectionInfo[1].stripWhiteSpace();
        //      std::cout << "Examining " << shortName << std::endl; 
        if(projName.length() == 0)
        {
          // If the projection name is blank, set the node to 
          // 0 so it won't be inserted
          node = projList;
          //       std::cout << "projection name is blank: " << shortName << std::endl; 
          assert(1 == 0);
        }
        else
        {

          // Replace the underscores with blanks
          projName = projName.replace('_', ' ');
          // Get the node for this type and add the projection to it
          // If the node doesn't exist, create it
          node = lstCoordinateSystems->findItem(projName, 0);
          if(node == 0)
          {
            //          std::cout << projName << " node not found -- creating it" << std::endl;

            // the node doesn't exist -- create it
            node = new QListViewItem(projList, projName);
            //          std::cout << "Added top-level projection node: " << projName << std::endl; 
          }
        }
      }
      else
      {
        // No projection type is specified so add it to the top-level
        // projection node
        //XXX This should never happen
        node = projList;
        //      std::cout << shortName << std::endl; 
        assert(1 == 0);
      }

      // now add the coordinate system to the appropriate node

      newItem = new QListViewItem(node, srs->name());
      // display the spatial reference id in the second column of the list view

      newItem->setText(1,srs->srid());
      if (myIterator.key()==currentSrid)
        mySelectedItem = newItem;
    }
  } //else = proj coord sys        


  /**
  //make sure all the loaded layer WKT's and the active project projection exist in the 
  //combo box too....
  std::map<QString, QgsMapLayer *> myMapLayers = QgsMapLayerRegistry::instance()->mapLayers();
  std::map<QString, QgsMapLayer *>::iterator myMapIterator;
  for ( myMapIterator = myMapLayers.begin(); myMapIterator != myMapLayers.end(); ++myMapIterator )
  {
  QgsMapLayer * myMapLayer = myMapIterator->second;
  QString myWKT = myMapLayer->getProjectionWKT();
  QString myWKTShortName = getWKTShortName(myWKT);
  //TODO add check here that CS is not already in the projections map
  //and if not append to wkt_defs file
  cboProjection->insertItem(myIterator.key());
  mProjectionsMap[myWKTShortName]=myWKT;
  }    

  //set the combo entry to the current entry for the project
  cboProjection->setCurrentText(mySelectedKey);
  */
  lstCoordinateSystems->setCurrentItem(mySelectedItem);
  lstCoordinateSystems->ensureItemVisible(mySelectedItem);
  /*
     for ( myIterator = mProjectionsMap.begin(); myIterator != mProjectionsMap.end(); ++myIterator ) 
     {
  //std::cout << "Widget map has: " <<myIterator.key().ascii() << std::endl;
  //cboProjection->insertItem(myIterator.key());
  if(myIterator.key().find("Lat/Long") > -1)
  {
  new QListViewItem(geoList, myIterator.key());
  }
  else
  {
  new QListViewItem(projList, myIterator.key());
  }
  }

*/
  std::cout << "Done adding projections from spatial_ref_sys singleton" << std::endl; 
}   


void QgsProjectionSelector::coordinateSystemSelected( QListViewItem * theItem)
{
  QgsSpatialRefSys *srs = QgsSpatialReferences::instance()->getSrsBySrid(theItem->text(1));
  if(srs)
  {

    //set the text box to show the full proection spec
    std::cout << "Item selected : " << theItem->text(0) << std::endl;
    std::cout << "Item selected full wkt : " << srs->srText() << std::endl;
    QString wkt = srs->srText();
    assert(wkt.length() > 0);
    // reformat the wkt to improve the display in the textedit
    // box
    wkt = wkt.replace(",", ", ");
    teProjection->setText(wkt);
  }
}

QString QgsProjectionSelector::getWKTShortName(QString theWKT)
{
  if (!theWKT) return NULL;
  if (theWKT.isEmpty()) return NULL;
  /* for example 
     PROJCS["Kertau / Singapore Grid",GEOGCS["Kertau",DATUM["Kertau",SPHEROID["Everest 1830 Modified",6377304.063,300.8017]],PRIMEM["Greenwich",0],UNIT["degree",0.0174532925199433]],PROJECTION["Cassini_Soldner"],PARAMETER["latitude_of_origin",1.28764666666667],PARAMETER["central_meridian",103.853002222222],PARAMETER["false_easting",30000],PARAMETER["false_northing",30000],UNIT["metre",1]]

     We want to pull out 
     Kertau / Singapore Grid
     and
     Cassini_Soldner
     */
  OGRSpatialReference mySpatialRefSys;
  //this is really ugly but we need to get a QString to a char**
  char * mySourceCharArrayPointer = (char*) theWKT.ascii();

  /* Here are the possible OGR error codes :
     typedef int OGRErr;

#define OGRERR_NONE                0
#define OGRERR_NOT_ENOUGH_DATA     1    --> not enough data to deserialize 
#define OGRERR_NOT_ENOUGH_MEMORY   2
#define OGRERR_UNSUPPORTED_GEOMETRY_TYPE 3
#define OGRERR_UNSUPPORTED_OPERATION 4
#define OGRERR_CORRUPT_DATA        5
#define OGRERR_FAILURE             6
#define OGRERR_UNSUPPORTED_SRS     7 */

  OGRErr myInputResult = mySpatialRefSys.importFromWkt( & mySourceCharArrayPointer );
  if (myInputResult != OGRERR_NONE)
  {
    return NULL;
  }
  //std::cout << theWKT << std::endl;
  //check if the coordinate system is projected or not

  // if the spatial ref sys starts with GEOGCS, the coordinate
  // system is not projected
  QString myProjection,myDatum,myCoordinateSystem,myName;
  if(theWKT.find(QRegExp("^GEOGCS")) == 0)
  {
    myProjection = "Lat/Long";
    myCoordinateSystem = mySpatialRefSys.GetAttrValue("GEOGCS",0);
    myName = myProjection + " - " + myCoordinateSystem.replace('_', ' ');
  }  
  else
  {    

    myProjection = mySpatialRefSys.GetAttrValue("PROJCS",0);
    myCoordinateSystem = mySpatialRefSys.GetAttrValue("PROJECTION",0);
    myName = myProjection + " - " + myCoordinateSystem.replace('_', ' ');
  } 
  //std::cout << "Projection short name " << myName << std::endl;
  return myName; 
}
