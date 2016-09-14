/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  This file is part of Wallpaper Switcher, a virtual desktop		//
//  wallpaper (background image) switcher for KDE Plasma 5.		//
//									//
//  Copyright (c) 2016 Jonathan Marten <jjm@keelhaul.me.uk>		//
//  Home page:  http://github.com/martenjj/wallpaperswitch		//
//									//
//  This program is free software; you can redistribute it and/or	//
//  modify it under the terms of the GNU General Public License as	//
//  published by the Free Software Foundation; either version 2 of	//
//  the License, or (at your option) any later version.			//
//									//
//  It is distributed in the hope that it will be useful, but		//
//  WITHOUT ANY WARRANTY; without even the implied warranty of		//
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the	//
//  GNU General Public License for more details.			//
//									//
//  You should have received a copy of the GNU General Public		//
//  License along with this program; see the file COPYING for further	//
//  details.  If not, see http://www.gnu.org/licenses/			//
//									//
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "systemtraywidget.h"

#include <qmenu.h>
#include <qapplication.h>

#include <klocalizedstring.h>
#include <kaboutdata.h>

#include "debug.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Constructor/destructor					 	//
//									//
//////////////////////////////////////////////////////////////////////////

SystemTrayWidget::SystemTrayWidget(QObject *pnt)
    : KStatusNotifierItem(pnt)
{
    qDebug();
    init();
}


SystemTrayWidget::~SystemTrayWidget()
{
}

//////////////////////////////////////////////////////////////////////////
//									//
//  init -- The real work of initialisation, creating the GUI.	 	//
//									//
//////////////////////////////////////////////////////////////////////////

void SystemTrayWidget::init()
{
    setCategory(KStatusNotifierItem::SystemServices);
    setStatus(KStatusNotifierItem::Active);		// permanent

    const KAboutData about = KAboutData::applicationData();

    setIconByName(APPICON);
    setTitle(qApp->applicationDisplayName());
    setToolTip(APPICON, qApp->applicationDisplayName(), about.shortDescription());
}

//////////////////////////////////////////////////////////////////////////
//									//
//  addMenuAction -- Add an action to the context menu.		 	//
//									//
//////////////////////////////////////////////////////////////////////////

void SystemTrayWidget::addMenuAction(QAction *act)
{
    QMenu *menu = contextMenu();
    if (menu==NULL) return;				// should not happen?
    if (act!=NULL) menu->addAction(act);		// add action to menu
    else menu->addSeparator();				// or add a separator
}
