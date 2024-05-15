/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	Plasma 6 Wallpaper Switcher				//
//  Edit:	15-May-24						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Copyright (c) 2015-2024 Jonathan Marten <jjm@keelhaul.me.uk>	//
//  Home and download page:  http://www.keelhaul.demon.co.uk/TBD/	//
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
//  details.  If not, write to the Free Software Foundation, Inc.,	//
//  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.		//
//									//
//////////////////////////////////////////////////////////////////////////

#ifndef PREFERENCESPAGE_H
#define PREFERENCESPAGE_H

#include <qwidget.h>
#include "libwallpaper_export.h"


class QCheckBox;
class QTreeWidget;
class QTreeWidgetItem;
class QPushButton;

//////////////////////////////////////////////////////////////////////////
//									//
//  PreferencesPage -- Abstract base class for a page of the dialogue	//
//									//
//////////////////////////////////////////////////////////////////////////

class LIBWALLPAPER_EXPORT PreferencesPage : public QWidget
{
    Q_OBJECT

public:
    explicit PreferencesPage(QWidget *pnt = nullptr) : QWidget(pnt)	{};
    virtual ~PreferencesPage() = default;

    virtual void loadSettings() = 0;
    virtual void saveSettings() = 0;

    static void setStandalone(bool isStandlone);
};

//////////////////////////////////////////////////////////////////////////
//									//
//  PreferencesWallpaperPage -- Settings for the wallpaper switcher	//
//									//
//////////////////////////////////////////////////////////////////////////

class LIBWALLPAPER_EXPORT PreferencesWallpaperPage : public PreferencesPage
{
    Q_OBJECT

public:
    explicit PreferencesWallpaperPage(QWidget *pnt = nullptr);
    virtual ~PreferencesWallpaperPage() = default;

    void loadSettings() override;
    void saveSettings() override;

protected slots:
    void slotSetWallpaper(QTreeWidgetItem *item = nullptr);
    void slotInfoLinkActivated(const QString &url);

private slots:
    void slotUpdateButtonStates();

private:
    QCheckBox *mEnableSwitcherCheck;
    QCheckBox *mAutoStartCheck;
    QTreeWidget *mWallpaperList;
    QPushButton *mSetWallpaperButton;
};

#endif							// PREFERENCESPAGE_H
