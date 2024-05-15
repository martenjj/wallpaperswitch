//////////////////////////////////////////////////////////////////////////
//									//
//  Project:	Plasma 6 Wallpaper Switcher				//
//  Edit:	15-May-24						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Copyright (c) 2024 Jonathan Marten <jjm@keelhaul.me.uk>		//
//  Home and download page:  http://www.keelhaul.me.uk/TBD/		//
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
//  details.  If not, see <http://www.gnu.org/licenses>.		//
//									//
//////////////////////////////////////////////////////////////////////////

#undef DEBUG_CONTAINMENT

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "wallpaperimagesetter.h"

#include <qguiapplication.h>
#include <qdbusmessage.h>
#include <qdbusconnection.h>
#include <qfile.h>
#include <qcursor.h>
#ifdef DEBUG_CONTAINMENT
#include <qdebug.h>
#endif

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>


bool WallpaperImageSetter::setImage(const QString &imageFile, int screenIndex)
{
    mErrorString.clear();

    if (imageFile.isEmpty() || !QFile::exists(imageFile))
    {
        mErrorString = xi18nc("@info:shell", "The image file <filename>%1</filename> does not exist", imageFile);
        return (false);
    }

    const QList<QScreen *> screens = QGuiApplication::screens();
    if (screenIndex==-1) screenIndex = screens.indexOf(QGuiApplication::screenAt(QCursor::pos()));
    if (screenIndex<0 || screenIndex>=screens.count())
    {
        mErrorString = xi18nc("@info:shell", "Screen %1 is not present (screen count is %2)", screenIndex, screens.count());
        return (false);
    }

    const KSharedConfig::Ptr config = KSharedConfig::openConfig("plasma-org.kde.plasma.desktop-appletsrc",
                                                                KSharedConfig::SimpleConfig);
    const KConfigGroup containmentGroup1 = config->group("Containments");
#ifdef DEBUG_CONTAINMENT
    qCDebug(DEBUGCAT) << "groups in" << containmentGroup1.name() << "=" << containmentGroup1.groupList();
#endif
    for (const QString &cont : containmentGroup1.groupList())
    {
        const KConfigGroup containmentGroup2 = containmentGroup1.group(cont);
#ifdef DEBUG_CONTAINMENT
        qCDebug(DEBUGCAT) << "  groups in" << containmentGroup2.name() << "=" << containmentGroup2.groupList();
#endif
        if (!containmentGroup2.groupList().contains("Wallpaper")) continue;

        const int lastScreen = containmentGroup2.readEntry("lastScreen", -1);
        if (lastScreen==-1) continue;
#ifdef DEBUG_CONTAINMENT
        qCDebug(DEBUGCAT) << "  on screen" << lastScreen;
#endif
        if (lastScreen==screenIndex)
        {
            const QString plugin = containmentGroup2.readEntry("wallpaperplugin", "");
#ifdef DEBUG_CONTAINMENT
            qCDebug(DEBUGCAT) << "found containment" << qPrintable(cont) << "plugin" << plugin;
#endif
            if (plugin!="org.kde.image")
            {
                // Not a fatal error, but should be shown to the user as a warning
                // if nothing of higher priority overrides this message later.
                mErrorString = xi18nc("@info:shell", "The wallpaper type for screen %1 should be set to \"Image\"", screenIndex);
            }

            // Script copied and adapted from plasma-workspace/wallpapers/image/
            //    wallpaperfileitemactionplugin/wallpaperfileitemaction.cpp
            //
            // Properties declared in plasma-workspace/shell/scripting/scriptengine_v1.h
            //    Q_INVOKABLE QJSValue desktopsForActivity(const QJSValue &id = QJSValue()) const;
            // (which returns an array of Containment values)
            //    Q_INVOKABLE QJSValue desktopForScreen(const QJSValue &screen = QJSValue()) const;
            // (which returns a Containment value)
            //
            // Properties declared in plasma-workspace/shell/scripting/containment.h
            //    Q_PROPERTY(int screen READ screen)

            const QString script = QString("const allDesktops = desktopsForActivity(currentActivity());"
                                           "for (i=0; i<allDesktops.length; i++)"
                                           "{"
                                           "    d = allDesktops[i];"
                                           "    if (d.screen==%2)"
                                           "    {"
                                           "        d.currentConfigGroup = Array(\"Wallpaper\", \"org.kde.image\", \"General\");"
                                           "        d.writeConfig(\"Image\", \"%1\")"
                                           "    }"
                                           "}").arg(imageFile).arg(screenIndex);

            QDBusMessage message = QDBusMessage::createMethodCall("org.kde.plasmashell", "/PlasmaShell", "org.kde.PlasmaShell", "evaluateScript");
            message.setArguments(QVariantList() << QVariant(script));
            QDBusMessage reply = QDBusConnection::sessionBus().call(message, QDBus::BlockWithGui, 2000);
            if (reply.type()==QDBusMessage::ErrorMessage)
            {
                mErrorString = xi18nc("@info:shell", "DBus error, %1", reply.errorMessage());
                return (false);
            }

            return (true);
        }
    }

    mErrorString = xi18nc("@info:shell", "Could not find the Plasma containment for screen %1", screenIndex);
    return (false);
}
