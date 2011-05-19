# -*- coding: utf-8 -*-
#-----------------------------------------------------------
#
# fTools
# Copyright (C) 2008-2011  Carson Farmer
# EMAIL: carson.farmer (at) gmail.com
# WEB  : http://www.ftools.ca/fTools.html
#
# A collection of data management and analysis tools for vector data
#
#-----------------------------------------------------------
#
# licensed under the terms of GNU GPL 2
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
#---------------------------------------------------------------------

from PyQt4.QtCore import *
from PyQt4.QtGui import *

from qgis.core import *
from qgis.gui import *

import ftools_utils

from ui_frmUnionNodes import Ui_Dialog

class Dialog( QDialog, Ui_Dialog ):
  def __init__( self, iface ):
    QDialog.__init__( self )
    self.setupUi( self )
    self.iface = iface

    self.simplifyThread = None

    self.okButton = self.buttonBox.button( QDialogButtonBox.Ok )
    self.closeButton = self.buttonBox.button( QDialogButtonBox.Close )

    QObject.connect( self.writeShapefileCheck, SIGNAL( "stateChanged( int )" ), self.updateGui )
    QObject.connect( self.btnSelectOutputFile, SIGNAL( "clicked()" ), self.selectOutputFile )

    self.manageGui()

  def manageGui( self ):
    layers = ftools_utils.getLayerNames( [ QGis.Polygon, QGis.Line ] )
    self.cmbInputLayer.addItems( layers )

  def updateGui( self ):
    if self.writeShapefileCheck.isChecked():
      self.outputFileEdit.setEnabled( True )
      self.btnSelectOutputFile.setEnabled( True )
      self.addToCanvasCheck.setEnabled( True )
    else:
      self.outputFileEdit.setEnabled( False )
      self.btnSelectOutputFile.setEnabled( False )
      self.addToCanvasCheck.setEnabled( False )
    self.encoding = None

  def selectOutputFile( self ):
    self.outputFileEdit.clear()
    (self.shapefileName, self.encoding) = ftools_utils.saveDialog(self)
    if self.shapefileName is None or self.encoding is None:
      return
    self.outputFileEdit.setText(QString(self.shapefileName))

  def accept( self ):
    vLayer = ftools_utils.getVectorLayerByName( self.cmbInputLayer.currentText() )

    QApplication.setOverrideCursor( Qt.WaitCursor )
    self.okButton.setEnabled( False )

    if self.writeShapefileCheck.isChecked():
      outFileName = self.outputFileEdit.text()
      outFile = QFile( outFileName )
      if outFile.exists():
        if not QgsVectorFileWriter.deleteShapeFile( outFileName ):
          QmessageBox.warning( self, self.tr( "Delete error" ), self.tr( "Can't delete file %1" ).arg( outFileName ) )
          return
      self.simplifyThread = GeneralizationThread( vLayer, self.useSelectionCheck.isChecked(), self.toleranceSpin.value(), True, outFileName, self.encoding )
    else:
      self.simplifyThread = GeneralizationThread( vLayer, self.useSelectionCheck.isChecked(), self.toleranceSpin.value(), False, None, None  )
    QObject.connect( self.simplifyThread, SIGNAL( "rangeCalculated( PyQt_PyObject )" ), self.setProgressRange )
    QObject.connect( self.simplifyThread, SIGNAL( "featureProcessed()" ), self.featureProcessed )
    QObject.connect( self.simplifyThread, SIGNAL( "generalizationFinished( PyQt_PyObject )" ), self.generalizationFinished )
    QObject.connect( self.simplifyThread, SIGNAL( "generalizationInterrupted()" ), self.generalizationInterrupted )

    self.closeButton.setText( self.tr( "Cancel" ) )
    QObject.disconnect( self.buttonBox, SIGNAL( "rejected()" ), self.reject )
    QObject.connect( self.closeButton, SIGNAL( "clicked()" ), self.stopProcessing )

    self.simplifyThread.start()

  def setProgressRange( self, max ):
    self.progressBar.setRange( 0, max )

  def featureProcessed( self ):
    self.progressBar.setValue( self.progressBar.value() + 1 )

  def generalizationFinished( self, pointsCount ):
    self.stopProcessing()

    QMessageBox.information( self,
                             self.tr( "Simplify results" ),
                             self.tr( "There were %1 vertices in original dataset which\nwere reduced to %2 vertices after simplification" )
                             .arg( pointsCount[ 0 ] )
                             .arg( pointsCount[ 1 ] ) )

    self.restoreGui()
    if self.addToCanvasCheck.isEnabled() and self.addToCanvasCheck.isChecked():
      if not ftools_utils.addShapeToCanvas( unicode( self.shapefileName ) ):
        QMessageBox.warning( self, self.tr( "Merging" ),
                             self.tr( "Error loading output shapefile:\n%1" )
                             .arg( unicode( self.shapefileName ) ) )

    self.iface.mapCanvas().refresh()
    #self.restoreGui()

  def generalizationInterrupted( self ):
    self.restoreGui()

  def stopProcessing( self ):
    if self.simplifyThread != None:
      self.simplifyThread.stop()
      self.simplifyThread = None

  def restoreGui( self ):
    self.progressBar.setValue( 0 )
    QApplication.restoreOverrideCursor()
    QObject.connect( self.buttonBox, SIGNAL( "rejected()" ), self.reject )
    self.closeButton.setText( self.tr( "Close" ) )
    self.okButton.setEnabled( True )

def geomVertexCount( geometry ):
  geomType = geometry.type()
  if geomType == 1: # line
    points = geometry.asPolyline()
    return len( points )
  elif geomType == 2: # polygon
    polylines = geometry.asPolygon()
    points = []
    for l in polylines:
      points.extend( l )
    return len( points )
  else:
    return None

def QgsPointCmp( p1, p2 ):
  if p1[0].x() < p2[0].x():
    return -1
  elif p1[0].x() > p2[0].x():
    return 1
  else:
    if p1[0].y() < p2[0].y():
      return -1
    elif p1[0].y() > p2[0].y():
      return 1
  return 0

def binarySearch( a, v ):
  minimum = 0;
  maximum = len(a)-1
  while True:
    avg = minimum + (maximum-minimum)/2
    if avg == maximum or avg == minimum:
      if QgsPointCmp(a[minimum],[v])==0:
        return minimum
      elif QgsPointCmp(a[maximum],[v])==0:
        return maximum
      else:
        return -1

    if QgsPointCmp(a[avg],[v])==0:
      return avg
    elif QgsPointCmp(a[avg],[v])>0:
      maximum = avg
    elif QgsPointCmp(a[avg],[v])<0:
      minimum = avg
  return -1

class GeneralizationThread( QThread ):
  def __init__( self, inputLayer, useSelection, tolerance, writeShape, shapePath, shapeEncoding ):
    QThread.__init__( self, QThread.currentThread() )
    self.inputLayer = inputLayer
    self.useSelection = useSelection
    self.tolerance = tolerance
    self.writeShape = writeShape
    self.outputFileName = shapePath
    self.outputEncoding = shapeEncoding

    self.shapeFileWriter = None

    self.mutex = QMutex()
    self.stopMe = 0
    
    self.points = []
    self.indx   = QgsSpatialIndex()
    self.diction = []

  def extractNodesFromGeometry( self, geom ):
    points = []
    t = geom.type()
    polylines = []
    if t == 1:
      polylines = [ geom.asPolyline() ]
    elif t == 2:
      t = geom.asPolygon()
      for v in t:
        polylines.append( v[:-1] )
    
    for polyline in polylines:
      for p in polyline:
        points.append([ p, [ p ] ])
    return points
 
  def extractNodes( self ):
    points = []
    if self.useSelection:
      selection = self.inputLayer.selectedFeatures()
      for f in selection:
        featGeometry = QgsGeometry( f.geometry() )
        points += self.extractNodesFromGeometry( featGeometry )
    else:
      self.inputLayer.select()
      f = QgsFeature()
      while self.inputLayer.nextFeature( f ):
        featGeometry = QgsGeometry( f.geometry() )
        points += self.extractNodesFromGeometry( featGeometry )

    return points

  def makeNewGeometry( self, featGeometry ):
    t = featGeometry.type()
    polylines = [];
    if t == 1:
      polylines = [ featGeometry.asPolyline() ]
    elif t == 2:
      polylines = featGeometry.asPolygon()
    new_polylines = []
    for polyline in polylines:
      new_polyline = []
      for point in polyline:
        b = binarySearch( self.diction, point )
        if b == -1:
          new_polyline.append( point )
        else:
          new_polyline.append( self.points[ self.diction[b][1] ][0] )
      
      new_polylines.append( new_polyline )
    new_geometry = None
    if t == 1:
      new_geometry = QgsGeometry.fromPolyline( new_polylines[0] )
    elif t == 2:
      new_geometry = QgsGeometry.fromPolygon( new_polylines )
    return new_geometry
  
  
  def addPoints( self, points ):
    i = 0;
    while i < len(points):
      p = points[i][0]
      res = self.indx.nearestNeighbor( p, 1 )
      pointId = None
      if len(res) != 0:
        pointId = res[0]
      
      if pointId != None and self.points[pointId][0].sqrDist( p ) < self.tolerance:
        k1 = len( self.points[pointId][1] )
        p1 = self.points[pointId][0]
        p2 = QgsPoint( (k1*p1.x()+p.x())/(k1+1) , (k1*p1.y()+p.y())/(k1+1) )
        self.points[pointId][0] = p2;
        self.points[pointId][1] += [ p ]

        #update self.indx
        f = QgsFeature()
        f.setFeatureId( pointId )
        f.setGeometry( QgsGeometry.fromPoint( p1 ) )
        self.indx.deleteFeature( f )
        f.setGeometry( QgsGeometry.fromPoint( p2 ) )
        self.indx.insertFeature( f )
       
      else:
        pointId = len(self.points)
        self.points.append( [ p, [p] ] )
        
        #insert self.indx
        f = QgsFeature()
        f.setFeatureId( pointId )
        f.setGeometry( QgsGeometry.fromPoint( p ) )
        self.indx.insertFeature( f )
      
      i = i + 1

  def run( self ):
    self.mutex.lock()
    self.stopMe = 0
    self.mutex.unlock()

    interrupted = False
    
    shapeFileWriter = None
    
    self.points = []
    if self.useSelection:
      selection = self.inputLayer.selectedFeatures()
      self.emit( SIGNAL("rangeCalculated( PyQt_PyObject )"), len( selection )*2 )
      for f in selection:
        featGeometry = QgsGeometry( f.geometry() )
        self.addPoints( self.extractNodesFromGeometry( featGeometry ) )
        self.emit( SIGNAL( "featureProcessed()" ) )
        
    else:
      self.inputLayer.select()
      self.emit( SIGNAL("rangeCalculated( PyQt_PyObject )"), self.inputLayer.featureCount() )
      f = QgsFeature()
      while self.inputLayer.nextFeature( f ):     
        featGeometry = QgsGeometry( f.geometry() )
        self.addPoints( self.extractNodesFromGeometry( featGeometry ) )
        self.emit( SIGNAL( "featureProcessed()" ) )

    i=0
    while i<len(self.points):
      for p in self.points[i][1]:
        self.diction.append( [ p, i ] )
      i=i+1

    self.diction = sorted( self.diction, cmp=QgsPointCmp )
   
    if not self.inputLayer.isEditable():
      self.inputLayer.startEditing()
      self.inputLayer.beginEditCommand( QString( "Simplify line(s)" ) )
 
    if self.useSelection:
      selection = self.inputLayer.selectedFeatures()
      for f in selection:
        featGeometry = QgsGeometry( f.geometry() )
        new_geom = self.makeNewGeometry( featGeometry )
        self.inputLayer.changeGeometry( f.id(), new_geom )
        self.emit( SIGNAL( "featureProcessed()" ) )

    else:
      self.inputLayer.select()
      f = QgsFeature()
      while self.inputLayer.nextFeature( f ):
        featGeometry = QgsGeometry( f.geometry() )
        new_geom = self.makeNewGeometry( featGeometry )
        featureId = f.id()
        self.inputLayer.changeGeometry( f.id(), new_geom )
        self.emit( SIGNAL( "featureProcessed()" ) )
    

    if self.inputLayer.isEditable():
      self.inputLayer.endEditCommand()

    if shapeFileWriter != None:
      del shapeFileWriter

    if not interrupted:
      self.emit( SIGNAL( "generalizationFinished( PyQt_PyObject )" ), ( 0, 0 ) )
    else:
      self.emit( SIGNAL( "generalizationInterrupted()" ) )

  def stop( self ):
    self.mutex.lock()
    self.stopMe = 1
    self.mutex.unlock()

    QThread.wait( self )
