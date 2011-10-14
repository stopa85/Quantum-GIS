/***************************************************************************
 *   Copyright (C) 2010 by Sergey Yakushev                                 *
 *   yakushevs@list.ru                                                     *
 *                                                                         *
 *   This is a plugin generated from the QGIS plugin template              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "rasterlayerwidget.h"
#include "rasterlayersettings.h"

//qt includes
#include <qgscontexthelp.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <qdialogbuttonbox.h>
#include <qtabwidget.h>
#include <qspinbox.h>
#include <qmessagebox.h>
#include <qlineedit.h>

// Qgis includes
#include "qgsmaplayerregistry.h"
#include "qgsrasterlayer.h"

//standard includes


RgRasterLayerSettingsWidget::RgRasterLayerSettingsWidget( RgRasterLayerSettings *s, QWidget* parent )
    : QWidget( parent )
{
  // create base widgets;
  QTabWidget *tab = new QTabWidget( this );
  QVBoxLayout *v = new QVBoxLayout( this );
  v->addWidget( tab );

  // transportation layer
  QFrame *frame = new QFrame( this );
  tab->addTab( frame, tr( "Transportation layer" ) );
  v = new QVBoxLayout( frame );
  QLabel *l = new QLabel( tr( "Layer" ), frame );
  mcbLayers = new QComboBox( frame );
  QHBoxLayout *h = new QHBoxLayout( this );

  h->addWidget( l );
  h->addWidget( mcbLayers );
  v->addLayout( h );

  h = new QHBoxLayout();
  l = new QLabel( tr( "Speed field" ), frame );
  mcbSpeed = new QComboBox( frame );
  h->addWidget( l );
  h->addWidget( mcbSpeed );
  mcbUnitOfSpeed = new QComboBox( this );
  h->addWidget( mcbUnitOfSpeed );

  mcbUnitOfSpeed->insertItem( 0, tr( "km/h" ) );
  mcbUnitOfSpeed->insertItem( 0, tr( "m/s" ) );

  v->addLayout( h );

  // fill list of layers
  QMap<QString, QgsMapLayer*> mapLayers = QgsMapLayerRegistry::instance()->mapLayers();
  QMap<QString, QgsMapLayer*>::iterator layer_it = mapLayers.begin();

  for ( ; layer_it != mapLayers.end(); ++layer_it )
  {
    QgsRasterLayer* vl = dynamic_cast<QgsRasterLayer*>( layer_it.value() );
    if ( !vl )
      continue;
    mcbLayers->insertItem( 0, vl->name() );
  }

  //sets current settings

  int idx = mcbLayers->findText( s->mLayer );
  if ( idx != -1 )
  {
    mcbLayers->setCurrentIndex( idx );
  }

} // RgLineVectorLayerSettingsWidget::RgLineVectorLayerSettingsWidget()

QgsRasterLayer* RgRasterLayerSettingsWidget::selectedLayer()
{
  QMap<QString, QgsMapLayer*> mapLayers = QgsMapLayerRegistry::instance()->mapLayers();
  QMap<QString, QgsMapLayer*>::iterator layer_it = mapLayers.begin();

  for ( ; layer_it != mapLayers.end(); ++layer_it )
  {
    QgsRasterLayer* vl = dynamic_cast<QgsRasterLayer*>( layer_it.value() );
    if ( !vl )
      continue;
    if ( vl->name() == mcbLayers->currentText() )
      return vl;
  }

  return NULL;
} // RgLineVectorLayerSettingsWidget::setlectedLayer()

void RgRasterLayerSettingsWidget::on_mcbLayers_selectItem()
{

} // RgDSettingsDlg::on_mcbLayers_selectItem()
