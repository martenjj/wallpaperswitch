#!/bin/sh
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

SRC=$1							# CMAKE_CURRENT_SOURCE_DIR
BIN=$2							# CMAKE_CURRENT_BINARY_DIR
VER=$3							# base application version

VCSTYPE=						# VCS type detected
VCSREV=							# VCS revision

if [ -d "$SRC/.svn" ]					# is source under SVN?
then
	VCSTYPE="SVN"
	VCSREV=`svn info $SRC 2>/dev/null | sed -n -e'/^Revision:/ { s/^[^:]*: *//;p;q }'`
	echo "Current $VCSTYPE revision: ${VCSREV:-unknown}"
else							# source not under SVN
	if [ -d "$SRC/.git" ]				# try for GIT instead
	then
		VCSTYPE="GIT"
		VCSREV=`cd $SRC && git describe 2>/dev/null`
							# formatted version
		if [ ! -n "$VCSREV" ]			# if not available then
		then					# hash of last commit
			VCSREV=`cd $SRC && git log -1 --abbrev-commit | sed -e 's/commit  *//;q'`
		fi
		echo "Current $VCSTYPE revision: ${VCSREV:-unknown}"
	fi
fi


TMPFILE="$BIN/version.h.tmp"				# temporary header file
{
	echo "#ifndef VERSION"
	echo "#define VERSION              \"${VER}\""
	echo "#endif"
	echo
	echo "#define VCS_REVISION_STRING  \"${VCSREV}\""
	echo "#define VCS_TYPE_STRING      \"${VCSTYPE}\""
	if [ -n "$VCSREV" ]
	then
		echo "#define VCS_HAVE_VERSION     1"
	fi
	echo
} >$TMPFILE


OUTFILE="$BIN/version.h"				# the real header file
if [ ! -f $OUTFILE ]					# does not exist yet
then
	echo "Creating $OUTFILE..."
else							# already exists
	if cmp -s $TMPFILE $OUTFILE			# is it still current?
	then
		rm $TMPFILE				# yes, nothing to do
		exit 0
	else
		echo "Updating $OUTFILE..."
	fi
fi
mv $TMPFILE $OUTFILE					# update the header file
exit 0							# successfully updated
