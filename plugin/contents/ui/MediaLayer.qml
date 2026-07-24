/*
 * One layer of the wallpaper, showing either a still image or a video.
 *
 * Two of these are used by main.qml.  The incoming wallpaper is loaded
 * into the hidden layer and only faded in once it reports itself as
 * ready, so that there is never a gap where neither layer has anything
 * to show.
 *
 * Copyright (c) 2026 Jonathan Marten <jjm@keelhaul.me.uk>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtMultimedia

Item {
    id: root

    // The wallpaper file to show, as a URL or an absolute path.
    // Setting this to an empty string releases any resources held.
    property string source: ""

    // Fill mode, using the same values as the standard Plasma "Image"
    // wallpaper plugin.  Those match the values of Image.fillMode.
    property int fillMode: Image.PreserveAspectCrop

    property bool muted: true
    property real volume: 1.0

    // Whether the video (if this layer is showing one) should be playing.
    property bool active: true

    // True once there is something to display.  The layer must not be
    // faded in before this becomes true.
    readonly property bool ready: root.isVideo ? videoReady : imageReady

    readonly property bool isVideo: {
        if (root.source === "") return false;
        // Match the file extension, ignoring any URL query or fragment.
        const path = root.source.split("?")[0].split("#")[0];
        return /\.(mp4|m4v|mkv|webm|mov|avi|wmv|mpg|mpeg|ogv|flv|3gp)$/i.test(path);
    }

    readonly property url sourceUrl: {
        if (root.source === "") return "";
        if (root.source.indexOf("://") > 0) return root.source;
        // A plain file name may contain characters, a space being the
        // most likely, which are not valid in a URL.
        return "file://" + encodeURI(root.source);
    }

    // The readiness of the layer is a binding on the state of the image
    // or the player, never a flag which is set and cleared as things
    // happen.  A cached image can finish loading during the very
    // assignment of the source, before any handler for that assignment
    // has had a chance to run, so anything which clears a flag when the
    // source changes may wipe out the notification that the layer is
    // ready - after which nothing would ever set it again.

    readonly property bool imageReady: !root.isVideo && root.source!==""
                                       && (image.status===Image.Ready
                                           // An image which cannot be loaded will never
                                           // become ready, so treat it as ready anyway
                                           // rather than holding up the change for ever.
                                           || image.status===Image.Error)

    readonly property bool videoReady: root.isVideo && root.source!==""
                                       && (player.mediaStatus===MediaPlayer.LoadedMedia
                                           || player.mediaStatus===MediaPlayer.BufferingMedia
                                           || player.mediaStatus===MediaPlayer.BufferedMedia
                                           // A very short video may reach the end at once.
                                           || player.mediaStatus===MediaPlayer.EndOfMedia
                                           // As for an image above, do not wait for ever
                                           // for a video which will never play.
                                           || root.videoFailed)

    // Which source the player last reported an error for.  Recording the
    // source, rather than just that there was an error, means that this
    // is also a binding on the source and cannot be left set from before.
    property string failedSource: ""
    readonly property bool videoFailed: root.failedSource!=="" && root.failedSource===root.source

    Image {
        id: image
        anchors.fill: parent
        visible: !root.isVideo
        // Loading a large wallpaper synchronously would block the whole
        // Plasma shell, which is very noticeable when it happens on every
        // virtual desktop change.
        asynchronous: true

        // Keeping the decoded image means that changing back to a virtual
        // desktop whose wallpaper has been shown before does not have to
        // decode it again, so the fade can start immediately.
        cache: true

        fillMode: root.fillMode
        source: root.isVideo ? "" : root.sourceUrl

        // Decoding a very large image takes a noticeable time, so limit
        // the size to which it is decoded.  An image is scaled to fit
        // within this size, preserving its aspect ratio, so allowing
        // twice the screen size in each direction means that even an
        // image whose shape is very different from that of the screen
        // will not need to be scaled up again to fill it.
        sourceSize: Qt.size(root.width*2, root.height*2)
    }

    VideoOutput {
        id: videoOutput
        anchors.fill: parent
        visible: root.isVideo
        fillMode: {
            switch (root.fillMode) {
            case 0:	return VideoOutput.Stretch;
            case 1:	return VideoOutput.PreserveAspectFit;
            // Tiling a video is not meaningful, so anything else
            // (including the tiled modes) crops it to fill the screen.
            default:	return VideoOutput.PreserveAspectCrop;
            }
        }
    }

    AudioOutput {
        id: audioOutput
        muted: root.muted
        volume: root.volume
    }

    MediaPlayer {
        id: player
        videoOutput: videoOutput
        audioOutput: audioOutput
        loops: MediaPlayer.Infinite
        source: root.isVideo ? root.sourceUrl : ""

        // There is no automatic playback in Qt 6, so the video has to be
        // started explicitly once it has been loaded.  Whether it is
        // ready to be shown is a binding on mediaStatus, see above.
        onMediaStatusChanged: {
            if (root.videoReady && root.active && playbackState!==MediaPlayer.PlayingState) play();
        }

        onErrorOccurred: (error, errorString) => {
            console.warn("wallpaperswitch: cannot play", root.source, errorString);
            root.failedSource = root.source;
        }
    }

    // Stop the video when the layer is not being shown, so that a
    // wallpaper which cannot be seen does not use the CPU.
    onActiveChanged: {
        if (!root.isVideo) return;
        if (root.active) player.play();
        else player.pause();
    }
}
