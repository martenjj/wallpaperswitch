Wallpaper Switching for KDE Plasma 5
====================================

**Unfortunately, due to recent changes in Plasma, WallpaperSwitch no
longer works.  Please read on for an explanation and an alternative
solution.**

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

WallpaperSwitch attempted to bring back the ability to have different
wallpapers on different virtual desktops, pending the feature being
reintroduced into Plasma.  It allowed a wallpaper image file to be
configured for each virtual desktop, changing the background when the
virtual desktop changes.  It did that by changing the wallpaper
slideshow image available.

Unfortunately, there have been changes within the Plasma desktop
slideshow plugin, in order to allow selection of multiple images and
displaying them in a configurable order.  This was implemented by
[commit ea32a761 to plasma-workspace](https://cgit.kde.org/plasma-workspace.git/commit/wallpapers/image/image.cpp?id=ea32a7611227ca141bd60d983d1489d2be82d10f)
on the 25th July 2019.  The change is useful, but as a side effect it
no longer notices changes to the slideshow images so this wallpaper
switching method will no longer work.  If you are using KDE distro
packages then this change may not have made it to your updates yet,
but it will surely do so soon.

**If you still wish to have different wallpaper images per virtual
desktop, then I suggest that you install the "Vallpaper" plugin**
and set it as the desktop background.  The configuration dialogue will
then allow you to select an image file (or more than one image) for
each virtual desktop.

You can obtain this plugin via [GitHub](https://github.com/lehklu/Vallpaper),
or by using the "Get New Plugins" button in the desktop configuration
dialogue.  I have no connection with Vallpaper, apart from being able
to report that it appears to work well and does exactly what it says.


Thanks for your past interest!
------------------------------

Jonathan Marten, http://github.com/martenjj
