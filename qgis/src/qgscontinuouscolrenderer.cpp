/***************************************************************************
                         qgscontinuouscolrenderer.cpp  -  description
                             -------------------
    begin                : Nov 2003
    copyright            : (C) 2003 by Marco Hugentobler
    email                : mhugent@geo.unizh.ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgscontinuouscolrenderer.h"

QgsContinuousColRenderer::~QgsContinuousColRenderer()
{
    if(m_minimumItem)
    {
	delete m_minimumItem;
    }
    if(m_maximumItem)
    {
	delete m_maximumItem;
    }
}

void QgsContinuousColRenderer::setMinimumItem(QgsRenderItem* it)
{
    if(m_minimumItem)
    {
	delete m_minimumItem;
    }
    m_minimumItem=it;
}

void QgsContinuousColRenderer::setMaximumItem(QgsRenderItem* it)
{
    if(m_maximumItem)
    {
	delete m_maximumItem;
    }
    m_maximumItem=it;
}
