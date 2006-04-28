/***************************************************************************
  qgsdelimitedtextprovider.cpp -  Data provider for delimted text
  -------------------
          begin                : 2004-02-27
          copyright            : (C) 2004 by Gary E.Sherman
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
/* $Id$ */

#include "qgsdelimitedtextprovider.h"

#include <cfloat>
#include <iostream>

#include <QtGlobal>
#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include <QStringList>
#include <QRegExp>
#include <QUrl>

#include <qmessagebox.h>
#include <qsettings.h>


#include "qgsdataprovider.h"
#include "qgsfeature.h"
#include "qgsfeatureattribute.h"
#include "qgsfield.h"
#include "qgsrect.h"


#ifdef WIN32
#define QGISEXTERN extern "C" __declspec( dllexport )
#else
#define QGISEXTERN extern "C"
#endif


static const QString TEXT_PROVIDER_KEY = "delimitedtext";
static const QString TEXT_PROVIDER_DESCRIPTION = "Delimited text data provider";



QgsDelimitedTextProvider::QgsDelimitedTextProvider(QString const &uri)
    : QgsVectorDataProvider(uri), 
      mMinMaxCacheDirty(true)
{
  // Get the file name and mDelimiter out of the uri
  mFileName = uri.left(uri.find("?"));
  // split the string up on & to get the individual parameters
  QStringList parameters = QStringList::split("&", uri.mid(uri.find("?")));
#ifdef QGISDEBUG
  std::cerr << "Parameter count after split on &" << parameters.
    size() << std::endl;
#endif
  // get the individual parameters and assign values
  QStringList temp = parameters.grep("delimiter=");
  mDelimiter = temp.size()? temp[0].mid(temp[0].find("=") + 1) : "";
  temp = parameters.grep("xField=");
  mXField = temp.size()? temp[0].mid(temp[0].find("=") + 1) : "";
  temp = parameters.grep("yField=");
  mYField = temp.size()? temp[0].mid(temp[0].find("=") + 1) : "";
  // Decode the parts of the uri. Good if someone entered '=' as a delimiter, for instance.
  QUrl::decode(mFileName);
  QUrl::decode(mDelimiter);
  QUrl::decode(mXField);
  QUrl::decode(mYField);
#ifdef QGISDEBUG
  std::cerr << "Data source uri is " << (const char *)uri.toLocal8Bit().data() << std::endl;
  std::cerr << "Delimited text file is: " << (const char *)mFileName.toLocal8Bit().data() << std::endl;
  std::cerr << "Delimiter is: " << (const char *)mDelimiter.toLocal8Bit().data() << std::endl;
  std::cerr << "xField is: " << (const char *)mXField.toLocal8Bit().data() << std::endl;
  std::cerr << "yField is: " << (const char *)mYField.toLocal8Bit().data() << std::endl;
#endif
  // Set the selection rectangle to null
  mSelectionRectangle = 0;
  // assume the layer is invalid until proven otherwise
  mValid = false;
  if (!mFileName.isEmpty() && !mDelimiter.isEmpty() && !mXField.isEmpty() &&
      !mYField.isEmpty())
  {
    // check to see that the file exists and perform some sanity checks
    if (QFile::exists(mFileName))
    {
      // Open the file and get number of rows, etc. We assume that the
      // file has a header row and process accordingly. Caller should make
      // sure the the delimited file is properly formed.
      mFile = new QFile(mFileName);
      if (mFile->open(QIODevice::ReadOnly))
      {
        QTextStream stream(mFile);
        QString line;
        mNumberFeatures = 0;
        int xyCount = 0;
        int lineNumber = 0;
        // set the initial extent
        mExtent = new QgsRect();
        //commented out by Tim for now - setMinimal needs to be merged in from 0.7 branch
        //mExtent->setMinimal(); // This defeats normalization
        while (!stream.atEnd())
        {
          lineNumber++;
          line = stream.readLine(); // line of text excluding '\n', default local 8 bit encoding.
          if (mNumberFeatures++ == 0)
          {
            // Get the fields from the header row and store them in the 
            // fields vector
#ifdef QGISDEBUG
            std::
              cerr << "Attempting to split the input line: " << (const char *)line.toLocal8Bit().data() <<
              " using delimiter " << (const char *)mDelimiter.toLocal8Bit().data() << std::endl;
#endif
            QStringList fieldList =
              QStringList::split(QRegExp(mDelimiter), line, true);
#ifdef QGISDEBUG
            std::cerr << "Split line into " << fieldList.
              size() << " parts" << std::endl;
#endif
            // We don't know anything about a text based field other
            // than its name. All fields are assumed to be text
            int fieldPos = 0;
            for (QStringList::Iterator it = fieldList.begin();
                 it != fieldList.end(); ++it)
            {
              QString field = *it;
              if (field.length() > 0)
              {
                attributeFields.push_back(QgsField(*it, "Text"));
                fieldPositions[*it] = fieldPos++;
                // check to see if this field matches either the x or y field 
                if (mXField == *it)
                {
#ifdef QGISDEBUG
                  std::cerr << "Found x field " << (const char *)(*it).toLocal8Bit().data() << std::endl;
#endif
                  xyCount++;
                }
                if (mYField == *it)
                {
#ifdef QGISDEBUG
                  std::cerr << "Found y field " << (const char *)(*it).toLocal8Bit().data() << std::endl;
#endif
                  xyCount++;
                }
#ifdef QGISDEBUG
                std::cerr << "Adding field: " << (const char *)(*it).toLocal8Bit().data() << std::endl;
#endif

              }
            }
#ifdef QGISDEBUG
            std::
              cerr << "Field count for the delimited text file is " <<
              attributeFields.size() << std::endl;
#endif
          }
          else
          {
            // examine the x,y and update extents
            //  std::cout << line << std::endl; 
            // split the line on the delimiter
            QStringList parts =
              QStringList::split(QRegExp(mDelimiter), line, true);
            //if(parts.size() == attributeFields.size())
            //{
            //  // we can populate attributes if required
            //  fieldsMatch = true;
            //}else
            //{
            //  fieldsMatch = false;
            //}
            /*
               std::cout << "Record hit line " << lineNumber << ": " <<
               parts[fieldPositions[mXField]] << ", " <<
               parts[fieldPositions[mYField]] << std::endl;
             */
            // Get the x and y values, first checking to make sure they
            // aren't null.
            QString sX = parts[fieldPositions[mXField]];
            QString sY = parts[fieldPositions[mYField]];
            //std::cout << "x ,y " << sX << ", " << sY << std::endl; 
            bool xOk = true;
            bool yOk = true;
            double x = sX.toDouble(&xOk);
            double y = sY.toDouble(&yOk);
            if (xOk && yOk)
            {
              if (x > mExtent->xMax())
              {
                mExtent->setXmax(x);
              }
              if (x < mExtent->xMin())
              {
                mExtent->setXmin(x);
              }
              if (y > mExtent->yMax())
              {
                mExtent->setYmax(y);
              }
              if (y < mExtent->yMin())
              {
                mExtent->setYmin(y);
              }
            }
          }
        }
        reset();
        mNumberFeatures--;

        if (xyCount == 2)
        {
#ifdef QGISDEBUG
          std::cerr << "Data store is valid" << std::endl;
          std::cerr << "Number of features " << mNumberFeatures << std::endl;
          std::cerr << "Extents " << (const char *)mExtent->stringRep().toLocal8Bit().data() << std::endl;
#endif
          mValid = true;
        }
        else
        {
          std::
            cerr << "Data store is invalid. Specified x,y fields do not match\n"
            << "those in the database (xyCount=" << xyCount << ")" << std::endl;
        }
      }
#ifdef QGISDEBUG
      std::cerr << "Done checking validity\n";
#endif

      //resize the cache matrix
      mMinMaxCache = new double *[attributeFields.size()];
      for (int i = 0; i < attributeFields.size(); i++)
      {
        mMinMaxCache[i] = new double[2];
      }
    }
    else
      // file does not exist
      std::
        cerr << "Data source " << (const char *)getDataSourceUri().toLocal8Bit().data() << " could not be opened" <<
        std::endl;

  }
  else
  {
    // uri is invalid so the layer must be too...
    std::cerr << "Data source is invalid" << std::endl;

  }
}

QgsDelimitedTextProvider::~QgsDelimitedTextProvider()
{
  mFile->close();
  delete mFile;
  for (int i = 0; i < fieldCount(); i++)
  {
    delete mMinMaxCache[i];
  }
  delete[]mMinMaxCache;
}


QString QgsDelimitedTextProvider::storageType()
{
  return "Delimited text file";
}


/**
 * Get the first feature resutling from a select operation
 * @return QgsFeature
 */
QgsFeature * QgsDelimitedTextProvider::getFirstFeature(bool fetchAttributes)
{
    QgsFeature *f = new QgsFeature;

    reset();                    // reset back to first feature

    if ( getNextFeature_( *f, fetchAttributes ) )
    {
        return f;
    }

    delete f;

    return 0x0;
} // QgsDelimitedTextProvider::getFirstFeature(bool fetchAttributes)

/**

  insure double value is properly translated into locate endian-ness

*/
static
double
translateDouble_( double d )
{
    union
    {
        double fpval;
        char   char_val[8];
    } from, to;

    // break double into byte sized chunks
    from.fpval = d;

    to.char_val[7] = from.char_val[0];
    to.char_val[6] = from.char_val[1];
    to.char_val[5] = from.char_val[2];
    to.char_val[4] = from.char_val[3];
    to.char_val[3] = from.char_val[4];
    to.char_val[2] = from.char_val[5];
    to.char_val[1] = from.char_val[6];
    to.char_val[0] = from.char_val[7];

    return to.fpval;

} // translateDouble_


bool
QgsDelimitedTextProvider::getNextFeature_( QgsFeature & feature, 
                                           bool getAttributes,
                                           std::list<int> const * desiredAttributes )
{
    // before we do anything else, assume that there's something wrong with
    // the feature
    feature.setValid( false );

    QTextStream textStream( mFile );

    if ( ! textStream.atEnd() )
    {
      QString line = textStream.readLine(); // Default local 8 bit encoding

        // lex the tokens from the current data line
        QStringList tokens = QStringList::split(QRegExp(mDelimiter), line, true);

        bool xOk = false;
        bool yOk = false;

        int xFieldPos = fieldPositions[mXField];
        int yFieldPos = fieldPositions[mYField];

        double x = tokens[xFieldPos].toDouble( &xOk );
        double y = tokens[yFieldPos].toDouble( &yOk );

        if ( xOk && yOk )
        {
            // if the user has selected an area, constrain iterator to
            // features that are within that area
            if ( mSelectionRectangle && ! boundsCheck(x,y) )
            {
                bool foundFeature = false;

                while ( ! textStream.atEnd() && 
                        (xOk && yOk) )
                {
                    if ( boundsCheck(x,y) )
                    {
                        foundFeature = true;
                        break;
                    }

                    ++mFid;     // since we're skipping to next feature,
                                // increment ID

                    line = textStream.readLine();

                    tokens = QStringList::split(QRegExp(mDelimiter), line, true);

                    x = tokens[xFieldPos].toDouble( &xOk );
                    y = tokens[yFieldPos].toDouble( &yOk );
                }

                // there were no other features from the current one forward
                // that were within the selection region
                if ( ! foundFeature )
                {
                    return false;
                }
            }

            // at this point, one way or another, the current feature values
            // are valid
           feature.setValid( true );

           ++mFid;             // increment to next feature ID

           feature.setFeatureId( mFid );

           unsigned char * geometry = new unsigned char[sizeof(wkbPoint)];
           QByteArray  buffer;
           buffer.setRawData( (const char*)geometry, sizeof(wkbPoint) ); // buffer
                                                                         // points
                                                                         // to
                                                                         // geometry

#if QT_VERSION < 0x040000
           QDataStream s( buffer, QIODevice::WriteOnly ); // open on buffers's data
#else
           QDataStream s( &buffer, QIODevice::WriteOnly ); // open on buffers's data
#endif

           switch ( endian() )
           {
               case QgsDataProvider::NDR :
                   // we're on a little-endian platform, so tell the data
                   // stream to use that
                   s.setByteOrder( QDataStream::LittleEndian );
                   s << (Q_UINT8)1; // 1 is for little-endian
                   break;
               case QgsDataProvider::XDR :
                   // don't change byte order since QDataStream is big endian by default
                   s << (Q_UINT8)0; // 0 is for big-endian
                   break;
               default :
                   qDebug( "%s:%d unknown endian", __FILE__, __LINE__ );
                   delete [] geometry;
                   return false;
           }

           s << (Q_UINT32)1; // 1 is for WKBPoint
           s << x;
           s << y;


           feature.setGeometryAndOwnership( geometry, sizeof(wkbPoint) );

           // ensure that the buffer doesn't delete the data on us
           buffer.resetRawData( (const char*)geometry, sizeof(wkbPoint) );

           if ( getAttributes && ! desiredAttributes )
           {
               for (int fi = 0; fi < attributeFields.size(); fi++)
               {
                   feature.addAttribute(attributeFields[fi].name(), tokens[fi]);
               }
           }
           // regardless of whether getAttributes is true or not, if the
           // programmer went through the trouble of passing in such a list of
           // attribute fields, then obviously they want them
           else if ( desiredAttributes )
           {
               for ( std::list<int>::const_iterator i = desiredAttributes->begin();
                     i != desiredAttributes->end();
                     ++i )
               {
                   feature.addAttribute(attributeFields[*i].name(), tokens[*i]);
               }
           }

           return true;

        } // if able to get x and y coordinates

    } // ! textStream EOF

    return false;

} // getNextFeature_( QgsFeature & feature )



/**
  Get the next feature resulting from a select operation
  Return 0 if there are no features in the selection set
 * @return false if unable to get the next feature
 */
bool QgsDelimitedTextProvider::getNextFeature(QgsFeature & feature,
                                              bool fetchAttributes)
{
    return getNextFeature_( feature, fetchAttributes );
} // QgsDelimitedTextProvider::getNextFeature



QgsFeature * QgsDelimitedTextProvider::getNextFeature(bool fetchAttributes)
{
    QgsFeature * f = new QgsFeature;

    if ( getNextFeature( *f, fetchAttributes ) )
    {
        return f;
    }
    
    delete f;

    return 0x0;
} // QgsDelimitedTextProvider::getNextFeature(bool fetchAttributes)



QgsFeature * QgsDelimitedTextProvider::getNextFeature(std::list<int> const & desiredAttributes, int featureQueueSize)
{
    QgsFeature * f = new QgsFeature;

    if ( getNextFeature_( *f, true, &desiredAttributes ) )
    {
        return f;
    }
    
    delete f;

    return 0x0;

} // QgsDelimitedTextProvider::getNextFeature(std::list < int >&attlist)




/**
 * Select features based on a bounding rectangle. Features can be retrieved
 * with calls to getFirstFeature and getNextFeature.
 * @param mbr QgsRect containing the extent to use in selecting features
 */
void QgsDelimitedTextProvider::select(QgsRect * rect, bool useIntersect)
{

  // Setting a spatial filter doesn't make much sense since we have to
  // compare each point against the rectangle.
  // We store the rect and use it in getNextFeature to determine if the
  // feature falls in the selection area
  mSelectionRectangle = new QgsRect((*rect));
  // Select implies an upcoming feature read so we reset the data source
  reset();
  // Reset the feature id to 0
  mFid = 0;

}


/**
 * Identify features within the search radius specified by rect
 * @param rect Bounding rectangle of search radius
 * @return std::vector containing QgsFeature objects that intersect rect
 */
std::vector < QgsFeature > &QgsDelimitedTextProvider::identify(QgsRect * rect)
{
  // reset the data source since we need to be able to read through
  // all features
  reset();
  std::cerr << "Attempting to identify features falling within " << (const char *)rect->
    stringRep().toLocal8Bit().data() << std::endl;
  // select the features
  select(rect);
#ifdef WIN32
  //TODO fix this later for win32
  std::vector < QgsFeature > feat;
  return feat;
#endif

}

/*
   unsigned char * QgsDelimitedTextProvider::getGeometryPointer(OGRFeature *fet){
   unsigned char *gPtr=0;
// get the wkb representation

//geom->exportToWkb((OGRwkbByteOrder) endian(), gPtr);
return gPtr;

}
*/


// Return the extent of the layer
QgsRect *QgsDelimitedTextProvider::extent()
{
  return new QgsRect(mExtent->xMin(), mExtent->yMin(), mExtent->xMax(),
                     mExtent->yMax());
}

/** 
 * Return the feature type
 */
int QgsDelimitedTextProvider::geometryType() const
{
  return 1;                     // WKBPoint
}

/** 
 * Return the feature type
 */
long QgsDelimitedTextProvider::featureCount() const
{
  return mNumberFeatures;
}

/**
 * Return the number of fields
 */
int QgsDelimitedTextProvider::fieldCount() const
{
  return attributeFields.size();
}

/**
 * Fetch attributes for a selected feature
 */
void QgsDelimitedTextProvider::getFeatureAttributes(int key, QgsFeature * f)
{
  //for (int i = 0; i < ogrFet->GetFieldCount(); i++) {

  //  // add the feature attributes to the tree
  //  OGRFieldDefn *fldDef = ogrFet->GetFieldDefnRef(i);
  //  QString fld = fldDef->GetNameRef();
  //  //    OGRFieldType fldType = fldDef->GetType();
  //  QString val;

  //  val = ogrFet->GetFieldAsString(i);
  //  f->addAttribute(fld, val);
  //}
}

std::vector<QgsField> const & QgsDelimitedTextProvider::fields() const
{
  return attributeFields;
}

void QgsDelimitedTextProvider::reset()
{
  // Reset the file pointer to BOF
  mFile->reset();
  // Reset feature id to 0
  mFid = 0;
  // Skip ahead one line since first record is always assumed to be
  // the header record
  QTextStream stream(mFile);
  stream.readLine();
}

QString QgsDelimitedTextProvider::minValue(int position)
{
  if (position >= fieldCount())
  {
    std::
      cerr << "Warning: access requested to invalid position " <<
      "in QgsDelimitedTextProvider::minValue(..)" << std::endl;
  }
  if (mMinMaxCacheDirty)
  {
    fillMinMaxCash();
  }
  return QString::number(mMinMaxCache[position][0], 'f', 2);
}


QString QgsDelimitedTextProvider::maxValue(int position)
{
  if (position >= fieldCount())
  {
    std::
      cerr << "Warning: access requested to invalid position " <<
      "in QgsDelimitedTextProvider::maxValue(..)" << std::endl;
  }
  if (mMinMaxCacheDirty)
  {
    fillMinMaxCash();
  }
  return QString::number(mMinMaxCache[position][1], 'f', 2);
}

void QgsDelimitedTextProvider::fillMinMaxCash()
{
  for (int i = 0; i < fieldCount(); i++)
  {
    mMinMaxCache[i][0] = DBL_MAX;
    mMinMaxCache[i][1] = -DBL_MAX;
  }

  QgsFeature f;
  reset();

  getNextFeature(f, true);
  do
  {
    for (int i = 0; i < fieldCount(); i++)
    {
      double value = (f.attributeMap())[i].fieldValue().toDouble();
      if (value < mMinMaxCache[i][0])
      {
        mMinMaxCache[i][0] = value;
      }
      if (value > mMinMaxCache[i][1])
      {
        mMinMaxCache[i][1] = value;
      }
    }
  }
  while (getNextFeature(f, true));

  mMinMaxCacheDirty = false;
}

//TODO - add sanity check for shape file layers, to include cheking to
//       see if the .shp, .dbf, .shx files are all present and the layer
//       actually has features
bool QgsDelimitedTextProvider::isValid()
{
  return mValid;
}

/** 
 * Check to see if the point is within the selection rectangle
 */
bool QgsDelimitedTextProvider::boundsCheck(double x, double y)
{
  bool inBounds = (((x < mSelectionRectangle->xMax()) &&
                    (x > mSelectionRectangle->xMin())) &&
                   ((y < mSelectionRectangle->yMax()) &&
                    (y > mSelectionRectangle->yMin())));
  // QString hit = inBounds?"true":"false";

  // std::cerr << "Checking if " << x << ", " << y << " is in " << 
  //mSelectionRectangle->stringRep().ascii() << ": " << hit.ascii() << std::endl; 
  return inBounds;
}

int QgsDelimitedTextProvider::capabilities() const
{
    return 0;
}



size_t QgsDelimitedTextProvider::layerCount() const
{
    return 1;                   // XXX How to calculate the layers?
} // QgsOgrProvider::layerCount()



int *QgsDelimitedTextProvider::getFieldLengths()
{
  // this function parses the entire data file and calculates the
  // max for each

  // Only do this if we haven't done it already (ie. the vector is
  // empty)
  int *lengths = new int[attributeFields.size()];
  // init the lengths to zero
  for (int il = 0; il < attributeFields.size(); il++)
  {
    lengths[il] = 0;
  }
  if (mValid)
  {
    reset();
    // read the line
    QTextStream stream(mFile);
    QString line;
    while (!stream.atEnd())
    {
      line = stream.readLine(); // line of text excluding '\n'
      // split the line
      QStringList parts = QStringList::split(QRegExp(mDelimiter), line, true);
      // iterate over the parts and update the max value
      for (int i = 0; i < parts.size(); i++)
      {
        if (parts[i] != QString::null)
        {
          // std::cerr << "comparing length for " << parts[i] << " against max len of " << lengths[i] << std::endl; 
          if (parts[i].length() > lengths[i])
          {
            lengths[i] = parts[i].length();
          }
        }

      }
    }
  }
  return lengths;
}





QString  QgsDelimitedTextProvider::name() const
{
    return TEXT_PROVIDER_KEY;
} // ::name()



QString  QgsDelimitedTextProvider::description() const
{
    return TEXT_PROVIDER_DESCRIPTION;
} //  QgsDelimitedTextProvider::name()


/**
 * Class factory to return a pointer to a newly created 
 * QgsDelimitedTextProvider object
 */
QGISEXTERN QgsDelimitedTextProvider *classFactory(const QString *uri)
{
  return new QgsDelimitedTextProvider(*uri);
}

/** Required key function (used to map the plugin to a data store type)
*/
QGISEXTERN QString providerKey()
{
    return TEXT_PROVIDER_KEY;
}

/**
 * Required description function 
 */
QGISEXTERN QString description()
{
    return TEXT_PROVIDER_DESCRIPTION;
}

/**
 * Required isProvider function. Used to determine if this shared library
 * is a data provider plugin
 */
QGISEXTERN bool isProvider()
{
  return true;
}
