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

#ifndef SYSTEMTRAYWIDGET_H
#define SYSTEMTRAYWIDGET_H

#include <kstatusnotifieritem.h>

#include <qwidget.h>


class QAction;


class SystemTrayWidget : public KStatusNotifierItem
{
    Q_OBJECT

public:
    explicit SystemTrayWidget(QObject *pnt = NULL);
    virtual ~SystemTrayWidget();

    void addMenuAction(QAction *act);

private:
    Q_DISABLE_COPY(SystemTrayWidget)
    void init();
};

#endif							// SYSTEMTRAYWIDGET_H
