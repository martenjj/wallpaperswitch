##########################################################################
##									##
##  This file is part of Wallpaper Switcher, a virtual desktop		##
##  wallpaper (background image) switcher for KDE Plasma 6.		##
##									##
##  Copyright (c) 2026 Jonathan Marten <jjm@keelhaul.me.uk>		##
##  Home page:  http://github.com/martenjj/wallpaperswitch		##
##									##
##  This program is free software; you can redistribute it and/or	##
##  modify it under the terms of the GNU General Public License as	##
##  published by the Free Software Foundation; either version 2 of	##
##  the License, or (at your option) any later version.			##
##									##
##  It is distributed in the hope that it will be useful, but		##
##  WITHOUT ANY WARRANTY; without even the implied warranty of		##
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the	##
##  GNU General Public License for more details.			##
##									##
##  You should have received a copy of the GNU General Public		##
##  License along with this program; see the file COPYING for further	##
##  details.  If not, see http://www.gnu.org/licenses/			##
##									##
##########################################################################


#########################################################################
#  The Plasma wallpaper plugin						#
#									#
#  This is a QML package, so there is nothing to compile.  It only	#
#  needs to be installed where Plasma will find it.  The plugin ID	#
#  must agree with WallpaperImageSetter::mediaPluginId() in the		#
#  application, and with the translation domain used in the QML.	#
#########################################################################

set(MEDIA_PLUGIN_ID "uk.me.keelhaul.wallpaperswitch.media")

install(DIRECTORY contents
        DESTINATION ${KDE_INSTALL_DATADIR}/plasma/wallpapers/${MEDIA_PLUGIN_ID})

install(FILES metadata.json
        DESTINATION ${KDE_INSTALL_DATADIR}/plasma/wallpapers/${MEDIA_PLUGIN_ID})
