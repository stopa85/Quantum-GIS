/***************************************************************************
                         qgssinglesymrenderer.cpp  -  description
                             -------------------
    begin                : Oct 2003
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

#include "qgsgraduatedsymrenderer.h"

inline QgsGraduatedSymRenderer::~QgsGraduatedSymRenderer()
{
    //free the memory first
    for(std::list<QgsRangeRenderItem*>::iterator it=m_items.begin();it!=m_items.end();++it)
    {
	delete *it;
    }

    //and remove the pointers then
    m_items.clear();
}

void QgsGraduatedSymRenderer::removeItems()
{
    //free the memory first
    for(std::list<QgsRangeRenderItem*>::iterator it=m_items.begin();it!=m_items.end();++it)
    {
	delete *it;
    }

    //and remove the pointers then
    m_items.clear();
}

