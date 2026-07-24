/*
 * Wallpaper plugin showing either a still image or a video, with an
 * animated transition between them when the wallpaper changes.
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
 * into the hidden one, and the transition only starts once that layer
 * reports that it has something to show.
 *
 * Only the layer being changed away from is ever animated.  It is the
 * one on top, and the layer appearing from underneath it stays fully
 * opaque and still throughout.  Animating both at the same time would
 * let the background show through in the middle of the transition,
 * which is exactly what this plugin exists to avoid.
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
    property int transitionDuration: root.configuration.FadeDuration
    property string transitionType: root.configuration.Transition

    // A layer sliding away moves outside the wallpaper area, and must
    // not be seen doing so.
    clip: true

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

    // The layers are not anchored to fill the wallpaper, because an
    // item positioned by anchors cannot be moved and so could not be
    // slid away by a transition.

    MediaLayer {
        id: layer1
        x: 0
        y: 0
        width: root.width
        height: root.height
        fillMode: root.configuration.FillMode
        muted: root.configuration.Muted
        volume: root.configuration.Volume
        opacity: 1
        z: 1
    }

    MediaLayer {
        id: layer2
        x: 0
        y: 0
        width: root.width
        height: root.height
        fillMode: root.configuration.FillMode
        muted: root.configuration.Muted
        volume: root.configuration.Volume
        opacity: 1
        z: 0
    }

    // The transition, which always applies to the layer being changed
    // away from.  Everything that any of the transitions can do is set
    // up here;  whatever a particular transition does not do is simply
    // animated to the value that it already has.
    //
    // There is deliberately no Behavior on the opacity of the layers.
    // Every change of it, animated or not, is made here, so that an
    // animation and a Behavior can never end up fighting over it.
    ParallelAnimation {
        id: transition
        property Item layer: null
        property int duration: root.transitionDuration

        NumberAnimation {
            id: animOpacity
            target: transition.layer
            property: "opacity"
            duration: transition.duration
            easing.type: Easing.InOutQuad
        }

        NumberAnimation {
            id: animX
            target: transition.layer
            property: "x"
            duration: transition.duration
            easing.type: Easing.InOutQuad
        }

        NumberAnimation {
            id: animY
            target: transition.layer
            property: "y"
            duration: transition.duration
            easing.type: Easing.InOutQuad
        }

        NumberAnimation {
            id: animScale
            target: transition.layer
            property: "scale"
            duration: transition.duration
            easing.type: Easing.InOutQuad
        }
    }

    // Only the layer which can be seen, or the one which is about to
    // appear, needs to be playing.  There is no point in decoding a
    // video which nobody can see.
    function updateActive() {
        // A video only becomes ready to be shown by playing, so the
        // layer being changed to must always be allowed to play even if
        // the desktop cannot be seen at the moment.
        layer1.active = (layer1===root.frontLayer && root.visible) || (root.switching && layer1===root.backLayer);
        layer2.active = (layer2===root.frontLayer && root.visible) || (root.switching && layer2===root.backLayer);
    }

    // Load a new wallpaper into the hidden layer.  Nothing visible
    // happens until it reports that it is ready, see checkReady() below.
    function startSwitch(file) {
        if (file==="") return;
        if (root.frontLayer.source===file)
        {
            // Already showing this wallpaper.  This can happen when the
            // virtual desktop changes to another one using the same
            // wallpaper, including if that happens while a change to a
            // different wallpaper is still in progress - in which case
            // the change is abandoned.
            if (root.switching)
            {
                readyTimeout.stop();
                root.switching = false;
                updateActive();
            }
            return;
        }

        const back = root.backLayer;

        // Any transition still running applies to this layer, because it
        // is the one which was last changed away from.  Stop it and put
        // the layer back to where a layer which is about to appear needs
        // to be:  below the outgoing one, in place, unscaled and fully
        // opaque.  None of this is animated, and none of it can be seen,
        // because the outgoing layer is on top and still opaque.
        transition.stop();
        back.z = 0;
        root.frontLayer.z = 1;
        back.x = 0;
        back.y = 0;
        back.scale = 1;
        back.opacity = 1;

        // Note that the change must be recorded as being in progress
        // before the source is set, because setting it may make the
        // layer ready immediately - a wallpaper which is still loaded
        // from last time does not need to be loaded again.  Then check
        // whether that has already happened, because in that case there
        // will be no change of the layer state to notice later.
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

    // The new wallpaper is loaded and waiting underneath, so animate the
    // old one away to reveal it.
    function commitSwitch() {
        if (!root.switching) return;
        readyTimeout.stop();
        root.switching = false;

        const out = root.frontLayer;
        root.frontIsFirst = !root.frontIsFirst;

        if (!root.firstShown)
        {
            // The first wallpaper has nothing to change from.  The front
            // layer starts out empty, and therefore transparent, so
            // there is nothing to animate away.
            out.opacity = 0;
            root.firstShown = true;
            updateActive();
            return;
        }

        // Set up the transition.  Anything which the sort of transition
        // wanted does not do is left as it is.
        animOpacity.to = 0;
        animX.to = 0;
        animY.to = 0;
        animScale.to = 1;
        transition.duration = root.transitionDuration;

        switch (root.transitionType)
        {
case "none":						// changes over at once
            transition.duration = 0;
            break;

case "slideleft":					// slides away to the left
            animX.to = -root.width;
            animOpacity.to = 1;
            break;

case "slideright":					// and so on
            animX.to = root.width;
            animOpacity.to = 1;
            break;

case "slideup":
            animY.to = -root.height;
            animOpacity.to = 1;
            break;

case "slidedown":
            animY.to = root.height;
            animOpacity.to = 1;
            break;

case "zoomin":						// grows and fades away
            animScale.to = 1.25;
            break;

case "zoomout":						// shrinks and fades away
            animScale.to = 0.75;
            break;

case "fade":
default:						// simply fades away
            break;
        }

        transition.layer = out;
        transition.start();

        // The wallpaper which has just been changed away from is not
        // released.  Keeping it loaded means that changing back to it,
        // as happens when moving between two virtual desktops, does not
        // have to load it again and can be done immediately.  It stays
        // until the layer is needed for a different wallpaper.
        //
        // A video which is being changed away from does have to be
        // paused eventually, but not until the transition has finished:
        // a frozen frame sliding away would be obvious.
        settleTimer.restart();
    }

    Timer {
        id: settleTimer
        interval: Math.max(root.transitionDuration, 0)+50
        onTriggered: updateActive()
    }

    // If the new wallpaper cannot be loaded at all - a missing file, or
    // a video in a format which cannot be played - then do not wait for
    // it for ever.  Abandon the change and stay on the wallpaper which
    // is currently being shown:  that is not what was asked for, but it
    // is much better than animating away to an empty layer and leaving
    // the desktop blank, which is exactly what this plugin exists to
    // avoid.
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
        if (root.currentFile==="") root.firstShown = true;
        else startSwitch(root.currentFile);
    }

    // Shown only if there is no wallpaper configured at all, which in
    // normal use means that the switcher has not set one yet.
    Text {
        anchors.centerIn: parent
        z: 10
        visible: root.currentFile===""
        color: Kirigami.Theme.textColor
        text: "No wallpaper image or video has been set"
    }
}
