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

#include <kx11extras.h>

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

    connect(KX11Extras::self(), &KX11Extras::currentDesktopChanged, this, &SwitcherInterface::slotCurrentDesktopChanged);
    connect(KX11Extras::self(), &KX11Extras::numberOfDesktopsChanged, this, &SwitcherInterface::slotDesktopConfigChanged);
    connect(KX11Extras::self(), &KX11Extras::desktopNamesChanged, this, &SwitcherInterface::slotDesktopConfigChanged);

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


int SwitcherInterface::currentDesktop()
{
    if (mCurrent==0)					// current desktop not known yet
    {
        mCurrent = KX11Extras::currentDesktop();	// with X11, this is a 1-origin index
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
    const int numDesktops = KX11Extras::numberOfDesktops();
    for (int i = 1; i<=numDesktops; ++i)
    {
        DesktopData data;
        data.uid = QString::number(i);
        data.name = KX11Extras::desktopName(i);
        mDesktops.append(data);
        qCDebug(DEBUGCAT) << "  desktop" << i << "=" << data.name;
    }

    qCDebug(DEBUGCAT) << "total" << mDesktops.count() << "desktops";
}


void SwitcherInterface::slotCurrentDesktopChanged(int desktop)
{
    qCDebug(DEBUGCAT) << "changed to desktop number" << desktop;
    mCurrent = desktop;
    emit desktopChanged(desktop);
}
