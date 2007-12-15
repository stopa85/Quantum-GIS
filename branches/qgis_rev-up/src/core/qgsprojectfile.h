/***************************************************************************
                          qgsprojectfile.h  -  description
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

/*
 * This class provides possibility to store a project file as a QDomDocument,
 * and provides the ability to specify version of the project file, and 
 * perform upgrades to a more recent version
 */

/* $Id: $ */

#ifndef QGSPROJECTFILE_H
#define QGSPROJECTFILE_H

#include <QString>
#include <QDomDocument>
#include <vector>

class QgsProjectFileVersion
{

 public:

  QgsProjectFileVersion() {} 
  QgsProjectFileVersion(int major, int minor, int sub)
    {
      mMajor = major;
      mMinor = minor;
      mSub   = sub;
    }

  ~QgsProjectFileVersion() {} 

  int major() { return mMajor;};
  int minor() { return mMinor;};
  int sub()   { return mSub;};

  bool operator==(const QgsProjectFileVersion &other)
    {
      return ((mMajor == other.mMajor) &&
              (mMinor == other.mMinor) &&
              (mSub == other.mSub));
    } 

 private:
  int mMajor;
  int mMinor;
  int mSub;
};

class QgsProjectFile 
{
 public:
  //Default constructor
  //QgsProjectFile() {}
  ~QgsProjectFile() {}
    
  /*! Create an instance from a DOM and a supplied version
   * @param domDocument The DOM document to use as content
   * @param version Version number
   */
  QgsProjectFile(QDomDocument & domDocument,
                 QgsProjectFileVersion version)
    {
      mDom = domDocument;
      mCurrentVersion = version;
    }


  bool revup(QgsProjectFileVersion version);

 private:

  typedef struct {
    QgsProjectFileVersion from;
    QgsProjectFileVersion to;
    void (QgsProjectFile::* transformFunc)();
  } transform;

  static transform transformers[];;

  QDomDocument mDom;
  QgsProjectFileVersion mCurrentVersion;

  // Transformer functions below. Declare functions here,
  // define them in qgsprojectfile.cpp and add them
  // to the transformArray with proper version number
  void transformNull() {}; // Do absolutely nothing
  void transform081to090();
  void transform090to091();
};


#endif //QGSPROJECTFILE_H

