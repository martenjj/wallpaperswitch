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

#include "preferencesdialogue.h"

#include <qpushbutton.h>

#include <klocalizedstring.h>
#include <kaboutapplicationdialog.h>
#include <kaboutdata.h>
#include <kstandardguiitem.h>

#include "settings.h"
#include "preferencespage.h"
#include "libwallpaper_logging.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  PreferencesDialogue -- The container dialogue			//
//									//
//////////////////////////////////////////////////////////////////////////

PreferencesDialogue::PreferencesDialogue(bool soloMode, QWidget *pnt)
    : KPageDialog(pnt)
{
    setObjectName("PreferencesDialogue");
    setFaceType(KPageDialog::Auto);

    if (soloMode)
    {
        setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Close|QDialogButtonBox::Help);
        KStandardGuiItem::assign(button(QDialogButtonBox::Close), KStandardGuiItem::Quit);
        connect(button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &PreferencesDialogue::saveSettings);
    }
    else
    {
        setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel|QDialogButtonBox::Help);
        connect(button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &PreferencesDialogue::saveSettings);
        setWindowTitle(i18n("Settings"));
    }

    button(QDialogButtonBox::Help)->setText(i18nc("@action:button", "About..."));
    connect(button(QDialogButtonBox::Help), &QPushButton::clicked, this, &PreferencesDialogue::slotAbout);

    mWallpaperPage = new PreferencesWallpaperPage(this);
    KPageWidgetItem *page = addPage(mWallpaperPage, i18n("Wallpaper"));
    page->setIcon(QIcon::fromTheme("user-desktop"));

    setMinimumSize(500, 240);

    loadSettings();
    QString savedSize = Settings::preferencesDialogueSize();
    if (!savedSize.isEmpty()) restoreGeometry(QByteArray::fromBase64(savedSize.toLatin1()));
    int savedIndex = Settings::preferencesPageIndex();
    if (savedIndex>=0)
    {
        KPageView *view = qobject_cast<KPageView *>(pageWidget());
        Q_ASSERT(view!=nullptr);
        view->setCurrentPage(view->model()->index(savedIndex, 0));
    }
}


PreferencesDialogue::~PreferencesDialogue()
{
    Settings::setPreferencesDialogueSize(QString::fromLocal8Bit(saveGeometry().toBase64()));
    KPageView *view = qobject_cast<KPageView *>(pageWidget());
    Q_ASSERT(view!=nullptr);
    Settings::setPreferencesPageIndex(view->currentPage().row());
    Settings::self()->save();
}


void PreferencesDialogue::slotAbout()
{
    KAboutApplicationDialog d(KAboutData::applicationData(), this);
    d.exec();
}

//////////////////////////////////////////////////////////////////////////
//									//
//  loadSettings/saveSettings -- Load or save the dialogue settings	//
//  from the application configuration.					//
//									//
//////////////////////////////////////////////////////////////////////////

void PreferencesDialogue::loadSettings()
{
    qCDebug(DEBUGCAT);
    mWallpaperPage->loadSettings();
}


void PreferencesDialogue::saveSettings()
{
    qCDebug(DEBUGCAT);
    mWallpaperPage->saveSettings();
    Settings::self()->save();
}
