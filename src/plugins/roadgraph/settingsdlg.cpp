/***************************************************************************
 *   Copyright (C) 2010 by Sergey Yakushev                                 *
 *   yakushevs <at> list.ru                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
//road-graph plugin includes
#include "settingsdlg.h"
#include <qgscontexthelp.h>

//qt includes
#include <qlabel.h>
#include <qcombobox.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <qdialogbuttonbox.h>
#include <qmessagebox.h>
#include <QDoubleSpinBox>

// Qgis includes
#include "settings.h"
#include "linevectorlayersettings.h"
#include "rasterlayersettings.h"

//standard includes
#include <iostream>

RgSettingsDlg::RgSettingsDlg( RgSettings *settings, QWidget* parent, Qt::WFlags fl )
    : QDialog( parent, fl )
    , mSettings( settings )
{
  // create base widgets;
  setWindowTitle( tr( "Road graph plugin settings" ) );
  QVBoxLayout *v = new QVBoxLayout( this );

  QHBoxLayout *h = new QHBoxLayout();
  QLabel *l = new QLabel( tr( "Time unit" ), this );
  h->addWidget( l );
  mcbPluginsTimeUnit = new QComboBox( this );
  h->addWidget( mcbPluginsTimeUnit );
  v->addLayout( h );

  h = new QHBoxLayout();
  l = new QLabel( tr( "Distance unit" ), this );
  h->addWidget( l );
  mcbPluginsDistanceUnit = new QComboBox( this );
  h->addWidget( mcbPluginsDistanceUnit );
  v->addLayout( h );

  h = new QHBoxLayout();
  l = new QLabel( tr( "Topology tolerance" ), this );
  h->addWidget( l );
  msbTopologyTolerance = new QDoubleSpinBox( this );
  msbTopologyTolerance->setMinimum( 0.0 );
  msbTopologyTolerance->setDecimals( 5 );
  h->addWidget( msbTopologyTolerance );
  v->addLayout( h );

  h = new QHBoxLayout();
  l = new QLabel( tr( "Graph director type" ), this );
  h->addWidget( l );
  mcbGraphDirector = new QComboBox( this );
  h->addWidget( mcbGraphDirector );
  v->addLayout( h );

  mSettingsLayout = new QHBoxLayout();
  mSettingsWidget = mSettings->getGui( this );
  mSettingsLayout->addWidget( mSettingsWidget );
  v->addLayout( mSettingsLayout );

  QDialogButtonBox *bb = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help, Qt::Horizontal, this );
  connect( bb, SIGNAL( accepted() ), this, SLOT( on_buttonBox_accepted() ) );
  connect( bb, SIGNAL( rejected() ), this, SLOT( on_buttonBox_rejected() ) );
  connect( bb, SIGNAL( helpRequested() ), this, SLOT( on_buttonBox_helpRequested() ) );
  v->addWidget( bb );

  mcbPluginsTimeUnit->addItem( tr( "second" ), QVariant( "s" ) );
  mcbPluginsTimeUnit->addItem( tr( "hour" ), QVariant( "h" ) );
  mcbPluginsDistanceUnit->addItem( tr( "meter" ), QVariant( "m" ) );
  mcbPluginsDistanceUnit->addItem( tr( "kilometer" ), QVariant( "km" ) );

  mcbGraphDirector->addItem( tr( "Line vector layer director" ), QVariant( "line vector layer" ) );
  mcbGraphDirector->addItem( tr( "Raster layer director" ), QVariant( "raster layer" ) );
  connect( mcbGraphDirector, SIGNAL( currentIndexChanged(int) ), this, SLOT(on_mcbGraphDirector_selectItem()) );
  
  // fill NULL value
  mSettingsMap[ "line vector layer" ].first = NULL;
  mSettingsMap[ "line vector layer" ].second = NULL;
  mSettingsMap[ "raster layer" ].first = NULL;
  mSettingsMap[ "raster layer" ].second = NULL;
  // fill real value
  mSettingsMap[ mSettings->name() ].first = mSettings;
  mSettingsMap[ mSettings->name() ].second = mSettingsWidget;

  int idx = mcbGraphDirector->findData( QVariant( mSettings->name() ) );
  if ( idx > -1 )
    mcbGraphDirector->setCurrentIndex( idx );
} // RgSettingsDlg::RgSettingsDlg()

RgSettingsDlg::~RgSettingsDlg()
{
  QMap< QString, QPair< RgSettings*, QWidget* > >::iterator it;
  for ( it = mSettingsMap.begin(); it != mSettingsMap.end(); ++it )
  {
    if ( it.value().first != mSettings && it.value().first != NULL )
      delete it.value().first;
    if ( it.value().second != NULL )
      delete it.value().second;
  }
}

void RgSettingsDlg::on_mcbGraphDirector_selectItem()
{
  QString directorName = mcbGraphDirector->itemData( mcbGraphDirector->currentIndex() ).toString();
  QMap< QString, QPair< RgSettings*, QWidget* > >::iterator it = mSettingsMap.find( directorName );
  if ( it.value().first == NULL )
  {
    if ( directorName == "line vector layer" )
    {
      it.value().first = new RgLineVectorLayerSettings();
    }else if ( directorName == "raster layer" ) 
    {
      it.value().first = new RgRasterLayerSettings();
    }
    it.value().second = it.value().first->getGui( this );
  }
  if ( mSettingsWidget != NULL )
    mSettingsWidget->hide();
  
  mSettings = it.value().first;
  mSettingsWidget = it.value().second;
  mSettingsLayout->addWidget( mSettingsWidget );
  mSettingsWidget->show();
}

void RgSettingsDlg::on_buttonBox_accepted()
{
  mSettings->setFromGui( mSettingsWidget );
  accept();
}

void RgSettingsDlg::on_buttonBox_rejected()
{
  reject();
}

void RgSettingsDlg::on_buttonBox_helpRequested()
{
  QgsContextHelp::run( metaObject()->className() );
}

QString RgSettingsDlg::timeUnitName()
{
  return mcbPluginsTimeUnit->itemData( mcbPluginsTimeUnit->currentIndex() ).toString();
}

void RgSettingsDlg::setTimeUnitName( const QString& name )
{
  int i = mcbPluginsTimeUnit->findData( QVariant( name ) );
  if ( i != -1 )
  {
    mcbPluginsTimeUnit->setCurrentIndex( i );
  }
}

QString RgSettingsDlg::distanceUnitName()
{
  return mcbPluginsDistanceUnit->itemData( mcbPluginsDistanceUnit->currentIndex() ).toString();
}

void RgSettingsDlg::setDistanceUnitName( const QString& name )
{
  int i = mcbPluginsDistanceUnit->findData( QVariant( name ) );
  if ( i != -1 )
  {
    mcbPluginsDistanceUnit->setCurrentIndex( i );
  }
}

void RgSettingsDlg::setTopologyTolerance( double f )
{
  msbTopologyTolerance->setValue( f );
}

double RgSettingsDlg::topologyTolerance()
{
  return msbTopologyTolerance->value();
}

RgSettings* RgSettingsDlg::settings()
{
  return mSettings;
}
