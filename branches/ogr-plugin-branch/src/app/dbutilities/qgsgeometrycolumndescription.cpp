/***************************************************************************
    qgsgeometrycolumndescription.h - Description of Spatial Database Column
     --------------------------------------
    Date                 : 26-02-2008
    Copyright            : (C) 2007 by Godofredo Contreras
    Email                : frdc at hotmail dot com
***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

/* $Id$ */

#include "qgsgeometrycolumndescription.h"

QgsGeometryColumnDescription::QgsGeometryColumnDescription(){
 }

QgsGeometryColumnDescription::QgsGeometryColumnDescription(QString type, QString schema, QString table, QString column){
  mType=type;
  mSchema=schema;
  mTable=table;
  mColumn=column;

 } 
QgsGeometryColumnDescription::QgsGeometryColumnDescription(QgsGeometryColumnDescription *geometrycolumn){
  //this=geometrycolumn;                                                                                      
  //mType=geometrycolumn->type();
  //mSchema=geometrycolumn->schema();
  //mTable=geometrycolumn->table();
  //mColumn=geometrycolumn->column();
 }
 
QgsGeometryColumnDescription::~QgsGeometryColumnDescription(){
 }

void QgsGeometryColumnDescription::setType(QString type){
     mType=type;
 }
void QgsGeometryColumnDescription::setSchema(QString schema){
   mSchema=schema;
 }
void QgsGeometryColumnDescription::setTable(QString table){
   mTable=table;
 }
void QgsGeometryColumnDescription::setColumn(QString column){
   mColumn=column;
 }
QString QgsGeometryColumnDescription::type(){
   return mType;
 }
QString QgsGeometryColumnDescription::schema(){
   return mSchema;
 }
QString QgsGeometryColumnDescription::table(){
   return mTable;
 }
QString QgsGeometryColumnDescription::column(){
   return mColumn;
 }



