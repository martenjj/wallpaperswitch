##########################################################################
##									##
##  This file is part of Wallpaper Switcher, a virtual desktop		##
##  wallpaper (background image) switcher for KDE Plasma 6.		##
##									##
##  Copyright (c) 2016-2025 Jonathan Marten <jjm@keelhaul.me.uk>	##
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
#  Requirements								#
#########################################################################

find_package(Qt${Q} ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Widgets)
if (NOT X11_ONLY)
  find_package(Qt${Q} ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS DBus)
endif ()

find_package(KF${Q} ${KF_MIN_VERSION} REQUIRED COMPONENTS I18n CoreAddons Config ConfigWidgets WidgetsAddons IconThemes XmlGui KIO DBusAddons Crash)
if (${Q} GREATER_EQUAL 6)
  find_package(KF${Q} ${KF_MIN_VERSION} REQUIRED COMPONENTS StatusNotifierItem)
else ()
  find_package(KF${Q} ${KF_MIN_VERSION} REQUIRED COMPONENTS Notifications)
endif ()
if (X11_ONLY)
  find_package(KF${Q} ${KF_MIN_VERSION} REQUIRED COMPONENTS WindowSystem)
endif ()

check_function_exists(strerror HAVE_STRERROR)
if (HAVE_STRERROR)
  add_definitions(-DHAVE_STRERROR)
  check_include_file(errno.h HAVE_ERRNO_H)
  if (HAVE_ERRNO_H)
  add_definitions(-DHAVE_ERRNO_H)
  endif (HAVE_ERRNO_H)
endif (HAVE_STRERROR)

include_directories("${CMAKE_CURRENT_BINARY_DIR}/..")

#########################################################################
#  Building the library 						#
#########################################################################

set(libwallpaper_SRCS
  wallpaperimagesetter.cpp
  wallpaperswitcher.cpp
  preferencespage.cpp
  switcherinterface.cpp
)

set(libwallpaper_HDRS
  wallpaperimagesetter.h
  wallpaperswitcher.h
  preferencespage.h
  ${CMAKE_CURRENT_BINARY_DIR}/libwallpaper_export.h
)

ecm_qt_declare_logging_category(libwallpaper_SRCS
  HEADER libwallpaper_logging.h
  IDENTIFIER "DEBUGCAT"
  CATEGORY_NAME "wallpapersettings"
  DESCRIPTION "Wallpaper Switcher"
  DEFAULT_SEVERITY "Debug"
)

kconfig_add_kcfg_files(libwallpaper_SRCS settings.kcfgc)
add_library(libwallpaper ${libwallpaper_SRCS})
set_target_properties(libwallpaper PROPERTIES LIBRARY_OUTPUT_NAME wallpaperswitcher VERSION ${VERSION} SOVERSION ${Q})
generate_export_header(libwallpaper BASE_NAME libwallpaper)

target_link_libraries(libwallpaper
  Qt${Q}::Core Qt${Q}::Widgets
  KF${Q}::I18n
  KF${Q}::ConfigCore KF${Q}::ConfigWidgets
  KF${Q}::WidgetsAddons
  KF${Q}::KIOCore KF${Q}::KIOFileWidgets
)

if (X11_ONLY)
  target_compile_definitions(libwallpaper PUBLIC X11_ONLY)
  target_link_libraries(libwallpaper KF${Q}::WindowSystem)
else ()
  target_link_libraries(libwallpaper Qt${Q}::DBus)
endif ()

if (${Q} GREATER_EQUAL 6)
  target_link_libraries(libwallpaper KF${Q}::StatusNotifierItem)
endif ()

#########################################################################
#  Library package config 						#
#########################################################################

set (CONFIGDIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/LibWallpaperSwitcher")
configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/LibWallpaperSwitcherConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/LibWallpaperSwitcherConfig.cmake"
  INSTALL_DESTINATION ${CONFIGDIR})

#########################################################################
#  Building the application						#
#########################################################################

if (BUILD_SWITCHER)
  set(wallpaperswitch_SRCS
    main.cpp
    switcherapp.cpp
    systemtraywidget.cpp
    preferencesdialogue.cpp
  )

  add_executable(wallpaperswitch ${wallpaperswitch_SRCS})
  add_dependencies(wallpaperswitch vcsversion)

  target_link_libraries(wallpaperswitch
    Qt${Q}::Core Qt${Q}::Widgets
    KF${Q}::I18n
    KF${Q}::CoreAddons
    KF${Q}::ConfigCore KF${Q}::ConfigWidgets
    KF${Q}::WidgetsAddons
    KF${Q}::IconThemes
    KF${Q}::XmlGui
    KF${Q}::KIOCore KF${Q}::KIOFileWidgets
    KF${Q}::DBusAddons
    KF${Q}::Crash
    libwallpaper
  )

  if (${Q} GREATER_EQUAL 6)
    target_link_libraries(wallpaperswitch KF${Q}::StatusNotifierItem)
  else ()
    target_link_libraries(wallpaperswitch KF${Q}::Notifications)
  endif ()
endif ()

#########################################################################
#  Installation								#
#########################################################################

install(TARGETS libwallpaper ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES ${libwallpaper_HDRS} DESTINATION ${KDE_INSTALL_INCLUDEDIR}/libwallpaper)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/LibWallpaperSwitcherConfig.cmake" DESTINATION ${CONFIGDIR} COMPONENT Devel)

if (BUILD_SWITCHER)
  install(TARGETS wallpaperswitch DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
endif ()
