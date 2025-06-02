//////////////////////////////////////////////////////////////////////////
//									//
//  Project:	Wallpaper Switcher for Plasma 6				//
//  Edit:	23-Aug-24						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Copyright (c) 2015-2024 Jonathan Marten <jjm@keelhaul.me.uk>	//
//  Home and download page:  http://www.keelhaul.demon.co.uk/TBD/	//
//									//
//  This program is free software; you can redistribute it and/or	//
//  modify it under the terms of the GNU General Public License as	//
//  published by the Free Software Foundation; either version 2 of	//
//  the License or (at your option) version 3 or any later version	//
//  accepted by the membership of KDE e.V. (or its successor approved	//
//  by the membership of KDE e.V.), which shall act as a proxy		//
//  defined in Section 14 of version 3 of the license.			//
//									//
//  It is distributed in the hope that it will be useful, but		//
//  WITHOUT ANY WARRANTY; without even the implied warranty of		//
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the	//
//  GNU General Public License for more details.			//
//									//
//  You should have received a copy of the GNU General Public		//
//  License along with this program; see the file COPYING for further	//
//  details.  If not, see <http://www.gnu.org/licenses/>		//
//									//
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "wallpaperswitcher.h"

#include <qfile.h>
#include <qguiapplication.h>
#include <qdbusconnection.h>
#include <qdbusmessage.h>
#include <qdbuspendingcall.h>

#include <kconfigskeleton.h>
#include <kx11extras.h>
#include <kmessagebox.h>
#include <klocalizedstring.h>

#include "settings.h"
#include "version.h"
#include "wallpaperimagesetter.h"
#include "libwallpaper_logging.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Debugging switches							//
//									//
//////////////////////////////////////////////////////////////////////////

#define DEBUG_CHANGE

//////////////////////////////////////////////////////////////////////////
//									//
//  Constructor/destructor					 	//
//									//
//////////////////////////////////////////////////////////////////////////

WallpaperSwitcher::WallpaperSwitcher(QObject *pnt)
    : QObject(pnt)
{
    qCDebug(DEBUGCAT);
    mFirstTime = true;
    connect(KX11Extras::self(), &KX11Extras::currentDesktopChanged, this, &WallpaperSwitcher::slotDesktopChanged);
}

//////////////////////////////////////////////////////////////////////////
//									//
//									//
//////////////////////////////////////////////////////////////////////////

void WallpaperSwitcher::slotDesktopChanged(int desktop)
{
    if (!Settings::enableSwitcher()) return;

    if (desktop==0) desktop = KX11Extras::currentDesktop();
#ifdef DEBUG_CHANGE
    qCDebug(DEBUGCAT) << "to" << desktop;
#endif // DEBUG_CHANGE

    KConfigSkeletonItem *ski = Settings::self()->wallpaperForDesktopItem();
    Q_ASSERT(ski!=nullptr);
    const KConfigGroup grp = Settings::self()->config()->group(ski->group());

    WallpaperImageSetter wis;

    const int numScreens = QGuiApplication::screens().count();
    for (int screen = 0; screen<numScreens; ++screen)
    {
        QString file = grp.readEntry(configKey(desktop, screen), "");
        if (file.isEmpty()) file = grp.readEntry(configKey(desktop), "");
#ifdef DEBUG_CHANGE
        qCDebug(DEBUGCAT) << "for screen" << screen << "image file" << file;
#endif
        if (file.isEmpty())
        {
            qCDebug(DEBUGCAT) << "No wallpaper configured for desktop" << desktop << "screen" << screen;
            continue;
        }

        const bool status = wis.setImage(file, screen);
        const QString msg = wis.errorString();
        if (!msg.isEmpty())
        {
            if (status) KMessageBox::information(nullptr, msg, i18n("Wallpaper Image Message"), "settingInfo");
            else KMessageBox::error(nullptr, msg, i18n("Wallpaper Image Error"));
        }
    }

    if (Settings::showPopupMessage())			// show popup on desktop change
    {
        if (!mFirstTime)				// but not the first time on startup
        {
            QDBusMessage msg = QDBusMessage::createMethodCall("org.kde.plasmashell",
                                                              "/org/kde/osdService",
                                                              "org.kde.osdService",
                                                              "virtualDesktopChanged");
            msg.setArguments(QList<QVariant>() << i18n("Desktop %1 \"%2\"", desktop, KX11Extras::desktopName(desktop)));
            QDBusConnection::sessionBus().asyncCall(msg);
        }
        else mFirstTime = false;			// show popup from now on
    }
}


/* static */ QString WallpaperSwitcher::configKey(const QString &desktop, const QString &screen)
{
    return (QString("Desktop%1_Screen%2").arg(desktop).arg(screen));
}


/* static */ QString WallpaperSwitcher::configKey(int desktop, int screen)
{
    if (screen==-1) return (QString::number(desktop));	// legacy
    return (configKey(QString::number(desktop), QString::number(screen)));
}


/* static */ QString WallpaperSwitcher::versionString()
{
    QString res = VERSION;
#if VCS_HAVE_VERSION
    res += QString(" %1 %2").arg(VCS_TYPE_STRING).arg(VCS_REVISION_STRING);
#endif
    return (res);


}
