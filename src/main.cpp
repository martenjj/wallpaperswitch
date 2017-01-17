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
#include <kdbusservice.h>
#include <kcrash.h>

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
                         i18n("Copyright (c) 2016,2017 Jonathan Marten"),
                         QString::null,			// text
                         "http://www.github.com/martenjj/wallpaperswitch",
							// homePageAddress
                         "http://www.github.com/martenjj/wallpaperswitch/issues");
							// bugsEmailAddress
    aboutData.addAuthor(i18n("Jonathan Marten"),
                         QString::null,
                        "jjm@keelhaul.me.uk",
                        "http://www.keelhaul.me.uk");

    KLocalizedString::setApplicationDomain(qPrintable(aboutData.componentName()));
    KCrash::setCrashHandler();				// enable standard DrKonqi

    QApplication app(argc, argv);
    KAboutData::setApplicationData(aboutData);
    app.setWindowIcon(QIcon::fromTheme(APPICON));

    KDBusService service(KDBusService::Unique|KDBusService::NoExitOnFailure);
    // Only get here if this is a unique application, i.e. there is no
    // previous instance running.  The NoExitOnFailure is included so that
    // if there is a DBus problem we will try to continue anyway.

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.setApplicationDescription(aboutData.shortDescription());
    parser.addHelpOption();
    parser.addVersionOption();

    // command and arguments
    parser.addOption(QCommandLineOption("w", i18n("Run in a normal window, without a system tray icon")));

    parser.process(app);
    aboutData.processCommandLine(&parser);

    WallpaperSwitch *sw = new WallpaperSwitch(parser.isSet("w"));
    QObject::connect(&service, &KDBusService::activateRequested, sw, &WallpaperSwitch::slotPreferences);

    return (app.exec());				// switcher will show itself
}
