Wallpaper Switcher for KDE Plasma 6
===================================

One of the changes to the Plasma workspace in Plasma&nbsp;5 (based on
KDE Frameworks&nbsp;5) and continued in Plasma&nbsp;6 (based on KDE
Frameworks&nbsp;6) is that the option of having separate Plasma
widgets on each virtual desktop is no longer available.  This is
unfortunate, because one useful side effect of being able to do that
was the ability to have different desktop backgrounds (wallpaper) on
different virtual desktops, so proving a useful visual cue as to the
desktop currently displayed.

The most relevant KDE bug is https://bugs.kde.org/show_bug.cgi?id=341143

The position of the Plasma developers, as stated in the evaluation of
that bug, is that there have been major architectural changes within
Plasma and at the moment the option is not available, with no simple
way to bring it back.  It has been acknowledged as an omission and is
currently being worked on.  Please be considerate to the developers
and refrain from commenting on the bug unless you really do have a
useful contribution to make to the discussion.

This system tray application attempts to being back the ability to
have different wallpapers on different virtual desktops, pending the
feature being reintroduced into Plasma.  It allows a wallpaper image
file (or, see below, a video file) to be configured for each virtual
desktop, and will change the
background when the virtual desktop changes.  It does this by changing
the wallpaper image settings, so there is some Plasma configuration
needed in order to use it.  See below for instructions on how to do
that.


Building and installing
-----------------------

The application is built and installed in much the same way as any
other KDE application.  Assuming that you have Qt&nbsp;6, KDE
Frameworks&nbsp;6 and the Plasma desktop installed, go to a suitable
build location (e.g. your home directory) and do:

     git clone https://github.com/martenjj/wallpaperswitch.git
     cd wallpaperswitch
     git switch kf6
     mkdir build
     cd build
     cmake ..
     make
     sudo make install

Building requires CMake and also KDE Frameworks development headers
and libraries.  Depending on your distro, you may need to install
additional packages for these.  If you are running Kubuntu then see
the <A HREF="README.kubuntu">README.kubuntu</A> file for a list of
packages required.


Running and configuration
-------------------------

After the files are installed above, start the switcher either from
whichever desktop launcher you use (it will appear in the "Utilities"
category), or by typing the command 'wallpaperswitch' in a terminal or
KRunner (summoned by typing Alt-F2).  The "desktop" icon will appear
in the system tray.  If you set the "Start automatically on desktop
startup" option on in the settings, the switcher will start
automatically when the Plasma desktop starts.

If this is the first time that the application has been run the the
configuration dialogue will appear automatically.  Otherwise, click
the right mouse button over the system tray icon and select the
"Configure..." option from the menu.  Turn on the "Enable the desktop
wallpaper switcher" check box if necessary.

Each currently configured virtual desktop will be listed there.  To
select the wallpaper image for a desktop, double click the entry or
select it and click the "Set Wallpaper" button.  From the file
dialogue that appears, choose an image file (or a video file, see
below).  Repeat the same for each
virtual desktop or, if you have more than one screen, each screen for
each virtual desktop.

Click the "Plasma desktop settings" link at the bottom right, and
follow the instructions there to set the required desktop settings.
Then finally click "OK" to save the configuration.

The wallpaper image that you selected for the current virtual desktop
should now appear, and change when the virtual desktop changes.

Switching can be enabled or disabled using the "Enable Switching"
option on the system tray popup menu.


Video wallpapers
----------------

As well as a still image, a video file (MP4, MKV, WebM, MOV and other
common formats) can be selected as the wallpaper for a virtual
desktop.  Plasma cannot play videos as the desktop background by
itself, so this needs the "Smart Video Wallpaper Reborn" wallpaper
plugin to be installed:

  https://github.com/luisbocanegra/plasma-smart-video-wallpaper-reborn

The easiest way to install it is with the "Get New Plugins" button in
the desktop wallpaper settings dialogue, searching for "Smart Video
Wallpaper Reborn".  Once it is installed, video files will appear in
the file dialogue of the "Set Wallpaper" button alongside the image
files.

Still images and videos can be mixed freely between virtual desktops
and screens.  When the wallpaper is set, the Plasma "Wallpaper type"
of the screen is switched automatically between "Image" and "Smart
Video Wallpaper Reborn" as required by the file selected, so there is
no need to set the wallpaper type manually for videos.

The video plugin keeps a list of videos together with their individual
settings (playback rate, looping and so on).  Setting a video as the
wallpaper adds it to that list if it is not already there, leaves only
that video enabled, and does not disturb any settings that have been
made for the other videos in the list.  This means that per-video
settings can still be adjusted in the plugin's own configuration
dialogue and they will be retained.

If a fork or a compatible successor of the plugin is installed under a
different plugin ID, then set that ID as the "videoWallpaperPlugin"
key in the "[Wallpaper]" group of the "wallpaperswitchrc"
configuration file.

A video is shown in the configuration list as a thumbnail of its first
frame.  Generating that needs a video thumbnailer to be installed, the
usual one being "ffmpegthumbs" (the package is called that on Kubuntu
and most other distributions).  If there is none installed then a
generic video icon is shown instead.


Transitions between wallpapers
------------------------------

Changing from one video to another is done by the video plugin itself,
and it can fade between them.  To enable that, open the wallpaper
settings for the desktop and turn on the "Crossfade" option of the
video wallpaper plugin.

Changing between a still image and a video, in either direction, means
that the Plasma wallpaper plugin of the desktop has to be switched.
Plasma has no transition between one wallpaper plugin and another, so
there is a brief blank while the old one is torn down and the new one
starts up.  This is a limitation of the Plasma desktop and cannot be
avoided from here.  If completely smooth transitions are wanted, then
use a video (which may be a still one) as the wallpaper for every
virtual desktop, so that the plugin never has to be switched.


Problems?
---------

If you are able to build and install the application, but it will not
run or it is not able to change the desktop background, then try
starting it from a terminal window within the Plasma desktop.  Observe
the debugging messages for anything about missing files or any other
problems.

If this does not indicate the problem then please raise an issue in
GitHub (http://github.com/martenjj/wallpaperswitch).  Do not raise a
bug against KDE Plasma.


Plasma 5
--------

The original wallpaper switch for Plasma&nbsp;5 (branch 'master' in
this repository) no longer works because of changes to the Plasma desktop
slideshow plugin ([commit ea32a761 to plasma-workspace](https://invent.kde.org/plasma/plasma-workspace/-/commit/ea32a761)).

If you are using Plasma&nbsp;5 and wish to have different wallpaper
images per virtual desktop, then I suggest that you install the
"Vallpaper" plugin and set it as the desktop background.  Its
configuration dialogue will then allow you to select an image file (or
multiple images) for each virtual desktop.  You can obtain this plugin
via [GitHub](https://github.com/lehklu/Vallpaper), or by using the
"Get New Plugins" button in the desktop configuration dialogue.  I
have no connection with Vallpaper, apart from being able to report
that it appears to work well and does exactly what it says.  Unfortunately
it does not yet appear to have been updated for Plasma&nbsp;6.


Thanks for your interest!
-------------------------

If you use virtual desktops and want wallpaper switching then I hope
that this utility will come in useful, and also that one day it will
not be necessary...

Jonathan Marten, http://github.com/martenjj
