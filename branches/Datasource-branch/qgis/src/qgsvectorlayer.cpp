/***************************************************************************
                          qgsvectorlayer.cpp  -  description
                             -------------------
    begin                : Oct 29, 2003
    copyright            : (C) 2003 by Gary E.Sherman
    email                : sherman at mrcc.com
  
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 /*  $Id$ */

#include <iostream>
#include <sstream>
#include <qapplication.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qpointarray.h>
#include <qstring.h>
#include <qmessagebox.h>
#include "qgsrect.h"
#include "qgspoint.h"
#include "qgscoordinatetransform.h"
#include "qgsvectorlayer.h"
#include "qgsidentifyresults.h"
#include "qgsattributetable.h"
#include "qgsdataprovider.h"
#include "qgsfeature.h"
#include <qlistview.h>
#include <qlibrary.h>
#include <ogrsf_frmts.h>
#include <ogr_geometry.h>

#include <dlfcn.h>

typedef QgsDataProvider * create_it(const char *uri);
QgsVectorLayer::QgsVectorLayer(QString vectorLayerPath, QString baseName)
:QgsMapLayer(VECTOR, baseName, vectorLayerPath)
{
// load the plugin
//TODO figure out how to register and identify data source plugin for a specific
//TODO layer type
	void *handle = dlopen("./providers/libproviders.so", RTLD_LAZY);
	             if (!handle) {
                      std::cout << "Error in dlopen: " <<  dlerror() << std::endl;
                     
                  }else{
				  	std::cout << "dlopen suceeded" << std::endl;
					dlclose(handle);
					}
				  	

QLibrary *myLib = new QLibrary("./providers/libproviders.so");
		std::cout << "Library name is " << myLib->library() << std::endl;
		bool loaded = myLib->load();
		if (loaded) {
			std::cout << "Loaded test shapefile provider library" << std::endl;
			std::cout << "Attempting to resolve the classFactory function" << std::endl;
			create_it *cf = (create_it *) myLib->resolve("classFactory");
	
			if (cf) {
				std::cout << "Getting pointer to a QgsShapefileProvider object from the library\n";
				dataProvider = cf(vectorLayerPath);
				if(dataProvider){
					std::cout << "Instantiated the shapefile provider plugin\n";
				/* 	QgsFeature *f = dataProvider->getFirstFeature();
					
					if(f){
                         unsigned char *geometry = f->getGeometry();
                        if(geometry){
                        std::cout << "Geometry:" <<  *geometry << std::endl;
                        }else{
                            std::cout << "f->getGeometry() returned null\n";
                        } 
					}else{
						std::cout << "QgsFeature f is null\n";
					} */
                    // iterate through the features
                 /*    while(f){
                        delete f;
                        f = dataProvider->getNextFeature();
                    } */
                    // show the extent
                    QgsRect *mbr = dataProvider->extent();
                    QString s = mbr->stringRep();
                    std::cout << "Extent of layer: " << s << std::endl;
					// store the extent
					layerExtent.setXmax(mbr->xMax());
					layerExtent.setXmin(mbr->xMin());
					layerExtent.setYmax(mbr->yMax());
					layerExtent.setYmin(mbr->yMin());
					// get and store the feature type
					geometryType = dataProvider->geometryType();
				}else{
					std::cout << "Unable to instantiate the shapefile test plugin\n";
				}
			}
		}else{
			std::cout << "Failed to load " << "../providers/libproviders.so" << "\n";
		}
	//create a boolean vector and set every entry to false

	if (valid) {
		selected = new QValueVector < bool > (dataProvider->featureCount(), false);
	} else {
		selected = 0;
	}
	tabledisplay = 0;
	//draw the selected features in yellow
	selectionColor.setRgb(255, 255, 0);
}

QgsVectorLayer::~QgsVectorLayer()
{
//delete ogrDataSource;
	if (selected) {
		delete selected;
	}
	if (tabledisplay) {
		tabledisplay->close();
	}
}

/** No descriptions */
void QgsVectorLayer::registerFormats()
{
}
/*
* Draw the layer
*/
void QgsVectorLayer::draw_old(QPainter * p, QgsRect * viewExtent, QgsCoordinateTransform * cXf)
{
  	// set pen and fill
	QgsSymbol *sym = symbol();
	QPen pen;
	pen.setColor(sym->color());
	pen.setWidth(sym->lineWidth());
	p->setPen(pen);


	QBrush *brush = new QBrush(sym->fillColor());
}
void QgsVectorLayer::draw(QPainter * p, QgsRect * viewExtent, QgsCoordinateTransform * cXf)
{
    if(1 == 1){
	// painter is active (begin has been called
	/* Steps to draw the layer
	   1. get the features in the view extent by SQL query
	   2. read WKB for a feature
	   3. transform
	   4. draw
	 */
	// set pen and fill
	QgsSymbol *sym = symbol();
	QPen pen;
	pen.setColor(sym->color());
	pen.setWidth(sym->lineWidth());
	p->setPen(pen);


	QBrush *brush = new QBrush(sym->fillColor());

	// select the records in the extent. The provider sets a spatial filter
    // and sets up the selection set for retrieval
     qWarning("Selecting features based on view extent");
  //  dataProvider->select(viewExtent);
		int featureCount = 0;
     //  QgsFeature *ftest = dataProvider->getFirstFeature();
        qWarning("Starting draw of features");
        QgsFeature *fet;
        unsigned char *feature;
		while ((fet = dataProvider->getNextFeature())) {
      
			if (fet == 0) {
				std::cout << "get next feature returned null\n";
			}else{
                std::cout << "get next feature returned valid feature\n";
            
			//if feature is selected, change the color of the painter
            //TODO fix this selection code to work with the provider
			/* if ((*selected)[fet->GetFID()] == true) {
				// must change color of pen since it holds not only color
				// but line width
				pen.setColor(selectionColor);
				p->setPen(pen);
				brush->setColor(selectionColor);
			} else { */
				pen.setColor(sym->color());
				p->setPen(pen);
				brush->setColor(sym->fillColor());
			/* } */

			/* OGRGeometry *geom = fet->GetGeometryRef();
			if (!geom) {
				std::cout << "geom pointer is null" << std::endl;
			} */
			// get the wkb representation
			feature = fet->getGeometry();
		//	if (feature != 0) {
				//    std::cout << featureCount << "'the feature is null\n";

			int wkbType = (int) feature[1];
			std::cout << "Feature type: " << wkbType << std::endl;
			// read each feature based on its type
			double *x;
			double *y;
			int *nPoints;
			int *numRings;
			int *numPolygons;
			int numPoints;
			int numLineStrings;
			int idx, jdx, kdx;
			unsigned char *ptr;
			char lsb;
			QgsPoint pt;
			QPointArray *pa;
			OGRFieldDefn *fldDef;
			QString fld;
			QString val;
			switch (wkbType) {
			  case WKBPoint:
				  p->setBrush(*brush);
				  //  fldDef = fet->GetFieldDefnRef(1);
				  //   fld = fldDef->GetNameRef();
				//NEEDTHIS?  val = fet->GetFieldAsString(1);
				  //std::cout << val << "\n";

				  x = (double *) (feature + 5);
				  y = (double *) (feature + 5 + sizeof(double));
				  //std::cout << "transforming point\n";
				  pt = cXf->transform(*x, *y);
				  //std::cout << "drawing marker for feature " << featureCount << "\n";
				  p->drawRect(pt.xToInt(), pt.yToInt(), 5, 5);
				  //std::cout << "marker draw complete\n";
				  break;
			  case WKBLineString:
				  // get number of points in the line
				  ptr = feature + 5;
				  nPoints = (int *) ptr;
				  ptr = feature + 1 + 2 * sizeof(int);
				  for (idx = 0; idx < *nPoints; idx++) {
					  x = (double *) ptr;
					  ptr += sizeof(double);
					  y = (double *) ptr;
					  ptr += sizeof(double);
					  // transform the point
					  pt = cXf->transform(*x, *y);
					  if (idx == 0)
						  p->moveTo(pt.xToInt(), pt.yToInt());
					  else
						  p->lineTo(pt.xToInt(), pt.yToInt());
				  }
				  break;
			  case WKBMultiLineString:

				  numLineStrings = (int) (feature[5]);
				  ptr = feature + 9;
				  for (jdx = 0; jdx < numLineStrings; jdx++) {
					  // each of these is a wbklinestring so must handle as such
					  lsb = *ptr;
					  ptr += 5;	// skip type since we know its 2
					  nPoints = (int *) ptr;
					  ptr += sizeof(int);
					  for (idx = 0; idx < *nPoints; idx++) {
						  x = (double *) ptr;
						  ptr += sizeof(double);
						  y = (double *) ptr;
						  ptr += sizeof(double);
						  // transform the point
						  pt = cXf->transform(*x, *y);
						  if (idx == 0)
							  p->moveTo(pt.xToInt(), pt.yToInt());
						  else
							  p->lineTo(pt.xToInt(), pt.yToInt());

					  }
				  }
				  break;
			  case WKBPolygon:
              
				  p->setBrush(*brush);
				  // get number of rings in the polygon
				  numRings = (int *) (feature + 1 + sizeof(int));
                  std::cout << "Number of rings: " << *numRings << std::endl;
				  ptr = feature + 1 + 2 * sizeof(int);
				  for (idx = 0; idx < *numRings; idx++) {
					  // get number of points in the ring
					  nPoints = (int *) ptr;
                      std::cout << "Number of points: " << *nPoints << std::endl;
					  ptr += 4;
					  pa = new QPointArray(*nPoints);
					  for (jdx = 0; jdx < *nPoints; jdx++) {
						  // add points to a point array for drawing the polygon
                       //   std::cout << "Adding points to array\n";
						  x = (double *) ptr;
						  ptr += sizeof(double);
						  y = (double *) ptr;
						  ptr += sizeof(double);
						  pt = cXf->transform(*x, *y);
						  pa->setPoint(jdx, pt.xToInt(), pt.yToInt());
					  }
					  // draw the ring
                      std::cout << "Drawing the polygon\n";
					  p->drawPolygon(*pa);

				  }
              
				  break;
			  case WKBMultiPolygon:
				  p->setBrush(*brush);
				  // get the number of polygons
				  ptr = feature + 5;
				  numPolygons = (int *) ptr;
				  for (kdx = 0; kdx < *numPolygons; kdx++) {
					  //skip the endian and feature type info and
					  // get number of rings in the polygon
					  ptr = feature + 14;
					  numRings = (int *) ptr;
					  ptr += 4;
					  for (idx = 0; idx < *numRings; idx++) {
						  // get number of points in the ring
						  nPoints = (int *) ptr;
						  ptr += 4;
						  pa = new QPointArray(*nPoints);
						  for (jdx = 0; jdx < *nPoints; jdx++) {
							  // add points to a point array for drawing the polygon
							  x = (double *) ptr;
							  ptr += sizeof(double);
							  y = (double *) ptr;
							  ptr += sizeof(double);
							  // std::cout << "Transforming " << *x << "," << *y << " to ";

							  pt = cXf->transform(*x, *y);
							  //std::cout << pt.xToInt() << "," << pt.yToInt() << std::endl;
							  pa->setPoint(jdx, pt.xToInt(), pt.yToInt());

						  }
						  // draw the ring
						  p->drawPolygon(*pa);
						  delete pa;
					  }
				  }
				  break;
			}

			std::cout << "deleting feature[]\n";
			//      std::cout << geom->getGeometryName() << std::endl;
			featureCount++;
			delete[]feature;
			std::cout << "deleting fet\n";
			delete fet;
			std::cout << "ready to fetch next feature\n";
		// if fet not null }
	
//      std::cout << featureCount << " features in ogr layer within the extent" << std::endl;
	//	OGRGeometry *filt = ogrLayer->GetSpatialFilter();
		//filt->dumpReadable(stdout);
		//ogrLayer->ResetReading();
        std::cout << "Idling in the getNextFeature loop\n";
	}
    }
  	std::cout << "finished reading features\n";
std::cout << "Doing processEvents()\n";
qApp->processEvents();
}
}

int QgsVectorLayer::endian()
{
	char *chkEndian = new char[4];
	memset(chkEndian, '\0', 4);
	chkEndian[0] = 0xE8;

	int *ce = (int *) chkEndian;
	int retVal;
	if (232 == *ce)
		retVal = NDR;
	else
		retVal = XDR;
	delete[]chkEndian;
	return retVal;
}

void QgsVectorLayer::identify(QgsRect * r)
{
	OGRGeometry *filter = 0;
	filter = new OGRPolygon();
	QString wktExtentString = QString("POLYGON ((%1))").arg(r->stringRep());
//	QTextStream wktExtent(&wktExtentString, IO_WriteOnly);
	//wktExtent << "POLYGON ((" << r->stringRep() << "))" << std::ends;
	const char *wktText = (const char *) wktExtentString;
	OGRErr result = ((OGRPolygon *) filter)->importFromWkt((char **)&wktText);
	if (result == OGRERR_NONE) {

		ogrLayer->SetSpatialFilter(filter);
		int featureCount = 0;
		// display features falling within the search radius
		QgsIdentifyResults *ir = 0;
		while (OGRFeature * fet = ogrLayer->GetNextFeature()) {
			//}

			if (fet) {
				featureCount++;
				// found at least one feature - show it in the identify box
				if (ir == 0) {
					// create the identify results dialog if it doesn't already
					// exist
					ir = new QgsIdentifyResults();
				}

				int numFields = fet->GetFieldCount();
				// Determine the field index for the feature column of the identify
				// dialog. We look for fields containing "name" first and second for
				// fields containing "id". If neither are found, the first field
				// is used as the node.
				int idxName = -1;
				int idxId = -1;
				for (int j = 0; j < numFields; j++) {
					OGRFieldDefn *def = fet->GetFieldDefnRef(j);
					QString fldName = def->GetNameRef();
					std::cout << "Checking field " << fldName << std::endl;
					if (fldName.contains("name", false)) {
						idxName = j;
						break;
					}
					if (fldName.contains("id", false)) {
						idxId = j;
						break;
					}
				}
				int fieldIndex = 0;
				if (idxName > -1) {
					fieldIndex = idxName;
				} else {
					if (idxId > -1) {
						fieldIndex = idxId;
					}
				}
				std::cout << "Field index for feature label is " << fieldIndex << std::endl;
				QListViewItem *featureNode = ir->addNode("foo");
				for (int i = 0; i < numFields; i++) {

					// add the feature attributes to the tree
					OGRFieldDefn *fldDef = fet->GetFieldDefnRef(i);
					QString fld = fldDef->GetNameRef();
					OGRFieldType fldType = fldDef->GetType();
					QString val;

					//if(fldType ==  16604 )    // geometry
					val = "(geometry column)";
					// else
					val = fet->GetFieldAsString(i);
					// Create a node for this feature
					std::cout << "i / fieldIndex " << i << " / " << fieldIndex << std::endl;
					if (i == fieldIndex) {
						featureNode->setText(0, val);
						std::cout << "Adding feature node: " << val << std::endl;
					}
					std::cout << "Adding attribute " << fld << " = " << val << std::endl;
					ir->addAttribute(featureNode, fld, val);
				}
			}

		}
		std::cout << "Feature count on identify: " << featureCount << std::endl;
		if (ir) {
			ir->setTitle(name());
			ir->show();
		}
		if (featureCount == 0) {
			QMessageBox::information(0, "No features found", "No features were found in the active layer at the point you clicked");
		}
		ogrLayer->ResetReading();
	}

}
void QgsVectorLayer::table()
{
	if (tabledisplay) {
		tabledisplay->raise();
	} else {
		// display the attribute table
		QApplication::setOverrideCursor(Qt::waitCursor);
		ogrLayer->SetSpatialFilter(0);
		OGRFeature *fet = ogrLayer->GetNextFeature();
		int numFields = fet->GetFieldCount();
		tabledisplay = new QgsAttributeTableDisplay();
	  QObject:connect(tabledisplay, SIGNAL(deleted()), this, SLOT(invalidateTableDisplay()));
		tabledisplay->table()->setNumRows(ogrLayer->GetFeatureCount(true));
		tabledisplay->table()->setNumCols(numFields + 1);	//+1 for the id-column

		int row = 0;
		// set up the column headers
		QHeader *colHeader = tabledisplay->table()->horizontalHeader();
		colHeader->setLabel(0, "id");	//label for the id-column
		for (int h = 1; h < numFields + 1; h++) {
			OGRFieldDefn *fldDef = fet->GetFieldDefnRef(h - 1);
			QString fld = fldDef->GetNameRef();
			colHeader->setLabel(h, fld);
		}
		while (fet) {

			//id-field
			tabledisplay->table()->setText(row, 0, QString::number(fet->GetFID()));
			tabledisplay->table()->insertFeatureId(fet->GetFID());	//insert the id into the search tree of qgsattributetable
			for (int i = 1; i < numFields + 1; i++) {
				// get the field values
				QString val;
				//if(fldType ==  16604 )    // geometry
				val = "(geometry column)";
				// else
				val = fet->GetFieldAsString(i - 1);

				tabledisplay->table()->setText(row, i, val);

			}
			row++;
			delete fet;
			fet = ogrLayer->GetNextFeature();

		}
		// reset the pointer to start of fetabledisplayures so
		// subsequent reads will not fail
		ogrLayer->ResetReading();
		tabledisplay->table()->setSorting(true);


		tabledisplay->setTitle("Tabledisplaytribute table - " + name());
		tabledisplay->show();
		tabledisplay->table()->clearSelection();	//deselect the first row

		//select the rows of the already selected features
		QObject::disconnect(tabledisplay->table(), SIGNAL(selectionChanged()), tabledisplay->table(), SLOT(handleChangedSelections()));
		for (int i = 0; i < ogrLayer->GetFeatureCount(); i++) {
			if ((*selected)[i] == true) {
				tabledisplay->table()->selectRow(i);
			}
		}
		QObject::connect(tabledisplay->table(), SIGNAL(selectionChanged()), tabledisplay->table(), SLOT(handleChangedSelections()));

		//etablish the necessary connections between the table and the shapefilelayer
		QObject::connect(tabledisplay->table(), SIGNAL(selected(int)), this, SLOT(select(int)));
		QObject::connect(tabledisplay->table(), SIGNAL(selectionRemoved()), this, SLOT(removeSelection()));
		QObject::connect(tabledisplay->table(), SIGNAL(repaintRequested()), this, SLOT(triggerRepaint()));
		QApplication::restoreOverrideCursor();
	}
}

void QgsVectorLayer::select(int number)
{
	(*selected)[number] = true;
}

void QgsVectorLayer::select(QgsRect * rect, bool lock)
 {
/*
	if (tabledisplay) {
		QObject::disconnect(tabledisplay->table(), SIGNAL(selectionChanged()), tabledisplay->table(), SLOT(handleChangedSelections()));
		QObject::disconnect(tabledisplay->table(), SIGNAL(selected(int)), this, SLOT(select(int)));	//disconnecting because of performance reason
	}

	if (lock == false) {
		removeSelection();		//only if ctrl-button is not pressed
		if (tabledisplay) {
			tabledisplay->table()->clearSelection();
		}
	}

	OGRGeometry *filter = 0;
	filter = new OGRPolygon();
	QString wktExtent = QString("POLYGON ((%1))").arg(rect->stringRep());
	const char *wktText = (const char *)wktExtent;

	OGRErr result = ((OGRPolygon *) filter)->importFromWkt((char **)&wktText);
	if (result == OGRERR_NONE) {
		ogrLayer->SetSpatialFilter(filter);
		int featureCount = 0;
		while (OGRFeature * fet = ogrLayer->GetNextFeature()) {
			if (fet) {
				select(fet->GetFID());
				if (tabledisplay) {
					tabledisplay->table()->selectRowWithId(fet->GetFID());
					(*selected)[fet->GetFID()] = true;
				}
			}
		}
		ogrLayer->ResetReading();
	}

	if (tabledisplay) {
		QObject::connect(tabledisplay->table(), SIGNAL(selectionChanged()), tabledisplay->table(), SLOT(handleChangedSelections()));
		QObject::connect(tabledisplay->table(), SIGNAL(selected(int)), this, SLOT(select(int)));	//disconnecting because of performance reason
	}
	triggerRepaint();*/
} 



/*
OGRGeometry *filter = 0;
	filter = new OGRPolygon();
	std::stringstream wktExtent;
	wktExtent << "POLYGON ((" << r->stringRep() << "))" << std::ends;
	char *wktText = wktExtent.str();

	OGRErr result = ((OGRPolygon *) filter)->importFromWkt(&wktText);
	if (result == OGRERR_NONE) {

		ogrLayer->SetSpatialFilter(filter);
		int featureCount = 0;
		// just id the first feature for now
		//while (OGRFeature * fet = ogrLayer->GetNextFeature()) {
		//}

		OGRFeature *fet = ogrLayer->GetNextFeature();
		if (fet) {
			// found feature - show it in the identify box
			QgsIdentifyResults *ir = new QgsIdentifyResults();
			// just show one result - modify this later
			int numFields = fet->GetFieldCount();
			for (int i = 0; i < numFields; i++) {
				// get the field definition
				OGRFieldDefn *fldDef = fet->GetFieldDefnRef(i);
				QString fld = fldDef->GetNameRef();
				OGRFieldType fldType = fldDef->GetType();
				QString val;
				//if(fldType ==  16604 )    // geometry
				val = "(geometry column)";
				// else
				val = fet->GetFieldAsString(i);
				ir->addAttribute(fld, val);
			}
			ir->setTitle(name());
			ir->show();
			ogrLayer->ResetReading();//remove this, if it is not a success

		} else {
			QMessageBox::information(0, "No features found", "No features were found in the active layer at the point you clicked");
		}
	}
*/

void QgsVectorLayer::removeSelection()
{
	for (int i = 0; i < (int) selected->size(); i++) {
		(*selected)[i] = false;
	}
}

void QgsVectorLayer::triggerRepaint()
{
	emit repaintRequested();
}

void QgsVectorLayer::invalidateTableDisplay()
{
	tabledisplay = 0;
}
