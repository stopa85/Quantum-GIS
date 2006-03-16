/***************************************************************************
  qgsxsdxerceswrapper.cpp  -  Wrapper of the Xerces functions for QGIS
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

#include "qgsxsdxerceswrapper.h"

#include <fstream>
#include <iostream>

// Xerces includes
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/internal/XMLGrammarPoolImpl.hpp>
#include <xercesc/validators/schema/XSDDOMParser.hpp>
#include <xercesc/framework/psvi/XSModel.hpp>

// Xerces-C++'s default MemoryManager/XMLGrammarPool implementations.
#include <xercesc/internal/MemoryManagerImpl.hpp>
#include <xercesc/internal/XMLGrammarPoolImpl.hpp>

QgsXsdXercesWrapper::QgsXsdXercesWrapper()
  : QObject()
{
#ifdef QGISDEBUG
  std::cout << "QgsXsdXercesWrapper: constructing." << std::endl;
#endif

  // "your application must initialize the Xerces system before using the API"
  //   http://xml.apache.org/xerces-c/program.html
  try {
    XMLPlatformUtils::Initialize();
  }
  catch (const XMLException& toCatch) {
    //! \todo Do your failure processing here
    return;
  }

  // Create a memory manager instance for memory handling requests.
  mMemoryManager = new MemoryManagerImpl();

  // Create a grammar pool that stores the cached grammars.
  mXmlGrammarPool = new XMLGrammarPoolImpl(mMemoryManager);

#ifdef QGISDEBUG
  std::cout << "QgsXsdXercesWrapper: exiting constructor." << std::endl;
#endif
}


QgsXsdXercesWrapper::~QgsXsdXercesWrapper()
{

  if (mXmlGrammarPool)
  {
    delete mXmlGrammarPool;
  }

  // MemoryManager needs to be the last to be deleted!
  if (mMemoryManager)
  {
    delete mMemoryManager;
  }

  // "your application must initialize the Xerces system before using the API, 
  //  and terminate it after you are done"
  //   http://xml.apache.org/xerces-c/program.html
  XMLPlatformUtils::Terminate();

#ifdef QGISDEBUG
  std::cout << "QgsXsdXercesWrapper: deconstructing." << std::endl;
#endif
}


void QgsXsdXercesWrapper::loadSchema()
{
#ifdef QGISDEBUG
  std::cout << "QgsXsdXercesWrapper: loadSchema: entering." << std::endl;
#endif

  mSchemaValidator = new SchemaValidator();  // TODO or long-form constructor ?

  mXsdDomParser = new XSDDOMParser(
    mSchemaValidator,
    mMemoryManager,                     // TODO or "MemoryManager *const manager=XMLPlatformUtils::fgMemoryManager" ?
    mXmlGrammarPool);

// TODO   parse;

  mXsModel = new XSModel(mXmlGrammarPool);

// TODO   XSModel should now be ready for inspection.


#ifdef QGISDEBUG
  std::cout << "QgsXsdXercesWrapper: loadSchema: exiting." << std::endl;
#endif
}


void QgsXsdXercesWrapper::parseSchema()
{
#ifdef QGISDEBUG
  std::cout << "QgsXsdXercesWrapper: parseSchema: entering." << std::endl;
#endif

  if (mXsModel)
  {
    

  }

#ifdef QGISDEBUG
  std::cout << "QgsXsdXercesWrapper: parseSchema: exiting." << std::endl;
#endif
}


// ENDS
