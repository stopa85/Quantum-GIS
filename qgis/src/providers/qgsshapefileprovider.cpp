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
		// get the extent (envelope) of the layer
		extent = new OGREnvelope();
		ogrLayer->GetExtent(extent);
		// get the feature type
		OGRFeature *feat = ogrLayer->GetNextFeature();
		if (feat) {
			OGRGeometry *geom = feat->GetGeometryRef();
			if (geom) {
				geometryType = geom->getGeometryType();
				ogrLayer->ResetReading();
			} else {
				valid = false;
			}
			delete feat;
		} else {
			valid = false;
		}
	} else {
		std::cout << "Data source is invalid" << std::endl;
		const char *er = CPLGetLastErrorMsg();
		std::cout << er << std::endl;
		valid = false;
	}

	//create a boolean vector and set every entry to false

	if (valid) {
		selected = new std::vector < bool > (ogrLayer->GetFeatureCount(), false);
	} else {
		selected = 0;
	}
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
	ogrLayer->ResetReading();
	OGRFeature *feat = ogrLayer->GetNextFeature();
	QgsFeature *f = new QgsFeature();
	f->setGeometry(getGeometryPointer(feat));
	return f;
}

	/** 
	* Get the next feature resutling from a select operation
	* @return QgsFeature
	*/
QgsFeature *QgsShapeFileProvider::getNextFeature()
{
	return (new QgsFeature());
}

	/**
	* Select features based on a bounding rectangle. Features can be retrieved 
	* with calls to getFirstFeature and getNextFeature.
	* @param extent QgsRect containing the extent to use in selecting features
	*/
void QgsShapeFileProvider::select()
{
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
	return QString("uri");
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
extern "C" QgsShapeFileProvider * classFactory(const char *uri)
{
	return new QgsShapeFileProvider(uri);
}
