Wallpaper Switcher for KDE Plasma 5
===================================

One of the changes to the Plasma workspace in Plasma 5 (based on KDE
Frameworks 5) is that the option of having separate Plasma widgets on
each virtual desktop is now gone.  This is unfortunate, because one
useful side effect of being able to do that was the ability to have
different desktop backgrounds (wallpaper) on different virtual
desktops, so proving a useful visual cue as to the desktop currently
displayed.

The most relevant KDE bug is https://bugs.kde.org/show_bug.cgi?id=341143

The position of the Plasma developers, as stated in the evaluation of
that bug, is that there have been major architectural changes within
Plasma and at the moment the option is not available, with no simple
way to bring it back.  It has been acknowledged as an omission and is
currently being worked on.  Please be considerate to the developers
and refrain from commenting on this bug unless you really do have a
useful contribution to make to the discussion.

This little system tray application attempts to being back the ability
to have different wallpapers on different virtual desktops, pending
the feature being reintroduced into Plasma.  It allows a wallpaper
image file to be configured for each virtual desktop, and will change
the background when the virtual desktop changes.  It does this by
changing the wallpaper slideshow image available, so there is some
Plasma configuration needed in order to use it.  See below for
instructions on how to do that.


Building and installing
-----------------------

The application is built and installed in much the same way as any
other KDE application.  Assuming that you have Qt 5, KDE Frameworks 5
and the Plasma desktop installed, go to a suitable build location
(e.g. your home directory) and do:

     git clone https://github.com/martenjj/wallpaperswitch.git
     cd wallpaperswitch
     mkdir build
     cd build
     cmake ..
     make
     sudo make install

Building requires CMake and also KDE Frameworks development headers
and libraries.  Depending on your distro, you may need to install
additional packages for these.


Running and configuration
-------------------------

After the files are installed above, start the switcher either from
whichever desktop launcher you use (it will appear in the "Utilities"
category), or by typing the command 'wallpaperswitch' in a terminal or
KRunner (summoned by typing Alt-F2).  The "desktop" icon will appear
in the system tray.

If this is the first time that the application has been run the the
configuration dialogue will appear automatically.  Otherwise, click
the right mouse button over the system tray icon and select the
"Configure..." option from the menu.  Turn on the "Enable the desktop
wallpaper switcher" check box if necessary.

Each currently configured virtual desktop will be listed there.  To
select the wallpaper image for a desktop, double click the entry or
select it and click the "Set Wallpaper" button.  From the file
dialogue that appears, choose an image file.  Repeat the same for each
virtual desktop.

Click the "Plasma desktop settings" link at the bottom right, and make
a note of the "Location" value shown there.  Depending on your
distro's configuration, this will be a pathname something like
"/home/yourname/.local/share/wallpaperswitch".  Then finally click
"OK" to save the configuration.

Then the Plasma desktop settings need to be configured.  Right click
over the desktop background and select "Configure Desktop".  Change
the "Wallpaper Type" to "Slideshow" and the "Positioning" to "Scaled".
The "Background colour" and "Change every" settings are not relevant
here.

If the list of folders below is not empty, then click the red "-"
button to the right of each entry to remove it.  Then click the "Add
Folder" button and select or enter the pathname that was given for
"Location" above.  Then click "OK" or "Apply".

The wallpaper image that you selected for the current virtual desktop
should now appear, and change when the virtual desktop changes.  If
nothing happens the first time, you may need to log out and log in
again (this is because originally the slideshow pathname didn't exist
so the slideshow plugin would not be watching it).

Switching can be enabled or disabled using the "Enable Switching"
option on the system tray popup menu.


Problems?
---------

If you are able to build and install the application, but it will not
run or it is not able to change the desktop background, then try
starting it from a terminal window within the Plasma desktop.  Observe
the debugging messages for anything about missing files or any other
problems.

If switching doesn't work and there are no obvious error indications
in the terminal as above, then ensure that the "Location" directory as
above exists and is writeable by you.  It should contain a single
symbolic link that changes in accordance with the virtual desktop.  If
this is all well, then check the Plasma slideshow settings and then
log out and log in again.

Please note that, for the wallpaper switching (or any use of the
slideshow wallpaper, even if not using this switcher application)
needs the revision 4be156bc (see
https://phabricator.kde.org/rPLASMAWORKSPACE4be156bc677692e96a273d02a3972447c444c94e
for details) to be incorporated in plasma-workspace.  If you are using
a self compiled KDE then check that this revision is included.  If you
are using KDE distro packages then check with your distro's version
control tracker.

If neither of this resolves the problem then please raise an issue in
GitHub (http://github.com/martenjj/wallpaperswitch).  Do not raise a
bug against KDE Plasma.


Thanks for your interest!
-------------------------

If you use virtual desktops and want wallpaper switching then I hope
that this utility will come in useful, and also that one day it will
not be necessary...

Jonathan Marten, http://github.com/martenjj
