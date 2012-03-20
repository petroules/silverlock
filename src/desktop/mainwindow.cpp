#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <silverlocklib.h>
#include <petroules-utilities.h>
#include "silverlockapplication.h"
#include "silverlockpreferences.h"
#include "dialogs/aboutdialog.h"
#include "dialogs/configurecolumnsdialog.h"
#include "dialogs/databaseauthenticationdialog.h"
#include "dialogs/databaseprintpreviewdialog.h"
#include "dialogs/entryeditdialog.h"
#include "dialogs/groupeditdialog.h"
#include "dialogs/newdatabasewizard.h"
#include "dialogs/preferencesdialog.h"
#include "dialogs/searchdatabasedialog.h"
#include "dialogs/selectgroupdialog.h"
#include "dialogs/updatedialog.h"
#include "widgets/expandingspacerwidget.h"
#include "widgets/toolbarsearchwidget.h"
#ifdef Q_WS_MAC
#include "mac/mactoolbarsearchwidget.h"
#endif

/*!
    \class MainWindow

    The MainWindow class represents the main window of the application.
 */

/*!
    Constructs a new MainWindow.

    \param filter The event filter used to detect user idle.
    \param parent The parent widget of the dialog.
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), m_nodeCountStatusLabel(NULL), m_toolbarSearch(NULL),
    m_lockIdleTimerValue(0), m_exitIdleTimerValue(0), widgetsAndToolbarsAction(NULL)
{
    this->ui->setupUi(this);
    WindowManager::centerMainWindow(this);
    this->updateSingleInstance();
    this->clearCurrentFile();
    this->setupSignals();
    this->setupUiAdditional();
    this->setupKeyboardShortcuts();
    this->setupMenuIcons();
    SilverlockPreferences::instance().restoreWindowSettings(this);

    QObject::connect(qApp, SIGNAL(resetIdleTimer(QObject*)), SLOT(resetIdleTimer(QObject*)));
}

/*!
    Destroys the MainWindow.
 */
MainWindow::~MainWindow()
{
    QList<MainWindow*> mainWindows;
    foreach (QWidget *widget, QApplication::topLevelWidgets())
    {
        MainWindow *mainWin = qobject_cast<MainWindow*>(widget);
        if (mainWin && this != mainWin)
        {
            mainWindows.append(mainWin);
        }
    }

    if (mainWindows.count() > 0)
    {
        this->updateSingleInstance(mainWindows.last());
    }

    // Disconnect the document state signals first, otherwise
    // the UI will get destroyed and DocumentState's destructor
    // will call close and emit a signal that is connected to a
    // slot containing UI code... leading to segfaults
    QObject::disconnect(&this->m_documentState, NULL, this, NULL);

    if (this->ui)
    {
        delete this->ui;
        this->ui = NULL;
    }
}

GroupBrowserWidget* MainWindow::groupBrowser() const
{
    return this->ui->groupBrowser;
}

EntryTableWidget* MainWindow::entryTable() const
{
    return this->ui->entryTable;
}

void MainWindow::updateSingleInstance(MainWindow *mw)
{
    IntegratedApplication *singleApp = qiApp;
    if (singleApp)
    {
        // Remove any previous messageReceived signal handler and reconnect it to the specified MainWindow
        QObject::disconnect(singleApp, SIGNAL(messageReceived(const QString&)), NULL, NULL);
        QObject::connect(singleApp, SIGNAL(messageReceived(const QString&)), mw, SLOT(handleMessage(const QString&)));

        // Set the activation window to the specified one
        singleApp->setActivationWindow(mw, false);

        // Connect our needToShow signal
        QObject::connect(mw, SIGNAL(needToShow()), singleApp, SLOT(activateWindow()));
    }
}

/*!
    \internal
 */
void MainWindow::changeEvent(QEvent* e)
{
    switch (e->type())
    {
        case QEvent::LanguageChange:
            this->ui->retranslateUi(this);
            break;
        case QEvent::WindowStateChange:
            {
                // The window was minimized...
                if (this->windowState() & Qt::WindowMinimized)
                {
                    // If the user selected 'lock when minimizing' in preferences, lock the workspace
                    if (SilverlockPreferences::instance().lockWhenMinimizing())
                    {
                        this->lockWorkspace();
                    }

                    // If the user selected 'minimize to tray' in preferences, hide the window entirely
                    // It can be reactivated by clicking the tray icon
                    if (SilverlockPreferences::instance().minimizeToTray())
                    {
                        QTimer::singleShot(250, this, SLOT(hide()));
                    }
                }

                break;
            }
        case QEvent::Resize:
            {
                // Update geometry of select subwidgets
                if (this->ui->standardToolBar)
                {
                    this->ui->standardToolBar->updateGeometry();
                }
            }
        default:
            break;
    }

    QMainWindow::changeEvent(e);
}

/*!
    \internal
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    // Do the prompting and saving, and it'll tell us whether the window should be closed or not
    if (this->maybeSave())
    {
        if (SilverlockPreferences::instance().clearClipboardWhenClosing())
        {
            QApplication::clipboard()->clear();
        }

        SilverlockPreferences::instance().saveWindowSettings(this);

        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->setAccepted(SilverlockApplication::isDesktopFileManagerDrop(event->mimeData()));
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QStringList files;

    if (SilverlockApplication::isDesktopFileManagerDrop(event->mimeData(), &files))
    {
        event->accept();
        qsApp->addQueuedFiles(files);
        QTimer::singleShot(50, qsApp, SLOT(openQueuedFiles()));
    }
    else
    {
        event->ignore();
    }
}

/*!
    \internal
 */
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
            {
#ifndef Q_WS_MAC
                // We don't want this to happen in Mac OS X since the Dock already has a "hide"
                // option which does pretty much the same thing, plus tray icons in Mac show the
                // context menu whether they are left or right clicked, so it makes no sense to have
                this->setVisible(!this->isVisible());
#endif
                if (this->isVisible())
                {
                    this->setWindowState(this->windowState() & ~Qt::WindowMinimized);
                    this->raise();
                    this->activateWindow();
                }

                break;
            }
        default:
            break;
    }
}

/*!
    Connects various slots and signals.
 */
void MainWindow::setupSignals()
{
    QObject::connect(&this->m_documentState, SIGNAL(stateChanged()), SLOT(updateInterfaceState()));
    QObject::connect(&this->m_documentState, SIGNAL(treeModified()), SLOT(databaseWasModified()));

    // Hide the password by default
    this->ui->unlockWorkspaceRevealToolButton->setChecked(true);
}

/*!
    Performs additional UI setup; creation of widgets and setting of properties.
 */
void MainWindow::setupUiAdditional()
{
    // Add spacer and search widget to toolbar
    this->ui->standardToolBar->addWidget(new ExpandingSpacerWidget());
    this->ui->standardToolBar->addWidget(this->m_toolbarSearch =
#ifdef Q_WS_MAC
        new MacToolbarSearchWidget()
#else
        new ToolbarSearchWidget()
#endif
    );

    QObject::connect(this->m_toolbarSearch, SIGNAL(searchRequested(QString)), SLOT(toolbarSearch(QString)));

    // Create the system tray icon
    QSystemTrayIcon *icon = new QSystemTrayIcon(this);
    icon->setIcon(this->windowIcon());
    icon->setToolTip(this->windowTitle());
    icon->setContextMenu(this->ui->menuFile);
    icon->show();
    QObject::connect(icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    this->m_clearClipboardTimer = new QTimer(this);
    QObject::connect(this->m_clearClipboardTimer, SIGNAL(timeout()), SLOT(clearClipboard()));

    this->m_idleTimer = new QTimer(this);
    QObject::connect(this->m_idleTimer, SIGNAL(timeout()), SLOT(incrementIdleTimer()));
    this->m_idleTimer->start(1000);

    this->setAttribute(Qt::WA_DeleteOnClose);

    this->statusBar()->showMessage(tr("Ready"));
    this->statusBar()->addPermanentWidget(this->m_nodeCountStatusLabel = new QLabel(this));

    // Add actions for dock widgets and toolbars
    this->widgetsAndToolbarsAction = this->ui->menuView->insertMenu(this->ui->actionFullScreen, this->createPopupMenu());
    this->widgetsAndToolbarsAction->setText(tr("Widgets && Toolbars"));
    this->ui->menuView->insertSeparator(this->ui->actionFullScreen);

    // Set menu actions to correct initial states
    this->ui->actionShowEntryView->setChecked(true);

    // Recent file actions
    this->separatorAction = this->ui->menuFile->insertSeparator(this->ui->actionLockWorkspace);
    this->recentFileActions
        << this->ui->recentFileAction1
        << this->ui->recentFileAction2
        << this->ui->recentFileAction3
        << this->ui->recentFileAction4
        << this->ui->recentFileAction5;
    for (int i = 0; i < SilverlockPreferences::instance().maxRecentFiles(); ++i)
    {
        QObject::connect(this->recentFileActions[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }

    this->updateInterfaceState();
    this->updateRecentFileActions();

#ifdef Q_WS_X11
    // Most X11 window managers provide a built-in menu allowing the
    // user to configure advanced window properties; we won't need these
    this->ui->actionAlwaysOnTop->setEnabled(false);
    this->ui->actionAlwaysOnTop->setVisible(false);
#endif

#ifndef Q_WS_WIN
    // Internet Explorer only exists in Windows, so...
    this->ui->actionInternetExplorer->setEnabled(false);
    this->ui->actionInternetExplorer->setVisible(false);
#endif

	// If automatic updates aren't enabled for this platform then we needn't show the menu item
	this->ui->actionCheckForUpdates->setVisible(UpdateDialog::automaticUpdatesSupported());
}

/*!
    Assigns cross-platform keyboard shortcuts to the program menu actions.
 */
void MainWindow::setupKeyboardShortcuts()
{
    // File menu
    this->ui->actionNew->setShortcut(QKeySequence::New);
    this->ui->actionOpen->setShortcut(QKeySequence::Open);
    this->ui->actionClose->setShortcut(QKeySequence::Close);
    this->ui->actionSave->setShortcut(QKeySequence::Save);
    this->ui->actionSaveAs->setShortcut(QKeySequence::SaveAs);
    this->ui->actionPrint->setShortcut(QKeySequence::Print);
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    this->ui->actionExit->setShortcut(QKeySequence::Quit);
#elif defined(Q_WS_MAC) || defined(Q_OS_LINUX)
    this->ui->actionExit->setShortcut(QKeySequence(Qt::Key_Control | Qt::Key_Q));
#endif

    // Edit menu
    this->ui->actionSelectAllEntries->setShortcut(QKeySequence::SelectAll);
    this->ui->actionSelectAllGroups->setShortcut(QKeySequence(Qt::SHIFT) | QKeySequence(QKeySequence::SelectAll));
    this->ui->actionFind->setShortcut(QKeySequence::Find);

    // Edit, entry menu
    this->ui->actionEditEntry->setShortcut(QKeySequence(Qt::Key_F2));
    this->ui->actionDeleteEntries->setShortcut(QKeySequence(Qt::Key_Delete));

    // Edit, group menu
    this->ui->actionEditGroup->setShortcut(QKeySequence(Qt::SHIFT | Qt::Key_F2));
    this->ui->actionDeleteGroups->setShortcut(QKeySequence(Qt::SHIFT | Qt::Key_Delete));

    // View menu
    this->ui->actionFullScreen->setShortcut(Qt::Key_F11);

    // Tools menu
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    this->ui->actionPreferences->setShortcut(QKeySequence::Preferences);
#elif defined(Q_WS_MAC)
    this->ui->actionPreferences->setShortcut(QKeySequence(Qt::Key_Control | Qt::Key_Comma));
#endif

    // Help menu
    this->ui->actionHelpContents->setShortcut(QKeySequence::HelpContents);
}

void MainWindow::setupMenuIcons()
{
    this->ui->actionOpen->setIcon(this->style()->standardIcon(QStyle::SP_DialogOpenButton));
    this->ui->actionClose->setIcon(this->style()->standardIcon(QStyle::SP_DialogCloseButton));
    this->ui->actionSave->setIcon(this->style()->standardIcon(QStyle::SP_DialogSaveButton));

#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    this->ui->actionExit->setIcon(QIcon::fromTheme("application-exit"));
    this->ui->actionPreferences->setIcon(QIcon::fromTheme("preferences-other"));
    this->ui->actionAboutSilverlock->setIcon(QIcon::fromTheme("help-about"));
#endif
}

void MainWindow::handleMessage(const QString &message)
{
    // If there was a message, we need to load a file..
    if (!message.isEmpty())
    {
        qsApp->openFile(message);
    }
    else
    {
        // If not, should we open the last opened file?
        if (SilverlockPreferences::instance().openLastDatabase())
        {
            QStringList list = SilverlockPreferences::instance().recentFileList();
            if (list.count() > 0)
            {
                qsApp->openFile(list.first());
            }
        }
    }

    emit this->needToShow();
}

void MainWindow::incrementIdleTimer()
{
    this->m_lockIdleTimerValue++;
    this->m_exitIdleTimerValue++;

    if (SilverlockPreferences::instance().lockWorkspaceTimeoutEnabled() &&
        this->m_lockIdleTimerValue >= SilverlockPreferences::instance().lockWorkspaceTimeout())
    {
        this->lockWorkspace();
    }

    if (SilverlockPreferences::instance().autoExitEnabled() &&
        this->m_exitIdleTimerValue >= SilverlockPreferences::instance().autoExit())
    {
        QApplication::closeAllWindows();
    }
}

void MainWindow::resetIdleTimer(QObject *object)
{
    // We want to make sure the event came from this window or one of its children
    // so that all windows aren't locked simultaneously from events in other windows
    QWidget *widget = qobject_cast<QWidget*>(object);
    if (this == object || (widget && this->isAncestorOf(widget)))
    {
        this->m_lockIdleTimerValue = 0;
    }

    this->m_exitIdleTimerValue = 0;
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (action)
    {
        qsApp->openFile(action->data().toString());
    }
}

void MainWindow::showPrintDialog(const DatabasePrinterFields &fields)
{
#ifndef QT_NO_PRINTER
    if (!this->m_documentState.hasDocument())
    {
        return;
    }

    DatabasePrinter databasePrinter(this->m_documentState.database());
    QTextDocument *document = databasePrinter.toTextDocument(fields);

    QPrinter printer;
    NativePrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        document->print(&printer);
    }

    delete document;
#else
    NativeDialogs::critical(this, tr("No print support"), tr("This computer or device does not support printing."));
#endif
}

void MainWindow::toolbarSearch(QString terms)
{
    SearchParameters params;
    params.searchPattern = terms;
    if (!params.searchPattern.isEmpty() && this->m_documentState.hasDocument())
    {
        QList<Entry*> entries = this->m_documentState.database()->findEntries(params);
        this->populateWithSearchResults(entries, params.searchPattern);
    }
    else if (this->m_documentState.hasDocument())
    {
        this->populateEntryTable(NULL);
    }
}

/*!
    Opens URLs in the entry info view in the user's browser.
 */
void MainWindow::on_infoView_anchorClicked(QUrl url)
{
    QDesktopServices::openUrl(url);
}

/*!
    Shows the context menu for the selected group in the left hand group browser.
 */
void MainWindow::on_groupBrowser_customContextMenuRequested(QPoint pos)
{
    Q_UNUSED(pos);
    if (this->ui->groupBrowser->selectedUuids().count() > 0)
    {
        this->ui->menuGroup->exec(QCursor::pos());
    }
}

/*!
    Shows the context menu for the selected entry in the right hand entry table.
 */
void MainWindow::on_entryTable_customContextMenuRequested(QPoint pos)
{
    Q_UNUSED(pos);
    if (this->ui->entryTable->selectedUuids().count() > 0)
    {
        this->ui->menuEntry->exec(QCursor::pos());
    }
    else
    {
        this->ui->menuGroup->exec(QCursor::pos());
    }
}

/*!
    Populates the detail view with a list of the entries in the
    group selected in the left hand tree view.
 */
void MainWindow::on_groupBrowser_itemSelectionChanged()
{
    this->updateInterfaceState();

    if (this->m_documentState.hasDocument())
    {
        Database *db = this->m_documentState.database();

        // If more than one group is selected, clear the views as
        // we can only display the contents of ONE AT A TIME!
        if (this->ui->groupBrowser->selectedUuids().count() != 1)
        {
            this->clearViews();
            return;
        }

        // Get the selected group UUID
        QUuid uuid = this->ui->groupBrowser->selectedUuid();
        if (!uuid.isNull())
        {
            // Find the group for this UUID and populate the entry table with it
            this->populateEntryTable(db->findGroup(uuid, true));
        }
    }
}

/*!
    Populates the info view with the details of the entry selected in the right hand detail view.
 */
void MainWindow::on_entryTable_itemSelectionChanged()
{
    this->updateInterfaceState();

    if (this->m_documentState.hasDocument())
    {
        Database *db = this->m_documentState.database();

        // If more than one entry is selected, clear the info view as
        // we can only display the properties of ONE AT A TIME!
        if (this->ui->entryTable->selectedUuids().count() != 1)
        {
            this->ui->infoView->clear();
            return;
        }

        // Get the selected entry UUID
        QUuid uuid = this->ui->entryTable->selectedUuid();
        if (!uuid.isNull())
        {
            // Find the entry for this UUID and update the info view with its data
            this->populateInfoView(db->findEntry(uuid));
        }
    }
}

void MainWindow::updateMenus()
{
    this->ui->actionAlwaysOnTop->setChecked(WindowManager::isTopMost(this));
    this->ui->actionFullScreen->setChecked(this->isFullScreen());
}

/*!
    Updates the state of interface so that only widgets and menu items
    that are applicable to the current application state are enabled.
 */
void MainWindow::updateInterfaceState()
{
    // A few variables to reduce verbosity
    const bool isDatabaseSelected = this->groupBrowser()->isDatabaseSelected();
    const int groups = this->ui->groupBrowser->selectedUuids().count();
    const int entries = this->ui->entryTable->selectedUuids().count();
    const bool hasDocument = this->m_documentState.hasDocument();
    const bool locked = this->m_documentState.isLocked();

    // Update all the menu actions
    this->ui->actionClose->setEnabled(hasDocument && !locked);
    this->ui->actionSave->setEnabled(hasDocument && !locked);
    this->ui->actionSaveAs->setEnabled(hasDocument && !locked);
    this->ui->actionChangeDatabasePassword->setEnabled(hasDocument && !locked);
    this->ui->actionPrint->setEnabled(hasDocument && !locked);
    this->ui->actionPrintPreview->setEnabled(hasDocument && !locked);
    this->ui->actionLockWorkspace->setEnabled(hasDocument && !locked);

    this->ui->actionAddSubgroup->setEnabled(groups == 1 && !locked);
    this->ui->actionAddEntry->setEnabled(groups == 1 && !locked);
    this->ui->actionMoveGroup->setEnabled(groups == 1 && !isDatabaseSelected && !locked);
    this->ui->actionEditGroup->setEnabled(groups == 1 && !locked);
    this->ui->actionDeleteGroups->setEnabled(groups > 0 && !isDatabaseSelected && !locked);
    this->ui->actionFindInGroup->setEnabled(groups == 1 && !locked);

    this->ui->actionCopyFieldValue->setEnabled(entries == 1 && !locked);
    this->ui->actionDefaultBrowser->setEnabled(entries == 1 && !locked);
#ifdef Q_WS_WIN
    this->ui->actionInternetExplorer->setEnabled(entries == 1 && !locked);
#endif
    this->ui->actionMoveEntries->setEnabled(entries > 0 && !locked);
    this->ui->actionDuplicateEntries->setEnabled(entries > 0 && !locked);
    this->ui->actionEditEntry->setEnabled(entries == 1 && !locked);
    this->ui->actionDeleteEntries->setEnabled(entries > 0 && !locked);

    this->ui->actionSelectAllEntries->setEnabled(hasDocument && !locked);
    this->ui->actionSelectAllGroups->setEnabled(hasDocument && !locked);
    this->ui->actionFind->setEnabled(hasDocument && !locked);

    this->ui->actionConfigureColumns->setEnabled(!locked);

    this->m_toolbarSearch->setEnabled(hasDocument && !locked);

    // Update the main widgets
    this->ui->groupBrowser->setEnabled(hasDocument && !locked);
    this->ui->entryTable->setEnabled(hasDocument && !locked);
    this->ui->infoView->setEnabled(hasDocument && !locked);
}

/*!
    Tells the window that the database was modified.
 */
void MainWindow::databaseWasModified()
{
    this->setWindowModified(true);
}

/*!
    Clears the entry table, info view, and the node count label.
 */
void MainWindow::clearViews()
{
    // The entry table only needs to be cleared if the group browser is populating;
    // the entry view clears itself internally before adding new entries, plus the
    // main purpose of this check is to prevent a flickering effect where the entry
    // view's info label and table are quickly shown and hidden in succession
    if (sender() == this->ui->groupBrowser)
    {
        this->ui->entryTable->clear();
    }

    this->ui->infoView->clear();
    this->setNodeCount(NULL);

    this->ui->stackedWidget->setCurrentWidget(this->ui->mainPage);
}

/*!
    Sets the node count of \a group on the node count status label in the status bar.

    Pass \c NULL to \a group to clear the label.
 */
void MainWindow::setNodeCount(Group *group)
{
    if (group)
    {
        // Display the number of groups and entries in the selected group, in the status bar
        QString nodeCount = tr("<b>%1:</b> Group(s): %2 / %3, Entry(-ies): %4 / %5")
            .arg(group->title())
            .arg(group->groups().count())
            .arg(group->countGroups())
            .arg(group->entries().count())
            .arg(group->countEntries());
        this->m_nodeCountStatusLabel->setText(nodeCount);
    }
    else
    {
        this->m_nodeCountStatusLabel->clear();
    }
}

void MainWindow::populateWithSearchResults(const QList<Entry*> &entries, const QString &keywords)
{
    this->ui->entryTable->populate(entries);
    this->m_nodeCountStatusLabel->setText(tr("<b>Search results:</b> (\"%1\"), %n entry(-ies) found", "", entries.count()).arg(keywords));
}

void MainWindow::populateEntryTable(Group *const group)
{
    if (group)
    {
        this->ui->entryTable->populate(group);
        this->setNodeCount(group);

        // Last accessed... NOW!
        group->setAccessed();
    }
    else
    {
        this->ui->entryTable->clear();
        this->setNodeCount(NULL);
    }
}

void MainWindow::populateInfoView(Entry *const entry)
{
    if (entry)
    {
        QStringList pairs;
        if (entry->parentNode() && !entry->parentNode()->title().isEmpty())
        {
            pairs.append(tr("<b>Group:</b> %1").arg(Qt::escape(entry->parentNode()->title())));
        }

        if (!entry->title().isEmpty())
        {
            pairs.append(tr("<b>Title:</b> %1").arg(Qt::escape(entry->title())));
        }

        if (!entry->username().isEmpty())
        {
            pairs.append(tr("<b>Username:</b> %1").arg(Qt::escape(entry->username())));
        }

        if (!entry->password().isEmpty())
        {
            pairs.append(tr("<b>Password:</b> %1").arg(Qt::escape(entry->password())));
        }

        if (!entry->url().isEmpty())
        {
            pairs.append(tr("<b>URL:</b> <a href=\"%1\">%1</a>").arg(Qt::escape(entry->url().toString())));
        }

        // Custom fields go after the primary data but before the unmodifiable details
        QMapIterator<QString, QString> j(entry->customFields());
        while (j.hasNext())
        {
            j.next();
            pairs.append(tr("<b>%1:</b> %2").arg(Qt::escape(j.key())).arg(Qt::escape(j.value())));
        }

        // We'll always have a UUID, and the created/accessed/modified times
        pairs.append(tr("<font color='gray'><b>UUID:</b> %1</font>").arg(Qt::escape(entry->uuid().toString())));
        pairs.append(tr("<font color='gray'><b>Created:</b> %1</font>").arg(Qt::escape(entry->created().toLocalTime().toString(Qt::SystemLocaleLongDate))));
        pairs.append(tr("<font color='gray'><b>Accessed:</b> %1</font>").arg(Qt::escape(entry->accessed().toLocalTime().toString(Qt::SystemLocaleLongDate))));
        pairs.append(tr("<font color='gray'><b>Modified:</b> %1</font>").arg(Qt::escape(entry->modified().toLocalTime().toString(Qt::SystemLocaleLongDate))));

        // Last accessed... NOW!
        entry->setAccessed();

        // Add the recovery questions...
        QString recoveryString;
        QMapIterator<QString, QString> i(entry->recoveryInfo());
        while (i.hasNext())
        {
            i.next();
            recoveryString += tr("<li><b>%1</b>: %2</li>").arg(Qt::escape(i.key())).arg(Qt::escape(i.value()));
        }

        QString info = tr("<p>%1</p> <ul>%2</ul> <hr /> <p>%3</p>")
            .arg(pairs.join(", "))
            .arg(recoveryString)
            .arg(Qt::escape(entry->notes()).replace(QRegExp("((?:https?|ftp)://\\S+)"), "<a href=\"\\1\">\\1</a>").replace("\n", "<br />"));
        this->ui->infoView->setText(info);
    }
    else
    {
        // If we couldn't find the entry, just clear the info view
        this->ui->infoView->clear();
    }
}

/*!
    Check if the database needs to be saved, and prompt the user for a choice.

    \return Whether the window should be closed.
 */
bool MainWindow::maybeSave()
{
    if (this->m_documentState.hasDocument() && this->isWindowModified())
    {
        if (SilverlockPreferences::instance().autoSaveOnClose() && !this->m_documentState.isUntitled())
        {
            return this->saveFile();
        }

        QMessageBox::StandardButtons buttons = QMessageBox::Discard | QMessageBox::Cancel;

        // It's important for security to do this here, because if the user locks a database
        // and decides to discard changes for the moment, another user could later come along
        // and close the window and save the changes the original user intended to discard...
        if (!this->m_documentState.isLocked())
        {
            buttons |= QMessageBox::Save;
        }

        QMessageBox::StandardButton ret = NativeDialogs::warning(this, tr("Save Changes"), tr("Do you want to save the changes you made in the database \"%1\"?").arg(this->m_documentState.currentFile()),
            tr("Your changes will be lost if you don't save them."), buttons);
        if (ret == QMessageBox::Save)
        {
            return this->saveFile();
        }
        else if (ret == QMessageBox::Cancel)
        {
            return false;
        }
    }

    return true;
}

/*!
    Reads a database file from \a fileName and sets it as the current file if it succeeds.

    \param fileName The name of the file to read from.
 */
void MainWindow::loadFile(const QString &fileName)
{
    if (!this->m_documentState.hasDocument())
    {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly))
        {
            NativeDialogs::critical(this, tr("Error"), tr("Cannot read file %1:\n%2.").arg(fileName).arg(file.errorString()));
            return;
        }

        DatabaseAuthenticationDialog dialog(file.fileName(), this);
        if (dialog.exec() == QDialog::Accepted)
        {
            QApplication::setOverrideCursor(Qt::WaitCursor);

            // Try to read the database, if it succeeds, populate the tree view, otherwise show an error message
            DatabaseReader reader;
            this->m_documentState.load(reader.read(file, dialog.password()));
            if (this->m_documentState.hasDocument())
            {
                this->ui->groupBrowser->populate(this->m_documentState.database());
                QApplication::restoreOverrideCursor();

                this->setCurrentFile(fileName);
                this->statusBar()->showMessage(tr("File loaded"), 2000);
            }
            else
            {
                QApplication::restoreOverrideCursor();
                NativeDialogs::critical(this, tr("Error"), reader.errorString());
            }
        }
    }
}

/*!
    Attempts to write the database file to \a fileName.

    \param fileName The file to write the database to.
    \param encrypt Whether to encrypt the database when saving it. The default is true.
 */
bool MainWindow::saveFile(const QString &fileName, bool encrypt)
{
    if (this->m_documentState.hasDocument())
    {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            NativeDialogs::critical(this, tr("Error"), tr("Cannot write file %1:\n%2").arg(fileName).arg(file.errorString()));
            return false;
        }

        QApplication::setOverrideCursor(Qt::WaitCursor);

        // Try to write the database to the IO device
        DatabaseWriter writer;
        if (writer.write(this->m_documentState.database(), file, encrypt))
        {
            QApplication::restoreOverrideCursor();

            // We only want to set the current file if we saved in encrypted format
            if (encrypt)
            {
                this->setCurrentFile(fileName);
            }

            this->statusBar()->showMessage(tr("File saved"), 2000);
            return true;
        }
        else
        {
            // Otherwise show a failure message
            QApplication::restoreOverrideCursor();
            NativeDialogs::critical(this, tr("Error"), tr("Cannot write file %1").arg(fileName));
        }
    }

    return false;
}

QString MainWindow::closeFileInternal()
{
    if (this->m_documentState.hasDocument())
    {
        if (!this->maybeSave())
        {
            return QString();
        }

        QString fileName = this->m_documentState.currentFile();

        this->clearCurrentFile();

        this->ui->groupBrowser->clear();
        this->ui->entryTable->clear();
        this->clearViews();

        // Put the welcome page back
        this->ui->stackedWidget->setCurrentWidget(this->ui->welcomePage);

        return fileName;
    }

    return QString();
}

/*!
    Sets the name of the currently loaded file to \a fileName.

    \param fileName The name of the file to set.
 */
void MainWindow::setCurrentFile(const QString &fileName)
{
    this->m_documentState.setUntitled(fileName.isEmpty());
    if (this->m_documentState.isUntitled())
    {
        this->m_documentState.setCurrentFile(tr("Untitled"));
    }
    else
    {
        this->m_documentState.setCurrentFile(QFileInfo(fileName).canonicalFilePath());

        // Add this file to the recent files list
        SilverlockPreferences::instance().addRecentFile(fileName);

        this->updateRecentFileActionsAll();
    }

    this->setWindowTitle(QString());
    this->setWindowModified(false);
    this->setWindowFilePath(this->m_documentState.currentFile());
}

void MainWindow::clearCurrentFile()
{
    this->m_documentState.close();
    this->setWindowTitle(QCoreApplication::applicationName());
    this->setWindowModified(false);
    this->setWindowFilePath(this->m_documentState.currentFile());
}

void MainWindow::updateRecentFileActionsAll()
{
    // Update all the main windows with file actions
    foreach (QWidget *widget, QApplication::topLevelWidgets())
    {
        MainWindow *mainWin = qobject_cast<MainWindow*>(widget);
        if (mainWin)
        {
            mainWin->updateRecentFileActions();
        }
    }
}

void MainWindow::updateRecentFileActions()
{
    const int maxFiles = SilverlockPreferences::instance().maxRecentFiles();
    QStringList files = SilverlockPreferences::instance().recentFileList();

    // Get the number of files we should add...
    const int numRecentFiles = qMin(files.count(), maxFiles);
    for (int i = 0; i < numRecentFiles; i++)
    {
        QString text = QString("&%1 %2").arg(i + 1).arg(this->strippedName(files[i]));
        this->recentFileActions[i]->setText(text);
        this->recentFileActions[i]->setStatusTip(QDir::toNativeSeparators(files[i]));
        this->recentFileActions[i]->setData(files[i]);
        this->recentFileActions[i]->setVisible(true);
    }

    for (int j = numRecentFiles; j < maxFiles; j++)
    {
        this->recentFileActions[j]->setVisible(false);
    }

    this->separatorAction->setVisible(numRecentFiles > 0);
}

/*!
    Returns the name of the file, excluding the path.

    \param fullFileName The full file path to strip.
 */
QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::on_unlockWorkspacePushButton_clicked()
{
    if (this->m_documentState.hasDocument())
    {
        if (this->ui->unlockWorkspacePasswordLineEdit->text() == this->m_documentState.database()->password())
        {
            // Big security risk without this line ;)
            this->ui->unlockWorkspacePasswordLineEdit->clear();
            this->unlockWorkspace();
        }
        else
        {
            NativeDialogs::critical(this, tr("Error"), tr("The password you entered is incorrect. Please verify that you have entered it correctly."));
        }
    }
}

/*!
    Sets a value indicating whether the password should be hidden using asterisks.

    \param checked \c true to hide the password, \c false to display it verbatim.
 */
void MainWindow::hideUnlockPassword(bool checked)
{
    this->ui->unlockWorkspacePasswordLineEdit->setEchoMode(checked ? QLineEdit::Password : QLineEdit::Normal);
}

QByteArray MainWindow::saveEntryTableState() const
{
    return this->ui->entryTable->saveState();
}

bool MainWindow::restoreEntryTableState(const QByteArray &state)
{
    return this->ui->entryTable->restoreState(state);
}
