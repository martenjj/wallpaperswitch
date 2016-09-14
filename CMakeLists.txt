##########################################################################
##									##
##  This file is part of Wallpaper Switcher, a virtual desktop		##
##  wallpaper (background image) switcher for KDE Plasma 5.		##
##									##
##  Copyright (c) 2016 Jonathan Marten <jjm@keelhaul.me.uk>		##
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

project(wallpaperswitch)
set(VERSION "1.0.0")
message(STATUS "Configuring for Wallpaper Switch version ${VERSION}")

#########################################################################
#									#
#  KDE paths and definitions						#
#									#
#########################################################################

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
set(QT_MIN_VERSION "5.4.0")
set(KF5_MIN_VERSION "5.15.0")
set(ECM_MIN_VERSION "1.8.0")

find_package(ECM ${ECM_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(ECMSetupVersion)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings)
include(ECMPackageConfigHelpers)
include(CMakePackageConfigHelpers)
include(ECMInstallIcons)
include(CheckFunctionExists)
include(CheckIncludeFile)

#########################################################################
#									#
#  VCS version tracking							#
#									#
#########################################################################

add_custom_target(vcsversion
  ALL
  COMMENT "Checking VCS type and version"
  VERBATIM
  COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/genversion.sh ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${VERSION}
)

#########################################################################
#									#
#  Rigourousness							#
#									#
#########################################################################

add_definitions("-DQT_USE_FAST_CONCATENATION")
add_definitions("-DQT_USE_FAST_OPERATOR_PLUS")
add_definitions("-DQT_NO_CAST_FROM_BYTEARRAY")
add_definitions("-DQT_NO_URL_CAST_FROM_STRING")
add_definitions("-DQT_NO_CAST_TO_ASCII")

remove_definitions("-DQT_NO_CAST_FROM_ASCII")
remove_definitions("-DQT_NO_SIGNALS_SLOTS_KEYWORDS")

add_definitions("-DVERSION=\"${VERSION}\"")
add_definitions("-DAPPICON=\"user-desktop\"")
add_definitions("-DTRANSLATION_DOMAIN=\"wallpaperswitch\"")

#########################################################################
#									#
#  Subdirectories							#
#									#
#########################################################################

add_subdirectory(desktop)
add_subdirectory(src)

#########################################################################
#									#
#  Information at end of cmake run					#
#									#
#########################################################################

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
