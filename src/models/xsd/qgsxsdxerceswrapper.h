/***************************************************************************
  qgsxsdxerceswrapper.h  -  Wrapper of the Xerces functions for QGIS
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

#ifndef QGSXSDXERCESWRAPPER_H
#define QGSXSDXERCESWRAPPER_H

#include <QObject>

// Xerces includes
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/framework/XMLGrammarPool.hpp>
#include <xercesc/framework/MemoryManager.hpp>
#include <xercesc/validators/schema/SchemaValidator.hpp>
#include <xercesc/validators/schema/XSDDOMParser.hpp>
#include <xercesc/framework/psvi/XSModel.hpp>
XERCES_CPP_NAMESPACE_USE

/**

  \brief  Wrapper of the Xerces functions for QGIS.

  The intent is to wrap all Xerces includes into this class
  so that other QGIS classes don't have to.

  Requires the Xerces headers (e.g. xerces-c-devel on RedHat)

  References: "Cache and serialize XML Schema with Xerces-C++"
    http://www-128.ibm.com/developerworks/webservices/library/x-xsdxerc.html

    http://permalink.gmane.org/gmane.text.xml.xerces-c.devel/6797

  \todo   Make it work

*/

class QgsXsdXercesWrapper : public QObject
{

  Q_OBJECT

public:
  /**
   * \brief Constructor.
   */
  QgsXsdXercesWrapper();

  //! Destructor
  virtual ~QgsXsdXercesWrapper();

  /**
   * \brief Load an XML Schema.
   *
   * \todo   Make it work - add file / buffer accessor functions
   *
   */
  virtual void loadSchema();


public slots:


signals:


private:

  //! Parse from the root of the loaded schema
  void parseSchema();


  //! Xerces Memory Manager
  MemoryManager * mMemoryManager;

  //! Cached XML Grammar Pool from Xerces
  XMLGrammarPool * mXmlGrammarPool;

  //! Schema Validator from Xerces
  SchemaValidator * mSchemaValidator;

  //! XSD DOM Parser from Xerces
  XSDDOMParser * mXsdDomParser;

  //! XS Model from Xerces
  XSModel * mXsModel;

};


#endif

// ENDS
