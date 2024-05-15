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

#ifndef WALLPAPERIMAGESETTER_H
#define WALLPAPERIMAGESETTER_H

#include <qstring.h>
#include "libwallpaper_export.h"


class LIBWALLPAPER_EXPORT WallpaperImageSetter
{
public:
    explicit WallpaperImageSetter() = default;
    ~WallpaperImageSetter() = default;

    bool setImage(const QString &imageFile, int screenIndex = -1);

    QString errorString() const			{ return (mErrorString); }

private:
    QString mErrorString;
};

#endif // WALLPAPERIMAGESETTER_H
