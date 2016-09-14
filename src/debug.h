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

#ifndef DEBUG_H
#define DEBUG_H

#ifdef QDEBUG_H
// This can happen when including a KConfigXT-generated header file,
// which inexplicably includes <QCoreApplication> and <QDebug>
#warning Included <qdebug.h> before "debug.h"
#endif

#include <qloggingcategory.h>

Q_DECLARE_LOGGING_CATEGORY(DEBUGCAT)

#ifdef qDebug
#undef qDebug
#endif
#define qDebug()	qCDebug(DEBUGCAT)

#ifdef qInfo
#undef qInfo
#endif
#define qInfo()		qCInfo(DEBUGCAT)

// We would normally want to see these in all cases
// #ifdef qCritical
// #undef qCritical
// #endif
// #define qCritical()	qCCritical(DEBUGCAT)
// 
// #ifdef qFatal
// #undef qFatal
// #endif
// #define qFatal()	qCCritical(DEBUGCAT)

#ifdef qWarning
#undef qWarning
#endif
#define qWarning()	qCWarning(DEBUGCAT)

#endif							// DEBUG_H
