//////////////////////////////////////////////////////////////////////////
//									//
//  Project:	Plasma 6 Wallpaper Switcher				//
//  Edit:	24-Jul-26						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Copyright (c) 2015-2025 Jonathan Marten <jjm@keelhaul.me.uk>	//
//  Home and download page:  http://www.keelhaul.demon.co.uk/TBD/	//
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
//  details.  If not, write to the Free Software Foundation, Inc.,	//
//  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.		//
//									//
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "preferencespage.h"

#include <qcheckbox.h>
#include <qlabel.h>
#include <qgridlayout.h>
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qimagereader.h>
#include <qtreewidget.h>
#include <qtreewidgetitemiterator.h>
#include <qwhatsthis.h>
#include <qurl.h>
#include <qmimedatabase.h>
#include <qmimetype.h>
#include <qguiapplication.h>
#include <qheaderview.h>
#include <qfileinfo.h>

#include <klocalizedstring.h>
#include <kconfigskeleton.h>
#include <krecentdirs.h>
#include <kfileitem.h>
#include <kio/previewjob.h>

#include "settings.h"
#include "wallpaperswitcher.h"
#include "wallpaperimagesetter.h"
#include "switcherinterface.h"
#include "libwallpaper_logging.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Static variables							//
//									//
//////////////////////////////////////////////////////////////////////////

static bool sIsStandalone = true;			// this is a standalone application

//////////////////////////////////////////////////////////////////////////
//									//
//  PreferencesWallpaperPage -- Settings for the wallpaper switcher	//
//									//
//////////////////////////////////////////////////////////////////////////

PreferencesWallpaperPage::PreferencesWallpaperPage(QWidget *pnt)
    : PreferencesPage(pnt)
{
    QGridLayout *gl = new QGridLayout(this);
    setLayout(gl);
    int row = 0;

    KConfigSkeletonItem *ski = Settings::self()->autoStartItem();
    Q_ASSERT(ski!=nullptr);
    mAutoStartCheck = new QCheckBox(ski->label(), this);
    mAutoStartCheck->setToolTip(ski->toolTip());
    gl->addWidget(mAutoStartCheck, row, 0, 1, -1, Qt::AlignLeft);
    ++row;
    if (!sIsStandalone) mAutoStartCheck->setHidden(true);

    ski = Settings::self()->enableSwitcherItem();
    Q_ASSERT(ski!=nullptr);
    mEnableSwitcherCheck = new QCheckBox(ski->label(), this);
    mEnableSwitcherCheck->setToolTip(ski->toolTip());
    connect(mEnableSwitcherCheck, &QAbstractButton::toggled, this, &PreferencesWallpaperPage::slotUpdateButtonStates);
    gl->addWidget(mEnableSwitcherCheck, row, 0, 1, -1, Qt::AlignLeft);
    ++row;

    ski = Settings::self()->showPopupMessageItem();
    Q_ASSERT(ski!=nullptr);
    mShowPopupCheck = new QCheckBox(ski->label(), this);
    mShowPopupCheck->setToolTip(ski->toolTip());
    gl->addWidget(mShowPopupCheck, row, 0, 1, -1, Qt::AlignLeft);
    ++row;

    mWallpaperList = new QTreeWidget(this);
    mWallpaperList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mWallpaperList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    mWallpaperList->setUniformRowHeights(true);
    mWallpaperList->setSortingEnabled(false);
    mWallpaperList->setSelectionMode(QAbstractItemView::SingleSelection);
    mWallpaperList->setSelectionBehavior(QAbstractItemView::SelectRows);
    mWallpaperList->setRootIsDecorated(false);
    mWallpaperList->setIconSize(QSize(32, 32));
    mWallpaperList->setTextElideMode(Qt::ElideMiddle);

    QStringList headers;
    headers << i18n("Desktop") << i18n("Wallpaper");
    mWallpaperList->setHeaderLabels(headers);
    mWallpaperList->setColumnCount(2);

    connect(mWallpaperList, &QTreeWidget::itemSelectionChanged, this, &PreferencesWallpaperPage::slotUpdateButtonStates);
    connect(mWallpaperList, &QTreeWidget::itemDoubleClicked, this, &PreferencesWallpaperPage::slotSetWallpaper);
    gl->addWidget(mWallpaperList, row, 0, 1, -1);
    gl->setRowStretch(row, 1);
    ++row;

    mSetWallpaperButton = new QPushButton(this);
    mSetWallpaperButton->setText(i18nc("@action:button", "Set Wallpaper..."));
    mSetWallpaperButton->setIcon(QIcon::fromTheme("view-catalog"));

    connect(mSetWallpaperButton, &QAbstractButton::clicked, this, [this]() { slotSetWallpaper(nullptr); });
    gl->addWidget(mSetWallpaperButton, row, 0, Qt::AlignLeft);

    QLabel *helpLabel = new QLabel(i18n("<qt>Help for <a href=\"settingshelp\">desktop settings</a>"));
    connect(helpLabel, &QLabel::linkActivated, this, &PreferencesWallpaperPage::slotInfoLinkActivated);
    gl->addWidget(helpLabel, row, 1, Qt::AlignRight);
    ++row;
}


void PreferencesWallpaperPage::slotInfoLinkActivated(const QString &url)
{
    if (url!="settingshelp") return;
    QWhatsThis::showText(QCursor::pos(),
                         xi18nc("@info:tooltip",
                                "To use the desktop wallpaper switcher, open either the "
                                "<interface>Desktop and Wallpaper</interface>&nbsp;- "
                                "<interface>Wallpaper</interface> settings page from "
                                "the desktop context menu, or the "
                                "<interface>Appearance&nbsp;&amp; Style</interface>&nbsp;- "
                                "<interface>Wallpaper</interface> page in the "
                                "<application>System Settings</application> application. "
                                "Set the <interface>Wallpaper type</interface> "
                                "to <resource>Image</resource> and the "
                                "<interface>Positioning</interface> as required."
                                "<nl/><nl/>"
                                "A video file (for example MP4 or MKV) can also be used as "
                                "the wallpaper. This needs the "
                                "<application>Smart Video Wallpaper Reborn</application> plugin "
                                "to be installed, using the <interface>Get New Plugins</interface> "
                                "button on that same settings page. The wallpaper type is then "
                                "switched automatically between <resource>Image</resource> and "
                                "<resource>Smart Video Wallpaper Reborn</resource> as required "
                                "by the wallpaper file selected for the virtual desktop."));
}


static void setItemImageFile(QTreeWidgetItem *item, const QString &file)
{
    item->setText(1, file);
    if (file.isEmpty())
    {
        item->setIcon(1, QIcon());
        return;
    }

    // A still image can be shown as a thumbnail of itself, but a video
    // cannot.  Use a generic icon for a video for now;  it is replaced by
    // a thumbnail of the first frame if one can be generated, see
    // PreferencesWallpaperPage::requestPreviews() below.
    if (WallpaperImageSetter::isVideoFile(file)) item->setIcon(1, QIcon::fromTheme("video-x-generic"));
    else item->setIcon(1, QIcon(file));
}


void PreferencesWallpaperPage::requestPreviews(const QStringList &files)
{
    if (files.isEmpty()) return;

    KFileItemList items;
    for (const QString &file : files) items.append(KFileItem(QUrl::fromLocalFile(file)));

    // Generating the thumbnail of a video needs a suitable thumbnailer
    // (for example "ffmpegthumbs") to be installed.  If there is none
    // then the job simply reports a failure and the generic icon set
    // above remains.
    KIO::PreviewJob *job = KIO::filePreview(items, mWallpaperList->iconSize()*2);
    if (job==nullptr) return;

    // Video files can be large, but the size limit which applies by
    // default is not relevant for a local file.
    job->setIgnoreMaximumSize(true);
    connect(job, &KIO::PreviewJob::gotPreview, this, &PreferencesWallpaperPage::slotGotPreview);
    mPreviewJob = job;
}


void PreferencesWallpaperPage::slotGotPreview(const KFileItem &item, const QPixmap &preview)
{
    const QString file = item.url().toLocalFile();
    qCDebug(DEBUGCAT) << "for" << file;
    if (file.isEmpty() || preview.isNull()) return;

    // The same file may be used for more than one desktop or screen, so
    // update every list entry that refers to it.  Searching the list here,
    // as opposed to remembering the items when the preview was requested,
    // means that there is no problem if the list has been rebuilt in the
    // meantime.
    const QIcon icon(preview);
    QTreeWidgetItemIterator it(mWallpaperList);
    while ((*it)!=nullptr)
    {
        QTreeWidgetItem *listItem = (*it);
        if (listItem->text(1)==file) listItem->setIcon(1, icon);
        ++it;
    }
}


void PreferencesWallpaperPage::loadSettings()
{
    mEnableSwitcherCheck->setChecked(Settings::enableSwitcher());
    mAutoStartCheck->setChecked(Settings::autoStart());
    mShowPopupCheck->setChecked(Settings::showPopupMessage());

    KConfigSkeletonItem *ski = Settings::self()->wallpaperForDesktopItem();
    Q_ASSERT(ski!=nullptr);
    const KConfigGroup grp = Settings::self()->config()->group(ski->group());

    SwitcherInterface *si = SwitcherInterface::self();
    const int numDesktops = si->numberOfDesktops();

    int numScreens = QGuiApplication::screens().count();
    qCDebug(DEBUGCAT) << "screens current" << numScreens;

    const QRegularExpression rx("^"+WallpaperSwitcher::configKey("(\\d+)", "(\\d+)")+"$");
    const QStringList keys = grp.keyList();
    for (const QString &key : std::as_const(keys))
    {
        // Look for a desktop key that refers to a screen number.
        const QRegularExpressionMatch match = rx.match(key);
        if (match.hasMatch())
        {
            // If one was found, update the screen count to cover
            // that screen number, which must have been previously seen.
            const int screen = match.captured(2).toInt();
            if (screen>numScreens) numScreens = screen;
        }
    }
    qCDebug(DEBUGCAT) << "screens after config" << numScreens;

    if (numScreens<1)					// must have at least one
    {
        qWarning(DEBUGCAT) << "No screens found!";
        return;
    }

    bool isTree = false;
    QStringList videoFiles;				// needing a thumbnail

    if (mPreviewJob!=nullptr) mPreviewJob->kill();	// any previous previews
    mWallpaperList->clear();				// start with an empty list
    for (int i = 1; i<=numDesktops; ++i)
    {
        const QString name = si->desktopName(i);
        qCDebug(DEBUGCAT) << "desktop" << i << name;

        // A top level item for the desktop.
        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0, i18n("%1 - %2", i, name));
        item->setData(0, Qt::UserRole, i);		// desktop number of item
        mWallpaperList->addTopLevelItem(item);

        if (numScreens==1)
        {
            // If there is only one screen, then just show a flat list
            // listing the desktops.  Fall back to the legacy key if
            // there is no entry for the desktop and default screen.
            QString img = grp.readEntry(WallpaperSwitcher::configKey(i, 0), "");
            if (img.isEmpty()) img = grp.readEntry(WallpaperSwitcher::configKey(i), "");
            setItemImageFile(item, img);
            if (WallpaperImageSetter::isVideoFile(img) && !videoFiles.contains(img)) videoFiles.append(img);
        }
        else
        {
            // If there is more than one screen, then show sub-items for
            // each screen of that desktop.
            for (int j = 0; j<numScreens; ++j)
            {
                QTreeWidgetItem *item2 = new QTreeWidgetItem;
                item2->setText(0, i18n("Screen %1", j));
                item2->setData(0, Qt::UserRole, j);	// screen number of item

                // If there is no image configured for that combination of
                // screen and desktop, then use the image (if any) configured
                // for the desktop under the legacy key.
                QString img = grp.readEntry(WallpaperSwitcher::configKey(i, j), "");
                if (img.isEmpty()) img = grp.readEntry(WallpaperSwitcher::configKey(i), "");
                setItemImageFile(item2, img);
                if (WallpaperImageSetter::isVideoFile(img) && !videoFiles.contains(img)) videoFiles.append(img);
                item->addChild(item2);

                isTree = true;				// note needs to be expandable
            }
        }
    }

    requestPreviews(videoFiles);			// thumbnails for any videos

    mWallpaperList->setRootIsDecorated(isTree);
    mWallpaperList->expandToDepth(1);
    const QByteArray colStates = QByteArray::fromBase64(Settings::wallpaperColumnStates().toLatin1());
    if (!colStates.isEmpty()) mWallpaperList->header()->restoreState(colStates);

    slotUpdateButtonStates();
}


void PreferencesWallpaperPage::saveSettings()
{
    Settings::setEnableSwitcher(mEnableSwitcherCheck->isChecked());
    Settings::setAutoStart(mAutoStartCheck->isChecked());
    Settings::setShowPopupMessage(mShowPopupCheck->isChecked());

    KConfigSkeletonItem *ski = Settings::self()->wallpaperForDesktopItem();
    Q_ASSERT(ski!=nullptr);
    KConfigGroup grp = Settings::self()->config()->group(ski->group());

    for (int i = 0; i<mWallpaperList->topLevelItemCount(); ++i)
    {
        const QTreeWidgetItem *item = mWallpaperList->topLevelItem(i);
        grp.writeEntry(WallpaperSwitcher::configKey(i+1), (item->childCount()>0 ? "" : item->text(1)));

        for (int j = 0; j<item->childCount(); ++j)
        {
            const QTreeWidgetItem *item2 = item->child(j);
            grp.writeEntry(WallpaperSwitcher::configKey(i+1, j), item2->text(1));
        }
    }

    Settings::setWallpaperColumnStates(mWallpaperList->header()->saveState().toBase64());
    Settings::self()->save();
}


void PreferencesWallpaperPage::slotUpdateButtonStates()
{
    const bool enabled = mEnableSwitcherCheck->isChecked();

    mShowPopupCheck->setEnabled(enabled);
    mWallpaperList->setEnabled(enabled);
    mSetWallpaperButton->setEnabled(enabled && !mWallpaperList->selectedItems().isEmpty());
}


void PreferencesWallpaperPage::slotSetWallpaper(QTreeWidgetItem *item)
{
    if (item==nullptr)					// by button click
    {							// set for current selection
        QList<QTreeWidgetItem *> selItems = mWallpaperList->selectedItems();
        if (selItems.count()!=1) return;
        item = selItems.first();
    }

    const int num = item->data(0, Qt::UserRole).toInt();
    if (item->parent()!=nullptr) qCDebug(DEBUGCAT) << "for screen" << num;
    else qCDebug(DEBUGCAT) << "for desktop" << num;

    QList<QByteArray> imageFormats = QImageReader::supportedMimeTypes();
    //qCDebug(DEBUGCAT) << imageFormats;

    QMimeDatabase db;

    QStringList imageFilters;
    QStringList imagePatterns;
    for (const QByteArray &format : std::as_const(imageFormats))
    {
        if (format.isEmpty()) continue;

        const QMimeType mime = db.mimeTypeForName(format);
        if (!mime.isValid()) continue;
        imageFilters << mime.filterString();
        imagePatterns << mime.globPatterns();
    }

    // A video file can also be used as the wallpaper, if the video
    // wallpaper plugin is installed.  If it is not then there is no point
    // in offering videos for selection.
    QStringList videoFilters;
    QStringList videoPatterns;
    if (WallpaperImageSetter::videoPluginAvailable())
    {
        const QStringList videoFormats = WallpaperImageSetter::videoMimeTypes();
        for (const QString &format : std::as_const(videoFormats))
        {
            const QMimeType mime = db.mimeTypeForName(format);
            if (!mime.isValid()) continue;
            videoFilters << mime.filterString();
            videoPatterns << mime.globPatterns();
        }
    }
    else qCDebug(DEBUGCAT) << "video wallpaper plugin" << WallpaperImageSetter::videoPluginId() << "not available";

    imageFilters.sort(Qt::CaseInsensitive);
    videoFilters.sort(Qt::CaseInsensitive);

    QStringList allFilters;
    if (!videoPatterns.isEmpty())
    {
        allFilters << i18nc("Qt file filter format", "All wallpaper files (%1)",
                            (imagePatterns+videoPatterns).join(' '));
    }
    allFilters << i18nc("Qt file filter format", "All image files (%1)", imagePatterns.join(' '));
    if (!videoPatterns.isEmpty())
    {
        allFilters << i18nc("Qt file filter format", "All video files (%1)", videoPatterns.join(' '));
    }
    allFilters << imageFilters << videoFilters;
    //qCDebug(DEBUGCAT) << allFilters;

    const QString recentClass(":wallpaper");
    QString recentDir = KRecentDirs::dir(recentClass);
    QString file = QFileDialog::getOpenFileName(this, i18n("Select Wallpaper File"),
                                                recentDir, allFilters.join(";;"));
    if (file.isEmpty()) return;

    QString rd = QFileInfo(file).path();
    KRecentDirs::add(recentClass, rd);

    setItemImageFile(item, file);
    if (WallpaperImageSetter::isVideoFile(file)) requestPreviews(QStringList(file));
}


//////////////////////////////////////////////////////////////////////////
//									//
//  PreferencesPage -- Global settings for the dialogue			//
//									//
//////////////////////////////////////////////////////////////////////////

void PreferencesPage::setStandalone(bool isStandalone)
{
    qCDebug(DEBUGCAT) << isStandalone;
    sIsStandalone = isStandalone;
}
