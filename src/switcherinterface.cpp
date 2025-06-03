//////////////////////////////////////////////////////////////////////////
//									//
//  Project:	Wallpaper Switcher for Plasma 6				//
//  Edit:	02-Jun-25						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Copyright (c) 2015-2025 Jonathan Marten <jjm@keelhaul.me.uk>	//
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

#include "switcherinterface.h"

#ifndef X11_ONLY
#include <qdbusinterface.h>
#include <qdbusconnection.h>
#include <qdbusreply.h>
#endif

#ifdef X11_ONLY
#include <kx11extras.h>
#endif

#include "libwallpaper_logging.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Constructor/instance					 	//
//									//
//////////////////////////////////////////////////////////////////////////

SwitcherInterface::SwitcherInterface(QObject *pnt)
    : QObject(pnt)
{
}


/* static */ SwitcherInterface *SwitcherInterface::self()
{
    static SwitcherInterface *interface = new SwitcherInterface;
    return (interface);
}


void SwitcherInterface::init()
{
    mCurrent = 0;					// not known yet
#ifdef X11_ONLY
    qCDebug(DEBUGCAT) << "Using X11 interface";
    connect(KX11Extras::self(), &KX11Extras::currentDesktopChanged, this, &SwitcherInterface::slotCurrentDesktopChanged);
    connect(KX11Extras::self(), &KX11Extras::numberOfDesktopsChanged, this, &SwitcherInterface::slotDesktopConfigChanged);
    connect(KX11Extras::self(), &KX11Extras::desktopNamesChanged, this, &SwitcherInterface::slotDesktopConfigChanged);
#else
    qCDebug(DEBUGCAT) << "Using KWin DBus interface";

    mKWinInterface = new QDBusInterface("org.kde.KWin", "/VirtualDesktopManager",
                                         "org.kde.KWin.VirtualDesktopManager",
                                         QDBusConnection::sessionBus());
    if (!mKWinInterface->isValid())
    {
        QDBusError err(mKWinInterface->lastError());
        qCWarning(DEBUGCAT) << "DBus error connecting to KWin," << err.name() << err.message();
        return;
    }

    connect(mKWinInterface, SIGNAL(currentChanged(const QString &)), this, SLOT(slotCurrentDesktopChanged(const QString &)));
    connect(mKWinInterface, SIGNAL(countChanged(uint)), this, SLOT(slotDesktopConfigChanged()));
    connect(mKWinInterface, SIGNAL(desktopDataChanged()), this, SLOT(slotDesktopConfigChanged()));
#endif

    slotDesktopConfigChanged();				// build the initial desktop list
}

//////////////////////////////////////////////////////////////////////////
//									//
//									//
//////////////////////////////////////////////////////////////////////////

unsigned int SwitcherInterface::numberOfDesktops() const
{
    return (mDesktops.count());
}


#ifndef X11_ONLY
/* private */ int SwitcherInterface::findByUID(const QString &uid) const
{
    for (int i = 1; i<=mDesktops.count(); ++i)		// compatibile with X11 numbering
    {
        const DesktopData &data = mDesktops.at(i-1);
        if (data.uid==uid) return (i);
    }

    return (0);
}
#endif


int SwitcherInterface::currentDesktop()
{
    if (mCurrent==0)					// current desktop not known yet
    {
#ifdef X11_ONLY
        mCurrent = KX11Extras::currentDesktop();	// with X11, this is a 1-origin index
#else
        if (!mKWinInterface->isValid())
        {
            QDBusError err(mKWinInterface->lastError());
            qCWarning(DEBUGCAT) << "DBus error getting current desktop from KWin," << err.name() << err.message();
        }
        else
        {
            // Unlike the "desktops" property which needs to be retrieved
            // in slotDesktopConfigChanged() below, the "current" property
            // is a simple QString type and therefore can be retrieved as
            // a QObject property directly.
            const QString uid = mKWinInterface->property("current").toString();
            qCDebug(DEBUGCAT) << "current desktop UID" << uid;
            mCurrent = findByUID(uid);
        }
#endif
        qCDebug(DEBUGCAT) << "current desktop number" << mCurrent;
    }

    return (mCurrent);
}


QString SwitcherInterface::desktopName(int desktop)
{
    return (mDesktops.value(desktop-1).name);
}


void SwitcherInterface::slotDesktopConfigChanged()
{
    qCDebug(DEBUGCAT) << "getting list of desktops";
    mDesktops.clear();
#ifdef X11_ONLY
    const int numDesktops = KX11Extras::numberOfDesktops();
    for (int i = 1; i<=numDesktops; ++i)
    {
        DesktopData data;
        data.uid = QString::number(i);
        data.name = KX11Extras::desktopName(i);
        mDesktops.append(data);
        qCDebug(DEBUGCAT) << "  desktop" << i << "=" << data.name;
    }
#else

    // The "desktops" information is a property of the KWin DBus interface,
    // not a callable method.  Qt would normally deserialise all of the
    // interface properties and automatically set them as QObject properties
    // of the QDBusInterface, but in order to be able to do that with this
    // property its type and structure would need to be registered with the
    // Qt meta object and DBus system.  To avoid such complication, instead
    // we use DBus introspection to get the property value (which is a
    // method call) and then deserialise it explicitly.
    QDBusInterface iface("org.kde.KWin", "/VirtualDesktopManager",
                         "org.freedesktop.DBus.Properties",
                         QDBusConnection::sessionBus());
    if (!iface.isValid())
    {
        QDBusError err(iface.lastError());
        qCWarning(DEBUGCAT) << "DBus error getting desktop list from KWin," << err.name() << err.message();
        return;
    }

    QDBusReply<QVariant> reply = iface.call("Get", "", "desktops");
    const QDBusArgument arg = qvariant_cast<QDBusArgument>(reply.value());

    // Not documented, but see for example
    // https://code.qt.io/cgit/qt/qttools.git/tree/src/qdbus/qdbus/qdbus.cpp#n74
    const QString sig = arg.currentSignature();
    if (sig!="a(uss)")
    {
        qCWarning(DEBUGCAT) << "unexpected DBus argument signature" << sig;
        return;
    }

    arg.beginArray();
    while (!arg.atEnd())
    {
        arg.beginStructure();

        int num;
        DesktopData data;
        arg >> num >> data.uid >> data.name;
        ++num;						// compatible with X11 numbering

        arg.endStructure();
        qCDebug(DEBUGCAT) << "  desktop" << num << "=" << data.name;
        mDesktops.append(data);
     }
     arg.endArray();
#endif

    qCDebug(DEBUGCAT) << "total" << mDesktops.count() << "desktops";
}


#ifndef X11_ONLY
void SwitcherInterface::slotCurrentDesktopChanged(const QString &uid)
{
    qCDebug(DEBUGCAT) << "changed to desktop UID" << uid;
    slotCurrentDesktopChanged(findByUID(uid));
}
#endif


void SwitcherInterface::slotCurrentDesktopChanged(int desktop)
{
    qCDebug(DEBUGCAT) << "changed to desktop number" << desktop;
    mCurrent = desktop;
    emit desktopChanged(desktop);
}
