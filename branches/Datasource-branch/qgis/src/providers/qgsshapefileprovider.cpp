/* QGIS data provider for ESRI Shapefile format */
/* $Id$ */
#include <iostream>
#include "../qgsdataprovider.h"
#include "../qgsfeature.h"
#include "../qgsrect.h"
#include <ogrsf_frmts.h>
#include <ogr_geometry.h>
#include <cpl_error.h>
#include "qgsshapefileprovider.h"
QgsShapeFileProvider::QgsShapeFileProvider(QString uri):dataSourceUri(uri)
{
	OGRRegisterAll();
		
	// make connection to the data source
	std::cout << "Data source uri is " << uri << std::endl;
	ogrDataSource = OGRSFDriverRegistrar::Open((const char *) uri);
	if (ogrDataSource != NULL) {
		std::cout << "Data source is valid" << std::endl;
		valid = true;
		ogrLayer = ogrDataSource->GetLayer(0);
		// get the extent_ (envelope) of the layer
		extent_ = new OGREnvelope();
		ogrLayer->GetExtent(extent_);
		// check the validity of the layer
		OGRFeature *feat = ogrLayer->GetNextFeature();
		if (feat) {
			OGRGeometry *geom = feat->GetGeometryRef();
			if (geom) {
				geomType = geom->getGeometryType();
				
			} else {
				valid = false;
			}
			delete feat;
		} else {
			valid = false;
		}
        ogrLayer->ResetReading();
	} else {
		std::cout << "Data source is invalid" << std::endl;
		const char *er = CPLGetLastErrorMsg();
		std::cout << er << std::endl;
		valid = false;
	}

	//create a boolean vector and set every entry to false

/* 	if (valid) {
		selected = new std::vector < bool > (ogrLayer->GetFeatureCount(), false);
	} else {
		selected = 0;
	} */
//  tabledisplay=0;
	//draw the selected features in yellow
//  selectionColor.setRgb(255,255,0);

}

QgsShapeFileProvider::~QgsShapeFileProvider()
{
}

/**
	* Get the first feature resutling from a select operation
	* @return QgsFeature
	*/
QgsFeature *QgsShapeFileProvider::getFirstFeature()
{
	QgsFeature *f = 0;
	if(valid){
		std::cout << "getting first feature\n";
		ogrLayer->ResetReading();
		OGRFeature *feat = ogrLayer->GetNextFeature();
		if(feat){
			std::cout << "First feature is not null\n";
		}else{
			std::cout << "First feature is null\n";
		}
		f = new QgsFeature();
		f->setGeometry(getGeometryPointer(feat));
	}
	return f;
}

	/**
	* Get the next feature resutling from a select operation
    * Return 0 if there are no features in the selection set
	* @return QgsFeature
	*/
QgsFeature *QgsShapeFileProvider::getNextFeature()
{
    
	QgsFeature *f = 0;
	if(valid){
		//std::cout << "getting next feature\n";
		OGRFeature *fet = ogrLayer->GetNextFeature();
		if(fet){
            OGRGeometry *geom = fet->GetGeometryRef();
			
			// get the wkb representation
			unsigned char *feature = new unsigned char[geom->WkbSize()];
			geom->exportToWkb((OGRwkbByteOrder) endian(), feature);
            f = new QgsFeature();
            f->setGeometry(feature);
            char *wkt = new char[2 * geom->WkbSize()];
            geom->exportToWkt(&wkt);
            f->setWellKnownText(wkt);
		}else{
			std::cout << "Feature is null\n";
            // probably should reset reading here
            ogrLayer->ResetReading();
		}
		
	}else{
        std::cout << "Read attempt on an invalid shapefile data source\n";
    }
	return f;
}

	/**
	* Select features based on a bounding rectangle. Features can be retrieved
	* with calls to getFirstFeature and getNextFeature.
	* @param mbr QgsRect containing the extent to use in selecting features
	*/
void QgsShapeFileProvider::select(QgsRect *rect)
{
    // spatial query to select features
    std::cout << "Selection rectangle is " << *rect << std::endl;
    OGRGeometry *filter = 0;
	filter = new OGRPolygon();
	QString wktExtent = QString("POLYGON ((%1))").arg(rect->stringRep());
	const char *wktText = (const char *)wktExtent;

	OGRErr result = ((OGRPolygon *) filter)->importFromWkt((char **)&wktText);
    //TODO - detect an error in setting the filter and figure out what to
    //TODO   about it. If setting the filter fails, all records will be returned
	if (result == OGRERR_NONE) {
        std::cout << "Setting spatial filter using " << wktExtent    << std::endl;
		ogrLayer->SetSpatialFilter(filter);
        std::cout << "Feature count: " << ogrLayer->GetFeatureCount() << std::endl;
	/* 	int featureCount = 0;
		while (OGRFeature * fet = ogrLayer->GetNextFeature()) {
			if (fet) {
				select(fet->GetFID());
				if (tabledisplay) {
					tabledisplay->table()->selectRowWithId(fet->GetFID());
					(*selected)[fet->GetFID()] = true;
				}
			} 
		}
		ogrLayer->ResetReading();*/
	}else{
        std::cout << "Setting spatial filter failed!" << std::endl;
    }
}

	/**
		* Set the data source specification. This may be a path or database
	* connection string
	* @uri data source specification
	*/
void QgsShapeFileProvider::setDataSourceUri(QString uri)
{
	dataSourceUri = uri;
}

		/**
	* Get the data source specification. This may be a path or database
	* connection string
	* @return data source specification
	*/
QString QgsShapeFileProvider::getDataSourceUri()
{
	return dataSourceUri;
}

	/**
	* Identify features within the search radius specified by rect
	* @param rect Bounding rectangle of search radius
	* @return std::vector containing QgsFeature objects that intersect rect
	*/
std::vector < QgsFeature > QgsShapeFileProvider::identify(QgsRect * rect)
{
	return features;
}

unsigned char * QgsShapeFileProvider::getGeometryPointer(OGRFeature *fet){
	OGRGeometry *geom = fet->GetGeometryRef();
	unsigned char *gPtr=0;
		// get the wkb representation
		gPtr = new unsigned char[geom->WkbSize()];
      
		geom->exportToWkb((OGRwkbByteOrder) endian(), gPtr);
	return gPtr;

}

int QgsShapeFileProvider::endian()
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

// TODO - make this function return the real extent_
QgsRect *QgsShapeFileProvider::extent()
{
  return new QgsRect(extent_->MinX, extent_->MinY, extent_->MaxX, extent_->MaxY);
}

/** 
* Return the feature type
*/
int QgsShapeFileProvider::geometryType(){
    return geomType;
}
/** 
* Return the feature type
*/
long QgsShapeFileProvider::featureCount(){
    return featureCount_;
}
extern "C" QgsShapeFileProvider * classFactory(const char *uri)
{
	return new QgsShapeFileProvider(uri);
}
