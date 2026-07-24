//////////////////////////////////////////////////////////////////////////
//									//
//  Project:	Plasma 6 Wallpaper Switcher				//
//  Edit:	24-Jul-26						//
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
#include <qstringlist.h>
#include "libwallpaper_export.h"


class LIBWALLPAPER_EXPORT WallpaperImageSetter
{
public:
    // The sort of wallpaper file that has been selected.  A still image
    // is set using the standard Plasma "org.kde.image" wallpaper plugin,
    // a video needs the "Smart Video Wallpaper Reborn" plugin (or another
    // plugin compatible with its configuration) to be installed.
    enum MediaType
    {
        Image,
        Video
    };

    explicit WallpaperImageSetter() = default;
    ~WallpaperImageSetter() = default;

    bool setImage(const QString &imageFile, int screenIndex = -1);

    QString errorString() const			{ return (mErrorString); }

    // Classify a wallpaper file by its MIME type, falling back to the
    // file name suffix if the MIME type cannot be determined.
    static MediaType mediaType(const QString &file);
    static bool isVideoFile(const QString &file)	{ return (mediaType(file)==WallpaperImageSetter::Video); }

    // The MIME types of the video files that can be used as a wallpaper.
    static QStringList videoMimeTypes();

    // The wallpaper plugin provided with this application, which shows
    // both images and videos and fades between them.  Because it never
    // has to be switched, changing the wallpaper does not blank the
    // desktop as switching between two plugins does.
    static QString mediaPluginId();
    static bool mediaPluginAvailable();

    // The Plasma wallpaper plugin ID used for videos if the plugin above
    // is not being used, and whether it appears to be installed.
    static QString videoPluginId();
    static bool videoPluginAvailable();

    // The wallpaper plugin to use for the specified sort of wallpaper
    // file, or a null string if there is none available.
    static QString wallpaperPluginFor(MediaType type);

    // Whether a video can be used as a wallpaper at all, that is,
    // whether there is a plugin available which can show one.
    static bool videosSupported();

private:
    bool runPlasmaScript(const QString &script);

private:
    QString mErrorString;
};

#endif // WALLPAPERIMAGESETTER_H
