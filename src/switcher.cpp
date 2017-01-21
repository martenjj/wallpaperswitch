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

#include "switcher.h"

#ifdef HAVE_STRERROR
#include <string.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#include <qfile.h>
#include <qdir.h>
#include <qstandardpaths.h>
#include <qtimer.h>

#include <kconfigskeleton.h>
#include <kwindowsystem.h>
#include <kdirwatch.h>

#include "debug.h"
#include "settings.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Debugging switches							//
//									//
//////////////////////////////////////////////////////////////////////////

#define DEBUG_CHANGE

//////////////////////////////////////////////////////////////////////////
//									//
//  Constructor/destructor					 	//
//									//
//////////////////////////////////////////////////////////////////////////

Switcher::Switcher(QObject *pnt)
    : QObject(pnt)
{
    qDebug();

    mIndexNumber = 0;

    // Find a temporary directory to use.  This cannot be a cache or temporary
    // location because it needs to be permanent;  if it does not exist on Plasma
    // startup then a KDirWatch will not be installed on it, so it will not
    // notice any changes.  So we use the QStandardPaths::AppLocalDataLocation
    // which will hopefully be within the user's home directory.
    QString tempDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    qDebug() << "using temp dir" << tempDir;
    mTempDir = new QDir(tempDir);
    if (!mTempDir->exists())
    {
        QString base = mTempDir->dirName();
        mTempDir->cdUp();
        if (!mTempDir->mkdir(base))
        {
            qDebug() << "cannot create directory" << mTempDir->absoluteFilePath(base);
#ifdef HAVE_STRERROR
#ifdef HAVE_ERRNO_H
            qDebug() << "reason" << strerror(errno);
#endif
#endif
            return;
        }
        mTempDir->cd(base);
        qDebug() << "created temp dir" << mTempDir->absolutePath();
    }

    connect(KWindowSystem::self(), SIGNAL(currentDesktopChanged(int)), SLOT(slotDesktopChanged(int)));
    QTimer::singleShot(0, this, SLOT(slotDesktopChanged()));	// set for current desktop

    connect(KDirWatch::self(), SIGNAL(dirty(const QString &)), SLOT(slotFileChanged(const QString &)));
}


Switcher::~Switcher()
{
    delete mTempDir;
    qDebug() << "done";
}

//////////////////////////////////////////////////////////////////////////
//									//
//									//
//////////////////////////////////////////////////////////////////////////

void Switcher::slotDesktopChanged(int desktop)
{
    if (!Settings::enableSwitcher()) return;

    if (desktop==0) desktop = KWindowSystem::currentDesktop();
#ifdef DEBUG_CHANGE
    qDebug() << "to" << desktop;
#endif // DEBUG_CHANGE

    KConfigSkeletonItem *ski = Settings::self()->wallpaperForDesktopItem();
    Q_ASSERT(ski!=NULL);
    const KConfigGroup grp = Settings::self()->config()->group(ski->group());

    QString file = grp.readEntry(QString::number(desktop), "");
    if (file.isEmpty())
    {
        qDebug() << "No wallpaper configured for desktop" << desktop;
        return;
    }

    if (!QFile::exists(file))
    {
        qDebug() << "File" << file << "not found for desktop" << desktop;
        return;
    }

    if (!mTempDir->exists())
    {
        qDebug() << "No temporary directory available!";
        return;
    }

    QStringList files = mTempDir->entryList(QDir::Files|QDir::NoDotAndDotDot|QDir::System);
    foreach (const QString &oldFile, files)		// "System" matches broken symlinks
    {
        // Stop watching for changes to the old background file
        const QString oldTarget = QFile::symLinkTarget(mTempDir->absoluteFilePath(oldFile));
        if (!oldTarget.isEmpty()) KDirWatch::self()->removeFile(oldTarget);

        // Remove the old symbolic link to the background file
        if (!mTempDir->remove(oldFile))
        {
            qDebug() << "unable to remove existing" << oldFile;
#ifdef HAVE_STRERROR
#ifdef HAVE_ERRNO_H
        qDebug() << "reason" << strerror(errno);
#endif // HAVE_ERRNO_H
#endif // HAVE_STRERROR
        }
#ifdef DEBUG_CHANGE
        else qDebug() << "removed existing" << file;
#endif // DEBUG_CHANGE
    }

    // The watched pathname has to change, so that the background plugin will
    // receive 'deleted' and 'created' signals.
    ++mIndexNumber;
    QString linkPath = mTempDir->absoluteFilePath(QString::number(mIndexNumber));

    // The link name has to have a valid image file suffix in order that it will
    // be accepted.  See Image::pathCreated() and BackgroundFinder::suffixes().
    // It doesn't have to actually match the file contents, though, so we
    // set one that is acceptable in order to allow any image file format.
    linkPath += ".png";

    if (!QFile::link(file, linkPath))
    {
        qDebug() << "cannot link" << linkPath << "->" << file;
#ifdef HAVE_STRERROR
#ifdef HAVE_ERRNO_H
        qDebug() << "reason" << strerror(errno);
#endif // HAVE_ERRNO_H
#endif // HAVE_STRERROR
        return;
    }

#ifdef DEBUG_CHANGE
    qDebug() << "created link" << linkPath << "->" << file;
#endif // DEBUG_CHANGE

    // Watch the actual background file for changes.
    KDirWatch::self()->addFile(file);
}


void Switcher::slotFileChanged(const QString &file)
{
#ifdef DEBUG_CHANGE
    qDebug() << file;
#endif // DEBUG_CHANGE
    slotDesktopChanged(0);				// update for current desktop
}


QString Switcher::wallpaperPath() const
{
    return (mTempDir==NULL ? QString::null : mTempDir->absolutePath());
}
