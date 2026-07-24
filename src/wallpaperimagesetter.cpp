//////////////////////////////////////////////////////////////////////////
//									//
//  Project:	Plasma 6 Wallpaper Switcher				//
//  Edit:	24-Jul-26						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Copyright (c) 2024 Jonathan Marten <jjm@keelhaul.me.uk>		//
//  Home and download page:  http://www.keelhaul.me.uk/TBD/		//
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
//  details.  If not, see <http://www.gnu.org/licenses>.		//
//									//
//////////////////////////////////////////////////////////////////////////

#undef DEBUG_CONTAINMENT

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "wallpaperimagesetter.h"

#include <qguiapplication.h>
#include <qdbusmessage.h>
#include <qdbusconnection.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qcursor.h>
#include <qurl.h>
#include <qmimedatabase.h>
#include <qmimetype.h>
#include <qstandardpaths.h>
#include <qjsondocument.h>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qpair.h>
#include <qlist.h>
#ifdef DEBUG_CONTAINMENT
#include <qdebug.h>
#endif

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

#include "settings.h"
#include "libwallpaper_logging.h"


//////////////////////////////////////////////////////////////////////////
//									//
//  The Plasma wallpaper plugins used.  Still images are set using the	//
//  standard "Image" plugin which is always available; videos need the	//
//  "Smart Video Wallpaper Reborn" plugin, see			 	//
//  https://github.com/luisbocanegra/plasma-smart-video-wallpaper-reborn	//
//  which the user needs to install.  The plugin ID for videos can be	//
//  overridden in the configuration file, for the case where a fork or	//
//  a compatible successor of that plugin is installed instead.		//
//									//
//////////////////////////////////////////////////////////////////////////

static const char imagePluginId[] = "org.kde.image";

//////////////////////////////////////////////////////////////////////////
//									//
//  Utilities							 	//
//									//
//////////////////////////////////////////////////////////////////////////

// Escape a string so that it can safely be embedded within a
// double quoted string literal in the Plasma script below.
static QString jsEscape(const QString &str)
{
    QString res = str;
    res.replace('\\', "\\\\");
    res.replace('"', "\\\"");
    res.replace('\n', "\\n");
    res.replace('\r', "\\r");
    return (res);
}


/* static */ QString WallpaperImageSetter::videoPluginId()
{
    const QString plugin = Settings::videoWallpaperPlugin();
    return (plugin.isEmpty() ? QString("luisbocanegra.smart.video.wallpaper.reborn") : plugin);
}


/* static */ bool WallpaperImageSetter::videoPluginAvailable()
{
    const QString dir = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                               "plasma/wallpapers/"+videoPluginId(),
                                               QStandardPaths::LocateDirectory);
    return (!dir.isEmpty());
}


/* static */ QStringList WallpaperImageSetter::videoMimeTypes()
{
    // There is no way to ask the wallpaper plugin which video formats it
    // supports, because that depends on the Qt multimedia backend and the
    // codecs installed.  Offer the common container formats which the
    // plugin is known to be able to play.
    return (QStringList()
            << "video/mp4"
            << "video/x-matroska"
            << "video/webm"
            << "video/quicktime"
            << "video/x-msvideo"
            << "video/x-ms-wmv"
            << "video/mpeg"
            << "video/ogg"
            << "video/x-flv"
            << "video/3gpp");
}


/* static */ WallpaperImageSetter::MediaType WallpaperImageSetter::mediaType(const QString &file)
{
    if (file.isEmpty()) return (WallpaperImageSetter::Image);

    // Only use the file name to detect the type, not its contents.  The
    // file may not exist (when checking an entry from the configuration),
    // and looking at the contents would need it to be read.
    QMimeDatabase db;
    const QMimeType mime = db.mimeTypeForFile(file, QMimeDatabase::MatchExtension);
    if (mime.isValid() && !mime.isDefault())
    {
        if (mime.name().startsWith("video/")) return (WallpaperImageSetter::Video);
        const QStringList ancestors = mime.allAncestors();
        for (const QString &anc : ancestors)
        {
            if (anc.startsWith("video/")) return (WallpaperImageSetter::Video);
        }
        return (WallpaperImageSetter::Image);
    }

    // The MIME type could not be resolved, fall back to the file suffix.
    static const QStringList videoSuffixes = { "mp4", "m4v", "mkv", "webm", "mov",
                                               "avi", "wmv", "mpg", "mpeg", "ogv",
                                               "flv", "3gp" };
    const QString suffix = QFileInfo(file).suffix().toLower();
    return (videoSuffixes.contains(suffix) ? WallpaperImageSetter::Video : WallpaperImageSetter::Image);
}


// Generate the value for the "VideoUrls" configuration key of the video
// wallpaper plugin.  That is a JSON array of objects, one for each video
// known to the plugin, and the plugin plays those which are enabled.
//
// Any existing entries are retained so that per-video settings (playback
// rate, looping and so on) that the user may have set in the plugin
// configuration are not lost, but only the video wanted here is left
// enabled.  If the video is not present in the list then an entry for it
// is added, with the same default values as the plugin itself uses in
// package/contents/ui/code/utils.js createVideo().
static QString generateVideoUrls(const QString &existing, const QString &videoUrl)
{
    QJsonArray videos;

    const QJsonDocument doc = QJsonDocument::fromJson(existing.toUtf8());
    if (doc.isArray()) videos = doc.array();
    else if (!existing.trimmed().isEmpty())
    {
        // An older version of the plugin stored the videos as a plain
        // list of file URLs, one per line.  Convert those to the current
        // format so as not to lose them.
        const QStringList lines = existing.split('\n', Qt::SkipEmptyParts);
        for (const QString &line : lines)
        {
            QJsonObject video;
            video.insert("filename", line.trimmed());
            videos.append(video);
        }
    }

    bool found = false;
    for (int i = 0; i<videos.count(); ++i)
    {
        QJsonObject video = videos.at(i).toObject();
        const bool isWanted = (video.value("filename").toString()==videoUrl);
        video.insert("enabled", isWanted);
        videos.replace(i, video);
        if (isWanted) found = true;
    }

    if (!found)
    {
        QJsonObject video;
        video.insert("filename", videoUrl);
        video.insert("enabled", true);
        video.insert("duration", 0);
        video.insert("customDuration", 0);
        video.insert("playbackRate", 0.0);
        video.insert("alternativePlaybackRate", 0.0);
        video.insert("loop", false);
        videos.append(video);
    }

    return (QString::fromUtf8(QJsonDocument(videos).toJson(QJsonDocument::Compact)));
}


/* private */ bool WallpaperImageSetter::runPlasmaScript(const QString &script)
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.kde.plasmashell", "/PlasmaShell", "org.kde.PlasmaShell", "evaluateScript");
    message.setArguments(QVariantList() << QVariant(script));
    QDBusMessage reply = QDBusConnection::sessionBus().call(message, QDBus::BlockWithGui, 2000);
    if (reply.type()==QDBusMessage::ErrorMessage)
    {
        mErrorString = xi18nc("@info:shell", "DBus error, %1", reply.errorMessage());
        return (false);
    }

    return (true);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Setting the wallpaper					 	//
//									//
//////////////////////////////////////////////////////////////////////////

bool WallpaperImageSetter::setImage(const QString &imageFile, int screenIndex)
{
    mErrorString.clear();

    if (imageFile.isEmpty() || !QFile::exists(imageFile))
    {
        mErrorString = xi18nc("@info:shell", "The wallpaper file <filename>%1</filename> does not exist", imageFile);
        return (false);
    }

    const WallpaperImageSetter::MediaType type = WallpaperImageSetter::mediaType(imageFile);
    if (type==WallpaperImageSetter::Video && !WallpaperImageSetter::videoPluginAvailable())
    {
        mErrorString = xi18nc("@info:shell",
                              "The video wallpaper plugin <resource>%1</resource> is not installed. "
                              "Install the <application>Smart Video Wallpaper Reborn</application> plugin "
                              "in order to use a video as the wallpaper.", WallpaperImageSetter::videoPluginId());
        return (false);
    }

    const QList<QScreen *> screens = QGuiApplication::screens();
    if (screenIndex==-1) screenIndex = screens.indexOf(QGuiApplication::screenAt(QCursor::pos()));
    if (screenIndex<0 || screenIndex>=screens.count())
    {
        mErrorString = xi18nc("@info:shell", "Screen %1 is not present (screen count is %2)", screenIndex, screens.count());
        return (false);
    }

    const KSharedConfig::Ptr config = KSharedConfig::openConfig("plasma-org.kde.plasma.desktop-appletsrc",
                                                                KSharedConfig::SimpleConfig);
    // The configuration object is cached and shared, so it may have been
    // read earlier in this process.  Plasma will have updated the file
    // since then, both by the wallpaper settings written here and by the
    // wallpaper plugins saving their own state, so read it again to be
    // sure of seeing the current values.
    config->reparseConfiguration();

    const KConfigGroup containmentGroup1 = config->group("Containments");
#ifdef DEBUG_CONTAINMENT
    qCDebug(DEBUGCAT) << "groups in" << containmentGroup1.name() << "=" << containmentGroup1.groupList();
#endif
    for (const QString &cont : containmentGroup1.groupList())
    {
        const KConfigGroup containmentGroup2 = containmentGroup1.group(cont);
#ifdef DEBUG_CONTAINMENT
        qCDebug(DEBUGCAT) << "  groups in" << containmentGroup2.name() << "=" << containmentGroup2.groupList();
#endif
        if (!containmentGroup2.groupList().contains("Wallpaper")) continue;

        const int lastScreen = containmentGroup2.readEntry("lastScreen", -1);
        if (lastScreen==-1) continue;
#ifdef DEBUG_CONTAINMENT
        qCDebug(DEBUGCAT) << "  on screen" << lastScreen;
#endif
        if (lastScreen==screenIndex)
        {
            const QString plugin = containmentGroup2.readEntry("wallpaperplugin", "");
#ifdef DEBUG_CONTAINMENT
            qCDebug(DEBUGCAT) << "found containment" << qPrintable(cont) << "plugin" << plugin;
#endif
            // The wallpaper plugin needed depends on whether the configured
            // wallpaper file is a still image or a video.  If the currently
            // set plugin is not the one required then it is changed by the
            // script below, so that images and videos can be mixed between
            // virtual desktops.
            const QString wantPlugin = (type==WallpaperImageSetter::Video ? WallpaperImageSetter::videoPluginId()
                                                                          : QString(imagePluginId));
            qCDebug(DEBUGCAT) << "screen" << screenIndex << "plugin" << plugin << "want" << wantPlugin;

            // The configuration keys and values to write for that plugin.
            QList<QPair<QString, QString>> configEntries;
            if (type==WallpaperImageSetter::Video)
            {
                const KConfigGroup pluginGroup = containmentGroup2.group("Wallpaper").
                                                     group(wantPlugin).group("General");

                // The video plugin refers to its videos by URL, and keeps the
                // list of them together with their individual settings.
                const QString videoUrl = QUrl::fromLocalFile(imageFile).toString();
                const QString existing = pluginGroup.readEntry("VideoUrls", "");
                configEntries.append(qMakePair(QString("VideoUrls"), generateVideoUrls(existing, videoUrl)));

                // The plugin remembers the video that was playing last, and
                // resumes it when it starts up again.  It looks that video up
                // by name in its list of enabled videos, and if it is not
                // found there then it ends up with no video to play at all -
                // showing just the background colour until something else
                // happens to change the configuration.  That is what happens
                // when the wallpaper of the previous virtual desktop was a
                // different video, which has just been disabled above.  So
                // point the plugin at the video wanted here, meaning that it
                // has one to play as soon as it starts.
                const QString lastVideo = pluginGroup.readEntry("LastVideo", "");
                configEntries.append(qMakePair(QString("LastVideo"), videoUrl));

                // The saved playback position belongs to whichever video was
                // playing before.  Resuming a different video at that position
                // would be wrong, so start it from the beginning;  but if it
                // is the same video then let it resume where it left off.
                if (lastVideo!=videoUrl) configEntries.append(qMakePair(QString("LastVideoPosition"), QString("0")));
            }
            else configEntries.append(qMakePair(QString("Image"), imageFile));

            // The script statements to write those values.
            QString writeStatements;
            for (const QPair<QString, QString> &entry : std::as_const(configEntries))
            {
                writeStatements += QString("d.writeConfig(\"%1\", \"%2\");").
                                       arg(jsEscape(entry.first), jsEscape(entry.second));
            }

            // Script copied and adapted from plasma-workspace/wallpapers/image/
            //    wallpaperfileitemactionplugin/wallpaperfileitemaction.cpp
            //
            // Properties declared in plasma-workspace/shell/scripting/scriptengine_v1.h
            //    Q_INVOKABLE QJSValue desktopsForActivity(const QJSValue &id = QJSValue()) const;
            // (which returns an array of Containment values)
            //    Q_INVOKABLE QJSValue desktopForScreen(const QJSValue &screen = QJSValue()) const;
            // (which returns a Containment value)
            //
            // Properties declared in plasma-workspace/shell/scripting/containment.h
            //    Q_PROPERTY(int screen READ screen)

            // The operation of this script depends on the correct configuration of
            // the Plasma desktop as regards activities, even if activities are not
            // actually being used.  If setting the wallpaper image via the script
            // below does not seem to work, first check that an image can be set
            // using the plasma-apply-wallpaperimage(1) command.  If this does not
            // work either, check that the desktop and activities are working
            // properly by doing:
            //
            //   qdbus org.kde.plasmashell /PlasmaShell evaluateScript 'SCRIPT'
            //
            // with SCRIPT as in turn:
            //
            //   print("current activity "+currentActivity());
            //   print("desktops count "+desktops().length);
            //   print("desktops for activity count "+desktopsForActivity(currentActivity()).length);
            //
            // and check that the results are as expected.

            // Changing the wallpaper plugin (as is done here if the type of
            // the wallpaper file requires it) is the same as is done by the
            // plasma-apply-wallpaperimage(1) command, which also reloads the
            // containment configuration afterwards.

            // Placeholders are substituted individually, and the statements
            // containing the wallpaper file name last, so that any text
            // within a file name that looks like a placeholder cannot be
            // substituted again.
            QString script("const allDesktops = desktopsForActivity(currentActivity());"
                           "for (i=0; i<allDesktops.length; i++)"
                           "{"
                           "    d = allDesktops[i];"
                           "    if (d.screen==@SCREEN@)"
                           "    {"
                           "        if (d.wallpaperPlugin!=\"@PLUGIN@\") d.wallpaperPlugin = \"@PLUGIN@\";"
                           "        d.currentConfigGroup = Array(\"Wallpaper\", \"@PLUGIN@\", \"General\");"
                           "        @WRITES@"
                           "        d.reloadConfig();"
                           "    }"
                           "}");
            script.replace("@SCREEN@", QString::number(screenIndex));
            script.replace("@PLUGIN@", jsEscape(wantPlugin));
            script.replace("@WRITES@", writeStatements);

            return (runPlasmaScript(script));
        }
    }

    mErrorString = xi18nc("@info:shell", "Could not find the Plasma containment for screen %1", screenIndex);
    return (false);
}
