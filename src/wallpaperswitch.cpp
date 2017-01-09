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

#include "wallpaperswitch.h"

#include <klocalizedstring.h>
#include <kstandardaction.h>
#include <ktoggleaction.h>

#include "debug.h"
#include "systemtraywidget.h"
#include "switcher.h"
#include "preferencesdialogue.h"
#include "settings.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Constructor/destructor					 	//
//									//
//////////////////////////////////////////////////////////////////////////

WallpaperSwitch::WallpaperSwitch(bool onlyWindow, QObject *pnt)
    : QObject(pnt)
{
    qDebug();

    mOnlyWindow = onlyWindow;
    init();
}


WallpaperSwitch::~WallpaperSwitch()
{
}

//////////////////////////////////////////////////////////////////////////
//									//
//  init -- The real work of initialisation, creating the GUI.	 	//
//									//
//////////////////////////////////////////////////////////////////////////

void WallpaperSwitch::init()
{
    qDebug();

    mSystemTray = NULL;
    mSwitcher = new Switcher(this);
    mPrefsActive = false;

    connect(qApp, SIGNAL(aboutToQuit()), SLOT(slotAboutToQuit()));

    if (mOnlyWindow)
    {
        PreferencesDialogue *d = new PreferencesDialogue(true);
        d->setWallpaperPath(mSwitcher->wallpaperPath());
        d->open();
    }
    else
    {
        mSystemTray = new SystemTrayWidget(this);

        mEnableAction = new KToggleAction(i18nc("@action:inmenu", "Enable Switching"), this);
        mEnableAction->setChecked(Settings::enableSwitcher());
        connect(mEnableAction, &QAction::triggered, this, &WallpaperSwitch::slotSetEnableState);
        mSystemTray->addMenuAction(mEnableAction);

        QAction *act = KStandardAction::preferences(this, SLOT(slotPreferences()), this);
        mSystemTray->addMenuAction(act);		// add "Preferences"

        if (Settings::firstStartup())
        {
            Settings::setFirstStartup(false);
            Settings::self()->config()->sync();
            slotPreferences();
        }
    }
}

//////////////////////////////////////////////////////////////////////////
//									//
//  slotAboutToQuit -- Ensure window state and geometry is saved.	//
//									//
//////////////////////////////////////////////////////////////////////////

void WallpaperSwitch::slotAboutToQuit()
{
    Settings::self()->config()->sync();			// ensure settings saved
}

//////////////////////////////////////////////////////////////////////////
//									//
//  slotPreferences -- Display the "Preferences" dialogue.	 	//
//									//
//////////////////////////////////////////////////////////////////////////

void WallpaperSwitch::slotPreferences()
{
    if (mPrefsActive) return;				// avoid double invocation

    mPrefsActive = true;
    PreferencesDialogue d(false);
    d.setWallpaperPath(mSwitcher->wallpaperPath());
    if (d.exec()) mEnableAction->setChecked(Settings::enableSwitcher());
    mPrefsActive = false;
}


void WallpaperSwitch::slotSetEnableState(bool on)
{
    qDebug() << on;
    Settings::setEnableSwitcher(on);
}
