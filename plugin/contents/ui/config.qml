/*
 * Configuration page for the wallpaper plugin.
 *
 * The wallpaper file itself is not set here - it is set for each virtual
 * desktop by the wallpaper switcher application, as is the transition
 * used when it changes.  Everything else about how the wallpaper is
 * shown is set here.
 *
 * Copyright (c) 2026 Jonathan Marten <jjm@keelhaul.me.uk>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Dialogs as QtDialogs
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

ColumnLayout {
    id: root

    readonly property string dom: "plasma_wallpaper_uk.me.keelhaul.wallpaperswitch.media"

    property string cfg_Media
    property int cfg_FillMode
    property color cfg_BackgroundColor
    property int cfg_FadeDuration			// set by the application
    property string cfg_Transition			// set by the application
    property bool cfg_Muted
    property real cfg_Volume
    property string cfg_PauseMode
    property string cfg_BlurMode
    property int cfg_BlurRadius
    property int cfg_BlurDuration
    property string cfg_AlternativeSpeedMode
    property real cfg_AlternativeSpeed
    property int cfg_BatteryLevel
    property bool cfg_BatteryPauses
    property bool cfg_BatteryDisablesBlur

    // Defaults, needed by the "Defaults" button of the settings dialogue.
    property string cfg_MediaDefault: ""
    property int cfg_FillModeDefault: 2
    property color cfg_BackgroundColorDefault: "black"
    property int cfg_FadeDurationDefault: 350
    property string cfg_TransitionDefault: "fade"
    property bool cfg_MutedDefault: true
    property real cfg_VolumeDefault: 1.0
    property string cfg_PauseModeDefault: "maximised"
    property string cfg_BlurModeDefault: "never"
    property int cfg_BlurRadiusDefault: 32
    property int cfg_BlurDurationDefault: 300
    property string cfg_AlternativeSpeedModeDefault: "never"
    property real cfg_AlternativeSpeedDefault: 0.5
    property int cfg_BatteryLevelDefault: 10
    property bool cfg_BatteryPausesDefault: true
    property bool cfg_BatteryDisablesBlurDefault: false

    // The window conditions which the pausing, the blurring and the
    // alternative speed can depend on.  These are the values handled by
    // conditionMet() in WindowModel.qml.  The blur has two additional
    // conditions of its own.
    readonly property var windowConditions: [
        { label: i18nd(root.dom, "Maximised or full-screen windows"), value: "maximised" },
        { label: i18nd(root.dom, "Active window"), value: "active" },
        { label: i18nd(root.dom, "At least one window is visible"), value: "visible" },
        { label: i18nd(root.dom, "Never"), value: "never" }
    ]

    readonly property var blurConditions: [
        { label: i18nd(root.dom, "Maximised or full-screen windows"), value: "maximised" },
        { label: i18nd(root.dom, "Active window"), value: "active" },
        { label: i18nd(root.dom, "At least one window is visible"), value: "visible" },
        { label: i18nd(root.dom, "Video is paused"), value: "paused" },
        { label: i18nd(root.dom, "Always"), value: "always" },
        { label: i18nd(root.dom, "Never"), value: "never" }
    ]

    Kirigami.FormLayout {
        Layout.fillWidth: true

        //////////////////////////////////////////////////////////////////
        //  How the wallpaper is shown				 	//
        //////////////////////////////////////////////////////////////////

        QQC2.ComboBox {
            Kirigami.FormData.label: i18nd(root.dom, "Positioning:")
            model: [
                { label: i18nd(root.dom, "Scaled and cropped"), value: 2 },
                { label: i18nd(root.dom, "Scaled, keep proportions"), value: 1 },
                { label: i18nd(root.dom, "Scaled and stretched"), value: 0 },
                { label: i18nd(root.dom, "Centred"), value: 6 },
                { label: i18nd(root.dom, "Tiled"), value: 3 }
            ]
            textRole: "label"
            valueRole: "value"
            onActivated: root.cfg_FillMode = currentValue
            Component.onCompleted: currentIndex = indexOfValue(root.cfg_FillMode)
        }

        QQC2.Button {
            Kirigami.FormData.label: i18nd(root.dom, "Background colour:")
            implicitWidth: Kirigami.Units.gridUnit*4
            onClicked: colourDialog.open()

            Rectangle {
                anchors.fill: parent
                anchors.margins: Kirigami.Units.smallSpacing
                color: root.cfg_BackgroundColor
                border.color: Kirigami.Theme.textColor
                border.width: 1
            }
        }

        Item { Kirigami.FormData.isSection: true }

        //////////////////////////////////////////////////////////////////
        //  Pausing the video					 	//
        //////////////////////////////////////////////////////////////////

        QQC2.ComboBox {
            id: pauseCombo
            Kirigami.FormData.label: i18nd(root.dom, "Pause video for:")
            model: root.windowConditions
            textRole: "label"
            valueRole: "value"
            onActivated: root.cfg_PauseMode = currentValue
            Component.onCompleted: currentIndex = Math.max(indexOfValue(root.cfg_PauseMode), 0)
        }

        QQC2.ComboBox {
            id: speedCombo
            Kirigami.FormData.label: i18nd(root.dom, "Play slowly for:")
            model: root.windowConditions
            textRole: "label"
            valueRole: "value"
            onActivated: root.cfg_AlternativeSpeedMode = currentValue
            Component.onCompleted: currentIndex = Math.max(indexOfValue(root.cfg_AlternativeSpeedMode), 0)
        }

        QQC2.SpinBox {
            Kirigami.FormData.label: i18nd(root.dom, "Slow speed:")
            enabled: root.cfg_AlternativeSpeedMode!=="never"
            from: 5
            to: 200
            stepSize: 5
            value: Math.round(root.cfg_AlternativeSpeed*100)
            onValueModified: root.cfg_AlternativeSpeed = value/100
            textFromValue: (value, locale) => value+"%"
            valueFromText: (text, locale) => parseInt(text)
        }

        Item { Kirigami.FormData.isSection: true }

        //////////////////////////////////////////////////////////////////
        //  Blurring the wallpaper				 	//
        //////////////////////////////////////////////////////////////////

        QQC2.ComboBox {
            id: blurCombo
            Kirigami.FormData.label: i18nd(root.dom, "Blur wallpaper for:")
            model: root.blurConditions
            textRole: "label"
            valueRole: "value"
            onActivated: root.cfg_BlurMode = currentValue
            Component.onCompleted: currentIndex = Math.max(indexOfValue(root.cfg_BlurMode), 0)
        }

        QQC2.SpinBox {
            Kirigami.FormData.label: i18nd(root.dom, "Blur radius:")
            enabled: root.cfg_BlurMode!=="never"
            from: 1
            to: 64
            stepSize: 1
            value: root.cfg_BlurRadius
            onValueModified: root.cfg_BlurRadius = value
            textFromValue: (value, locale) => value+"px"
            valueFromText: (text, locale) => parseInt(text)
        }

        QQC2.SpinBox {
            Kirigami.FormData.label: i18nd(root.dom, "Blur duration:")
            enabled: root.cfg_BlurMode!=="never"
            from: 0
            to: 5000
            stepSize: 50
            value: root.cfg_BlurDuration
            onValueModified: root.cfg_BlurDuration = value
            textFromValue: (value, locale) => value+" ms"
            valueFromText: (text, locale) => parseInt(text)
        }

        Item { Kirigami.FormData.isSection: true }

        //////////////////////////////////////////////////////////////////
        //  On battery						 	//
        //////////////////////////////////////////////////////////////////

        QQC2.SpinBox {
            Kirigami.FormData.label: i18nd(root.dom, "On battery below:")
            from: 0
            to: 100
            stepSize: 5
            value: root.cfg_BatteryLevel
            onValueModified: root.cfg_BatteryLevel = value
            textFromValue: (value, locale) => value+"%"
            valueFromText: (text, locale) => parseInt(text)
        }

        QQC2.CheckBox {
            text: i18nd(root.dom, "Pause video")
            checked: root.cfg_BatteryPauses
            onToggled: root.cfg_BatteryPauses = checked
        }

        QQC2.CheckBox {
            text: i18nd(root.dom, "Disable blur")
            checked: root.cfg_BatteryDisablesBlur
            onToggled: root.cfg_BatteryDisablesBlur = checked
        }

        Item { Kirigami.FormData.isSection: true }

        //////////////////////////////////////////////////////////////////
        //  Sound						 	//
        //////////////////////////////////////////////////////////////////

        QQC2.CheckBox {
            Kirigami.FormData.label: i18nd(root.dom, "Video sound:")
            text: i18nd(root.dom, "Muted")
            checked: root.cfg_Muted
            onToggled: root.cfg_Muted = checked
        }

        QQC2.Slider {
            Kirigami.FormData.label: i18nd(root.dom, "Volume:")
            enabled: !root.cfg_Muted
            from: 0
            to: 1
            value: root.cfg_Volume
            onMoved: root.cfg_Volume = value
        }

        Item { Kirigami.FormData.isSection: true }

        QQC2.Label {
            Layout.maximumWidth: Kirigami.Units.gridUnit*20
            wrapMode: Text.WordWrap
            text: i18nd(root.dom, "The wallpaper image or video is set for each virtual desktop by the Wallpaper Switcher application, as is the transition used when it changes.")
        }

        QQC2.Label {
            Layout.maximumWidth: Kirigami.Units.gridUnit*20
            wrapMode: Text.WordWrap
            visible: root.cfg_Media!==""
            text: i18nd(root.dom, "Currently showing: %1", root.cfg_Media)
        }
    }

    QtDialogs.ColorDialog {
        id: colourDialog
        selectedColor: root.cfg_BackgroundColor
        onAccepted: root.cfg_BackgroundColor = selectedColor
    }
}
