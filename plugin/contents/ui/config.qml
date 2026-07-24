/*
 * Configuration page for the wallpaper plugin.
 *
 * The wallpaper file itself is not set here - it is set for each virtual
 * desktop by the wallpaper switcher application.  Only the settings
 * which apply to however the wallpaper is shown appear here.
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

    property string cfg_Media
    property int cfg_FillMode
    property color cfg_BackgroundColor
    property int cfg_FadeDuration			// set by the application
    property string cfg_Transition			// set by the application
    property bool cfg_Muted
    property real cfg_Volume

    // Defaults, needed by the "Defaults" button of the settings dialogue.
    property int cfg_FillModeDefault: 2
    property color cfg_BackgroundColorDefault: "black"
    property int cfg_FadeDurationDefault: 350
    property string cfg_TransitionDefault: "fade"
    property bool cfg_MutedDefault: true
    property real cfg_VolumeDefault: 1.0
    property string cfg_MediaDefault: ""

    Kirigami.FormLayout {
        Layout.fillWidth: true

        QQC2.ComboBox {
            Kirigami.FormData.label: i18nd("plasma_wallpaper_uk.me.keelhaul.wallpaperswitch.media", "Positioning:")
            model: [
                { label: i18nd("plasma_wallpaper_uk.me.keelhaul.wallpaperswitch.media", "Scaled and cropped"), value: 2 },
                { label: i18nd("plasma_wallpaper_uk.me.keelhaul.wallpaperswitch.media", "Scaled, keep proportions"), value: 1 },
                { label: i18nd("plasma_wallpaper_uk.me.keelhaul.wallpaperswitch.media", "Scaled and stretched"), value: 0 },
                { label: i18nd("plasma_wallpaper_uk.me.keelhaul.wallpaperswitch.media", "Centred"), value: 6 },
                { label: i18nd("plasma_wallpaper_uk.me.keelhaul.wallpaperswitch.media", "Tiled"), value: 3 }
            ]
            textRole: "label"
            valueRole: "value"
            onActivated: root.cfg_FillMode = currentValue
            Component.onCompleted: currentIndex = indexOfValue(root.cfg_FillMode)
        }

        QQC2.Button {
            id: colourButton
            Kirigami.FormData.label: i18nd("plasma_wallpaper_uk.me.keelhaul.wallpaperswitch.media", "Background colour:")
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

        Item {
            Kirigami.FormData.isSection: true
        }

        QQC2.CheckBox {
            Kirigami.FormData.label: i18nd("plasma_wallpaper_uk.me.keelhaul.wallpaperswitch.media", "Video sound:")
            text: i18nd("plasma_wallpaper_uk.me.keelhaul.wallpaperswitch.media", "Muted")
            checked: root.cfg_Muted
            onToggled: root.cfg_Muted = checked
        }

        QQC2.Slider {
            Kirigami.FormData.label: i18nd("plasma_wallpaper_uk.me.keelhaul.wallpaperswitch.media", "Volume:")
            enabled: !root.cfg_Muted
            from: 0
            to: 1
            value: root.cfg_Volume
            onMoved: root.cfg_Volume = value
        }

        Item {
            Kirigami.FormData.isSection: true
        }

        QQC2.Label {
            Layout.maximumWidth: Kirigami.Units.gridUnit*20
            wrapMode: Text.WordWrap
            text: i18nd("plasma_wallpaper_uk.me.keelhaul.wallpaperswitch.media",
                        "The wallpaper image or video is set for each virtual desktop by the Wallpaper Switcher application, as is the transition used when it changes.")
        }

        QQC2.Label {
            Layout.maximumWidth: Kirigami.Units.gridUnit*20
            wrapMode: Text.WordWrap
            visible: root.cfg_Media !== ""
            text: i18nd("plasma_wallpaper_uk.me.keelhaul.wallpaperswitch.media",
                        "Currently showing: %1", root.cfg_Media)
        }
    }

    QtDialogs.ColorDialog {
        id: colourDialog
        selectedColor: root.cfg_BackgroundColor
        onAccepted: root.cfg_BackgroundColor = selectedColor
    }
}
