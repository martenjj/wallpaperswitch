/*
 * Keeps track of the windows on the current virtual desktop, so that
 * playing the video and blurring the wallpaper can depend on whether
 * there is anything covering it.
 *
 * The approach, and the way that the task manager model has to be asked
 * about the windows, follows TasksModel.qml of the Smart Video Wallpaper
 * Reborn plugin, which is also GPL 2 or later:
 * https://github.com/luisbocanegra/plasma-smart-video-wallpaper-reborn
 * Copyright 2018 Rog131, 2019 adhe, 2024 Luis Bocanegra
 *
 * Copyright (c) 2026 Jonathan Marten <jjm@keelhaul.me.uk>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import org.kde.taskmanager 0.1 as TaskManager

Item {
    id: root

    // The screen that this wallpaper is on.  If it is not known then
    // the windows on all screens are considered.
    property var screenGeometry: null
    property bool activeScreenOnly: true

    // Is there a window which is maximised or full screen, is there one
    // which has the focus, and is there any window visible at all?
    property bool maximisedExists: false
    property bool activeExists: false
    property bool visibleExists: false

    readonly property var tasks: TaskManager.AbstractTasksModel

    TaskManager.VirtualDesktopInfo {
        id: virtualDesktopInfo
    }

    TaskManager.ActivityInfo {
        id: activityInfo
    }

    TaskManager.TasksModel {
        id: model
        sortMode: TaskManager.TasksModel.SortVirtualDesktop
        groupMode: TaskManager.TasksModel.GroupDisabled
        virtualDesktop: virtualDesktopInfo.currentDesktop
        activity: activityInfo.currentActivity
        screenGeometry: root.screenGeometry
        filterByVirtualDesktop: true
        filterByScreen: root.activeScreenOnly && root.screenGeometry!==null
        filterByActivity: true
        filterMinimized: true

        // The model is updated in stages as windows appear and change,
        // so do not look at it until it has settled.
        onDataChanged: Qt.callLater(root.update)
        onCountChanged: Qt.callLater(root.update)
    }

    function update() {
        let maximised = 0;
        let active = 0;
        let visible = 0;

        for (var i = 0; i<model.count; ++i)
        {
            const task = model.index(i, 0);
            if (task===undefined) continue;
            if (!model.data(task, root.tasks.IsWindow)) continue;
            if (model.data(task, root.tasks.IsMinimized)) continue;

            ++visible;
            if (model.data(task, root.tasks.IsMaximized) || model.data(task, root.tasks.IsFullScreen)) ++maximised;
            if (model.data(task, root.tasks.IsActive)) ++active;
        }

        root.maximisedExists = (maximised>0);
        root.activeExists = (active>0);
        root.visibleExists = (visible>0);
    }

    // Whether the condition selected by one of the "when windows are"
    // settings is true at the moment.  The values are those written by
    // the wallpaper switcher application.
    function conditionMet(condition) {
        switch (condition)
        {
case "maximised":	return (root.maximisedExists);
case "active":		return (root.activeExists);
case "visible":		return (root.visibleExists);
case "always":		return (true);
default:		return (false);		// including "never"
        }
    }

    Component.onCompleted: update()
}
