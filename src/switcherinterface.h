/////////////////////////////////////////////////// -*- mode:c++; -*- ////
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

#ifndef SWITCHERINTERFACE_H
#define SWITCHERINTERFACE_H

#include <qobject.h>
#include "libwallpaper_export.h"


struct DesktopData
{
    QString uid;
    QString name;
};


class QDBusInterface;


class LIBWALLPAPER_EXPORT SwitcherInterface : public QObject
{
    Q_OBJECT

public:
    static SwitcherInterface *self();
    void init();

    unsigned int numberOfDesktops() const;
    int currentDesktop();
    QString desktopName(int desktop);

signals:
    void desktopChanged(int desktop);

protected slots:
    void slotDesktopConfigChanged();
#ifndef X11_ONLY
    void slotCurrentDesktopChanged(const QString &uid);
#endif
    void slotCurrentDesktopChanged(int desktop);

private:
    explicit SwitcherInterface(QObject *pnt = nullptr);
    virtual ~SwitcherInterface() = default;
#ifndef X11_ONLY
    int findByUID(const QString &uid) const;
#endif

private:
    QList<DesktopData> mDesktops;
    int mCurrent;
    QDBusInterface *mKWinInterface;
};

#endif							// SWITCHERINTERFACE_H
