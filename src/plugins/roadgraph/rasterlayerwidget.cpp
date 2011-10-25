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
  mcbSpeedUnitName = new QComboBox( this );
  h->addWidget( mcbSpeedUnitName );

  mcbSpeedUnitName->insertItem( 0, tr( "km/h" ) );
  mcbSpeedUnitName->insertItem( 0, tr( "m/s" ) );

  v->addLayout( h );
  connect( mcbLayers, SIGNAL( currentIndexChanged( int ) ), 
    this, SLOT( on_mcbLayers_selectItem() ) );

  // fill list of layers
  QMap<QString, QgsMapLayer*> mapLayers = QgsMapLayerRegistry::instance()->mapLayers();
  QMap<QString, QgsMapLayer*>::iterator layer_it = mapLayers.begin();
  for ( ; layer_it != mapLayers.end(); ++layer_it )
  {
    QgsRasterLayer* rl = dynamic_cast<QgsRasterLayer*>( layer_it.value() );
    if ( !rl )
      continue;
    mcbLayers->insertItem( 0, rl->name() );
  }

  //sets current settings

  int idx = mcbLayers->findText( s->mLayer );
  if ( idx != -1 )
  {
    mcbLayers->setCurrentIndex( idx );
  }

  idx = mcbSpeedUnitName->findData( s->mSpeedUnitName );
  if ( idx != -1 )
  {
    mcbSpeedUnitName->setCurrentIndex( idx );
  }
  
  idx = mcbSpeed->findData( s->mSpeedBand );
  if ( idx != -1 )
  {
    mcbSpeed->setCurrentIndex( idx );
  }

} // RgLineVectorLayerSettingsWidget::RgLineVectorLayerSettingsWidget()

QgsRasterLayer* RgRasterLayerSettingsWidget::selectedLayer()
{
  QMap<QString, QgsMapLayer*> mapLayers = QgsMapLayerRegistry::instance()->mapLayers();
  QMap<QString, QgsMapLayer*>::iterator layer_it = mapLayers.begin();

  for ( ; layer_it != mapLayers.end(); ++layer_it )
  {
    QgsRasterLayer* rl = dynamic_cast<QgsRasterLayer*>( layer_it.value() );
    if ( !rl )
      continue;
    if ( rl->name() == mcbLayers->currentText() )
      return rl;
  }

  return NULL;
} // RgLineVectorLayerSettingsWidget::setlectedLayer()

void RgRasterLayerSettingsWidget::on_mcbLayers_selectItem()
{
  mcbSpeed->clear();
  QgsRasterLayer* rl = selectedLayer();
  if ( rl == NULL )
    return;
  QgsRasterDataProvider *provider = rl->dataProvider();
  if ( provider == NULL )
    return;

  int i = 0;
  for ( i = 0; i < provider->bandCount(); ++i )
  {
    int dataType = provider->dataType( i+1 );
    if (dataType == QgsRasterDataProvider::UnknownDataType ||
        dataType == QgsRasterDataProvider::ARGBDataType || 
        dataType == QgsRasterDataProvider::TypeCount )
    {
      continue;
    }
    mcbSpeed->insertItem( 0, tr( "band #" ) + QString("%1").arg(i), QVariant( i ) );
  }
} // RgDSettingsDlg::on_mcbLayers_selectItem()
