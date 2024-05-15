/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  This file is part of Wallpaper Switcher, a virtual desktop		//
//  wallpaper (background image) switcher for KDE Plasma 6.		//
//									//
//  Copyright (c) 2016-2024 Jonathan Marten <jjm@keelhaul.me.uk>	//
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

#ifndef SWITCHERAPP_H
#define SWITCHERAPP_H

#include <qobject.h>

class KToggleAction;
class SystemTrayWidget;
class WallpaperSwitcher;


class SwitcherApp : public QObject
{
    Q_OBJECT

public:
    explicit SwitcherApp(bool onlyWindow, QObject *pnt = NULL);
    virtual ~SwitcherApp() = default;

public slots:
    void slotPreferences();

protected slots:
    void slotAboutToQuit();
    void slotSetEnableState(bool on);

private:
    Q_DISABLE_COPY(SwitcherApp)
    void init();

private:
    bool mOnlyWindow;
    bool mPrefsActive;
    KToggleAction *mEnableAction;
    SystemTrayWidget *mSystemTray;
    WallpaperSwitcher *mSwitcher;
};

#endif							// SWITCHERAPP_H
