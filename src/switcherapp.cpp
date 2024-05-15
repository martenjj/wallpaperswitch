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


//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "switcherapp.h"

#include <qcoreapplication.h>

#include <klocalizedstring.h>
#include <kstandardaction.h>
#include <ktoggleaction.h>

#include "systemtraywidget.h"
#include "wallpaperswitcher.h"
#include "preferencesdialogue.h"
#include "appsettings.h"
#include "wallpapersettings.h"
#include "libwallpaper_logging.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Constructor/destructor					 	//
//									//
//////////////////////////////////////////////////////////////////////////

SwitcherApp::SwitcherApp(bool onlyWindow, QObject *pnt)
    : QObject(pnt)
{
    qCDebug(DEBUGCAT);

    mOnlyWindow = onlyWindow;
    init();
}

//////////////////////////////////////////////////////////////////////////
//									//
//  init -- The real work of initialisation, creating the GUI.	 	//
//									//
//////////////////////////////////////////////////////////////////////////

void SwitcherApp::init()
{
    mSystemTray = nullptr;
    mSwitcher = new WallpaperSwitcher(this);
    mPrefsActive = false;

    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, &SwitcherApp::slotAboutToQuit);

    if (mOnlyWindow)
    {
        PreferencesDialogue *d = new PreferencesDialogue(true);
        d->open();
    }
    else
    {
        mSystemTray = new SystemTrayWidget(this);

        mEnableAction = new KToggleAction(i18nc("@action:inmenu", "Enable Switching"), this);
        mEnableAction->setChecked(WallpaperSettings::enableSwitcher());
        connect(mEnableAction, &QAction::triggered, this, &SwitcherApp::slotSetEnableState);
        mSystemTray->addMenuAction(mEnableAction);

        QAction *act = KStandardAction::preferences(this, &SwitcherApp::slotPreferences, this);
        mSystemTray->addMenuAction(act);		// add "Preferences"

        if (AppSettings::firstStartup())
        {
            AppSettings::setFirstStartup(false);
            AppSettings::self()->config()->sync();
            slotPreferences();
        }
    }
}

//////////////////////////////////////////////////////////////////////////
//									//
//  slotAboutToQuit -- Ensure window state and geometry is saved.	//
//									//
//////////////////////////////////////////////////////////////////////////

void SwitcherApp::slotAboutToQuit()
{
    AppSettings::self()->config()->sync();
    WallpaperSettings::self()->config()->sync();
}

//////////////////////////////////////////////////////////////////////////
//									//
//  slotPreferences -- Display the "Preferences" dialogue.	 	//
//									//
//////////////////////////////////////////////////////////////////////////

void SwitcherApp::slotPreferences()
{
    if (mPrefsActive) return;				// avoid double invocation

    mPrefsActive = true;
    PreferencesDialogue d(false);
    if (d.exec()) mEnableAction->setChecked(WallpaperSettings::enableSwitcher());
    mPrefsActive = false;
}


void SwitcherApp::slotSetEnableState(bool on)
{
    qCDebug(DEBUGCAT) << on;
    WallpaperSettings::setEnableSwitcher(on);
}
