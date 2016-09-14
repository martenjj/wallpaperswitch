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

#include <qapplication.h>
#include <qcommandlineparser.h>
#include <qicon.h>

#include <kaboutdata.h>
#include <klocalizedstring.h>

#include "wallpaperswitch.h"
#include "debug.h"
#include "version.h"

Q_LOGGING_CATEGORY(DEBUGCAT, "wallpaperswitch")

//////////////////////////////////////////////////////////////////////////
//									//
//  Main							 	//
//									//
//////////////////////////////////////////////////////////////////////////
 
int main(int argc,char *argv[])
{
    KAboutData aboutData("wallpaperswitch",		// componentName
                         i18n("Wallpaper Switcher"),	// displayName
#ifdef VCS_HAVE_VERSION
                         ( VERSION " (" VCS_TYPE_STRING " " VCS_REVISION_STRING ")" ),
#else
                         VERSION,			// version
#endif
                         i18n("Switch wallpaper when virtual desktop changes"),
                         KAboutLicense::GPL,
                         i18n("Copyright (c) 2016 Jonathan Marten"),
                         QString::null,			// text
                         "http://www.keelhaul.me.uk",	// homePageAddress
                         "jjm@keelhaul.me.uk");		// bugsEmailAddress
    aboutData.addAuthor(i18n("Jonathan Marten"),
                         QString::null,
                        "jjm@keelhaul.me.uk",
                        "http://www.keelhaul.me.uk");

    KLocalizedString::setApplicationDomain(qPrintable(aboutData.componentName()));

    QApplication app(argc, argv);
    KAboutData::setApplicationData(aboutData);
    app.setWindowIcon(QIcon::fromTheme(APPICON));

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.setApplicationDescription(aboutData.shortDescription());
    parser.addHelpOption();
    parser.addVersionOption();

    // command and arguments
    parser.addOption(QCommandLineOption("w", i18n("Run in a normal window, without a system tray icon")));

    parser.process(app);
    aboutData.processCommandLine(&parser);

    new WallpaperSwitch(parser.isSet("w"));			// will show itself
    return (app.exec());
}
