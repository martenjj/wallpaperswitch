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
        return "file://" + root.source;
    }

    property bool imageReady: false
    property bool videoReady: false

    function restart() {
        if (root.isVideo && root.active) player.play();
    }

    onSourceChanged: {
        root.imageReady = false;
        root.videoReady = false;
    }

    Image {
        id: image
        anchors.fill: parent
        visible: !root.isVideo
        // Loading a large wallpaper synchronously would block the whole
        // Plasma shell, which is very noticeable when it happens on every
        // virtual desktop change.
        asynchronous: true
        cache: false
        fillMode: root.fillMode
        source: root.isVideo ? "" : root.sourceUrl
        sourceSize: Qt.size(root.width, root.height)
        onStatusChanged: root.imageReady = (status === Image.Ready)
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

        onMediaStatusChanged: {
            // The video can be shown as soon as it has been loaded and
            // decoding has started.  EndOfMedia is included because a
            // very short video may reach it immediately.
            switch (mediaStatus) {
            case MediaPlayer.LoadedMedia:
            case MediaPlayer.BufferingMedia:
            case MediaPlayer.BufferedMedia:
            case MediaPlayer.EndOfMedia:
                root.videoReady = true;
                // There is no automatic playback in Qt 6, the video has
                // to be started explicitly once it has been loaded.
                if (root.active && playbackState !== MediaPlayer.PlayingState) play();
                break;
            case MediaPlayer.InvalidMedia:
            case MediaPlayer.NoMedia:
                root.videoReady = false;
                break;
            }
        }

        onErrorOccurred: (error, errorString) => {
            console.warn("wallpaperswitch: cannot play", root.source, errorString);
            // Give up waiting for this video, so that the fade is not
            // held up for ever by a file which will never play.
            root.videoReady = true;
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
