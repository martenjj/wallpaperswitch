/*
 * Wallpaper plugin showing either a still image or a video, and fading
 * smoothly between them when the wallpaper changes.
 *
 * The standard Plasma "Image" wallpaper plugin cannot show a video, and
 * the video wallpaper plugins cannot show a still image.  Using one for
 * some virtual desktops and the other for the rest means that Plasma has
 * to switch the wallpaper plugin of the containment as the desktop
 * changes, and it destroys the old wallpaper as soon as the new one is
 * created (see ContainmentItem::loadWallpaper() in libplasma).  Nothing
 * is shown in between, so the desktop goes blank for as long as the new
 * wallpaper takes to load.
 *
 * Handling both sorts of wallpaper here means that the plugin never has
 * to be switched.  Two layers are kept:  the new wallpaper is loaded
 * into the hidden one, and the fade only starts once that layer reports
 * that it has something to show.
 *
 * The layer being faded out is always the one on top, and the layer
 * appearing from underneath it stays fully opaque throughout.  Fading
 * both layers at the same time would let the background show through in
 * the middle of the fade, which is exactly the dip that this plugin
 * exists to avoid.
 *
 * Copyright (c) 2026 Jonathan Marten <jjm@keelhaul.me.uk>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import org.kde.plasma.plasmoid
import org.kde.kirigami as Kirigami

WallpaperItem {
    id: root

    property string currentFile: root.configuration.Media
    property int fadeDuration: root.configuration.FadeDuration

    // The layer currently being shown, and the one which is hidden and
    // available to load the next wallpaper into.
    property bool frontIsFirst: true
    readonly property Item frontLayer: frontIsFirst ? layer1 : layer2
    readonly property Item backLayer: frontIsFirst ? layer2 : layer1

    // Set while a new wallpaper is being loaded into the back layer.
    property bool switching: false

    // Plasma holds up the desktop startup until the wallpaper reports
    // that it has finished loading.
    property bool firstShown: false
    loading: !firstShown

    Rectangle {
        id: background
        anchors.fill: parent
        color: root.configuration.BackgroundColor
        z: -1
    }

    MediaLayer {
        id: layer1
        anchors.fill: parent
        fillMode: root.configuration.FillMode
        muted: root.configuration.Muted
        volume: root.configuration.Volume
        opacity: 1
        z: 1
        Behavior on opacity {
            enabled: layer1.fadeEnabled
            NumberAnimation {
                duration: root.fadeDuration
                easing.type: Easing.InOutQuad
            }
        }
    }

    MediaLayer {
        id: layer2
        anchors.fill: parent
        fillMode: root.configuration.FillMode
        muted: root.configuration.Muted
        volume: root.configuration.Volume
        opacity: 1
        z: 0
        Behavior on opacity {
            enabled: layer2.fadeEnabled
            NumberAnimation {
                duration: root.fadeDuration
                easing.type: Easing.InOutQuad
            }
        }
    }

    // Only the layer which can be seen, or the one which is about to
    // appear, needs to be playing.  There is no point in decoding a
    // video which nobody can see.
    function updateActive() {
        // A video only becomes ready to be shown by playing, so the
        // layer being changed to must always be allowed to play even if
        // the desktop cannot be seen at the moment.
        layer1.active = (layer1 === root.frontLayer && root.visible) || (root.switching && layer1 === root.backLayer);
        layer2.active = (layer2 === root.frontLayer && root.visible) || (root.switching && layer2 === root.backLayer);
    }

    // Load a new wallpaper into the hidden layer.  Nothing visible
    // happens until it reports that it is ready, see checkReady() below.
    function startSwitch(file) {
        if (file === "") return;
        if (root.frontLayer.source === file)
        {
            // Already showing this wallpaper.  This can happen when the
            // virtual desktop changes to another one using the same
            // wallpaper, including if that happens while a switch to a
            // different wallpaper is still in progress - in which case
            // the switch is abandoned.
            if (root.switching)
            {
                readyTimeout.stop();
                root.switching = false;
                updateActive();
            }
            return;
        }

        const back = root.backLayer;
        // Make sure that the incoming layer is below the outgoing one
        // and fully opaque, so that it is completely hidden until the
        // outgoing layer fades away to reveal it.
        back.z = 0;
        root.frontLayer.z = 1;

        // Make the incoming layer opaque immediately, with no animation.
        // It is hidden underneath the outgoing layer so this cannot be
        // seen, and it must be fully opaque before that layer starts to
        // fade away or the background would show through in between.
        back.fadeEnabled = false;
        back.opacity = 1;
        back.fadeEnabled = true;

        // Note that the switch must be recorded as being in progress
        // before the source is set, because setting it may make the
        // layer ready immediately - an image which is already in the
        // cache does not need to be loaded again.  Then check whether
        // that has already happened, because in that case there will be
        // no change of the layer state to notice later.
        root.switching = true;
        back.source = file;

        updateActive();
        readyTimeout.restart();
        checkReady();
    }

    // Called when either layer becomes ready.
    function checkReady() {
        if (!root.switching) return;
        if (!root.backLayer.ready) return;
        commitSwitch();
    }

    // The new wallpaper is loaded and waiting underneath, so fade the
    // old one out to reveal it.
    function commitSwitch() {
        if (!root.switching) return;
        readyTimeout.stop();
        root.switching = false;

        if (!root.firstShown)
        {
            // The first wallpaper has nothing to fade from.  The front
            // layer starts out empty, and therefore transparent, so
            // fading it away is not noticeable in any case.
            root.frontLayer.opacity = 0;
            root.frontIsFirst = !root.frontIsFirst;
            root.firstShown = true;
            updateActive();
            return;
        }

        root.frontLayer.opacity = 0;			// animated, reveals the back layer
        root.frontIsFirst = !root.frontIsFirst;

        // The wallpaper which has just been faded away from is not
        // released.  Keeping it loaded means that changing back to it,
        // as happens when moving between two virtual desktops, does not
        // have to load it again and can be done immediately.  It stays
        // until the layer is needed for a different wallpaper.
        //
        // A video which is being faded away from does have to be paused
        // eventually, but not until the fade has finished:  a frozen
        // frame fading away would be obvious.
        settleTimer.restart();
    }

    Timer {
        id: settleTimer
        interval: Math.max(root.fadeDuration, 0)+50
        onTriggered: updateActive()
    }

    // If the new wallpaper cannot be loaded at all - a missing file, or
    // a video in a format which cannot be played - then do not wait for
    // it for ever.  Abandon the change and stay on the wallpaper which
    // is currently being shown:  that is not what was asked for, but it
    // is much better than fading to an empty layer and leaving the
    // desktop blank, which is exactly what this plugin exists to avoid.
    Timer {
        id: readyTimeout
        interval: 5000
        onTriggered: {
            console.warn("wallpaperswitch: timed out loading", root.backLayer.source);
            root.switching = false;
            // On startup there is nothing being shown to stay on, but
            // Plasma must still be told that the wallpaper is no longer
            // loading, otherwise it keeps waiting for it.
            root.firstShown = true;
            updateActive();
        }
    }

    onCurrentFileChanged: startSwitch(root.currentFile)
    onVisibleChanged: updateActive()

    Connections {
        target: layer1
        function onReadyChanged() { root.checkReady(); }
    }

    Connections {
        target: layer2
        function onReadyChanged() { root.checkReady(); }
    }

    Component.onCompleted: {
        if (root.currentFile === "") root.firstShown = true;
        else startSwitch(root.currentFile);
    }

    // Shown only if there is no wallpaper configured at all, which in
    // normal use means that the switcher has not set one yet.
    Text {
        anchors.centerIn: parent
        z: 10
        visible: root.currentFile === ""
        color: Kirigami.Theme.textColor
        text: "No wallpaper image or video has been set"
    }
}
