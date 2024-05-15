/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	StartMenu for Plasma 5					//
//  Edit:	11-Mar-24						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Copyright (c) 2015 Jonathan Marten <jjm@keelhaul.me.uk>		//
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

#ifndef WALLPAPERSWITCHER_H
#define WALLPAPERSWITCHER_H

#include <qobject.h>


class WallpaperSwitcher : public QObject
{
    Q_OBJECT

public:
    explicit WallpaperSwitcher(QObject *pnt = nullptr);
    virtual ~WallpaperSwitcher() = default;

    static QString configKey(int desktop, int screen = -1);
    static QString configKey(const QString &desktop, const QString &screen);

public slots:
    void slotDesktopChanged(int desktop = 0);
};

#endif							// WALLPAPERSWITCHER_H
