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

#ifndef PREFERENCESDIALOGUE_H
#define PREFERENCESDIALOGUE_H

#include <kpagedialog.h>


class QCheckBox;
class QTreeWidget;
class QTreeWidgetItem;
class PreferencesPage;

//////////////////////////////////////////////////////////////////////////
//									//
//  PreferencesDialogue -- The container dialogue			//
//									//
//////////////////////////////////////////////////////////////////////////

class PreferencesDialogue : public KPageDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialogue(bool soloMode, QWidget *pnt = NULL);
    virtual ~PreferencesDialogue();

    void setWallpaperPath(const QString &path);

protected:
    void loadSettings();

protected slots:
    void saveSettings();
    void slotAbout();

private:
    Q_DISABLE_COPY(PreferencesDialogue)
    PreferencesPage *mWallpaperPage;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  PreferencesPage -- Abstract base class for a page of the dialogue	//
//									//
//////////////////////////////////////////////////////////////////////////

class PreferencesPage : public QWidget
{
    Q_OBJECT

public:
    explicit PreferencesPage(QWidget *pnt = NULL) : QWidget(pnt)	{};
    virtual ~PreferencesPage()						{};

    virtual void loadSettings() = 0;
    virtual void saveSettings() = 0;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  PreferencesWallpaperPage -- Settings for the wallpaper switcher	//
//									//
//////////////////////////////////////////////////////////////////////////

class PreferencesWallpaperPage : public PreferencesPage
{
    Q_OBJECT

public:
    explicit PreferencesWallpaperPage(QWidget *pnt = NULL);
    virtual ~PreferencesWallpaperPage()				{};

    void loadSettings();
    void saveSettings();

    void setWallpaperPath(const QString &path)		{ mWallpaperPath = path; }

protected slots:
    void slotSetWallpaper(QTreeWidgetItem *item = NULL);
    void slotInfoLinkActivated(const QString &url);

private slots:
    void slotUpdateButtonStates();

private:
    QCheckBox *mEnableSwitcherCheck;
    QTreeWidget *mWallpaperList;
    QPushButton *mSetWallpaperButton;

    QString mWallpaperPath;
};

#endif							// PREFERENCESDIALOGUE_H
