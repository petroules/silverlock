#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <silverlocklib.h>
#include <liel.h>
#include "aboutdialog.h"
#include "entryeditdialog.h"
#include "groupeditdialog.h"
#include "preferencesdialog.h"
#include "searchdatabasedialog.h"
#include "selectgroupdialog.h"
#include "silverlockpreferences.h"
#include "version.h"
#include "configurecolumnsdialog.h"
#include "databaseauthenticationdialog.h"
#include "updatedialog.h"
#include "inactivityeventfilter.h"
#include "databaseprintdialog.h"
#include <QtSingleApplication>

/*!
    \class MainWindow

    The MainWindow class represents the main window of the application.
 */

/*!
    Constructs a new MainWindow.
 */
MainWindow::MainWindow(InactivityEventFilter *filter, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), m_nodeCountStatusLabel(NULL), m_searchBox(NULL), m_searchButton(NULL),
    m_lockIdleTimerValue(0), m_exitIdleTimerValue(0)
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

    QObject::connect(this->m_filter = filter, SIGNAL(resetIdleTimer(QObject*)), SLOT(resetIdleTimer(QObject*)));
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

void MainWindow::updateSingleInstance(MainWindow *mw)
{
    QCoreApplication *app = QCoreApplication::instance();
    QtSingleApplication *singleApp = qobject_cast<QtSingleApplication*>(app);
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
                if (this->windowState() & Qt::WindowMinimized)
                {
                    if (SilverlockPreferences::instance().lockWhenMinimizing())
                    {
                        this->lockWorkspace();
                    }

                    if (SilverlockPreferences::instance().minimizeToTray())
                    {
                        QTimer::singleShot(250, this, SLOT(hide()));
                    }
                }

                break;
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

/*!
    \internal
 */
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
            {
                this->setVisible(!this->isVisible());
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
    QObject::connect(&this->m_documentState, SIGNAL(modified()), SLOT(databaseWasModified()));

    // Hide the password by default
    this->ui->unlockWorkspaceRevealToolButton->setChecked(true);
}

/*!
    Performs additional UI setup; creation of widgets and setting of properties.
 */
void MainWindow::setupUiAdditional()
{
    // Add the toolbar search box and button and connect their slots appropriately
    this->ui->mainToolBar->addWidget(this->m_searchBox = new QLineEdit(this));
    this->m_searchBox->setSizePolicy(QSizePolicy::Preferred, this->m_searchBox->sizePolicy().verticalPolicy());
    this->ui->mainToolBar->addWidget(this->m_searchButton = new QPushButton(tr("Search"), this));
    this->m_searchButton->setSizePolicy(QSizePolicy::Preferred, this->m_searchButton->sizePolicy().verticalPolicy());
    QObject::connect(this->m_searchBox, SIGNAL(returnPressed()), this->m_searchButton, SLOT(click()));
    QObject::connect(this->m_searchButton, SIGNAL(clicked()), SLOT(toolbarSearch()));

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
    this->setUnifiedTitleAndToolBarOnMac(true);

    this->statusBar()->showMessage(tr("Ready"));
    this->statusBar()->addPermanentWidget(this->m_nodeCountStatusLabel = new QLabel(this));

    // Add actions for toolbars
    this->ui->menuToolbars->addAction(this->ui->mainToolBar->toggleViewAction());

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

#ifndef Q_OS_WIN
    // Internet Explorer only exists in Windows, so...
    this->ui->actionInternetExplorer->setEnabled(false);
    this->ui->actionInternetExplorer->setVisible(false);
#endif
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
    this->ui->actionExit->setShortcut(QKeySequence::Quit);

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
    this->ui->actionPreferences->setShortcut(QKeySequence::Preferences);

    // Help menu
    this->ui->actionHelpContents->setShortcut(QKeySequence::HelpContents);
}

void MainWindow::setupMenuIcons()
{
    // File menu
    this->ui->actionOpen->setIcon(this->style()->standardIcon(QStyle::SP_DialogOpenButton));
    this->ui->actionClose->setIcon(this->style()->standardIcon(QStyle::SP_DialogCloseButton));
    this->ui->actionSave->setIcon(this->style()->standardIcon(QStyle::SP_DialogSaveButton));
    this->ui->actionExit->setIcon(QIcon::fromTheme("application-exit"));

    // Tools menu
    this->ui->actionPreferences->setIcon(QIcon::fromTheme("preferences-other"));

    // Help menu
    this->ui->actionAboutSilverlock->setIcon(QIcon::fromTheme("help-about"));
}

void MainWindow::handleMessage(const QString &message)
{
    // If there was a message, we need to load a file..
    if (!message.isEmpty())
    {
        this->loadFileInWindow(message);
    }
    else
    {
        // If not, should we open the last opened file?
        if (SilverlockPreferences::instance().openLastDatabase())
        {
            QStringList list = SilverlockPreferences::instance().recentFileList();
            if (list.count() > 0)
            {
                this->loadFileInWindow(list.first());
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

void MainWindow::on_actionNew_triggered()
{
    if (this->m_documentState.hasDocument())
    {
        MainWindow *other = new MainWindow(this->m_filter);
        other->move(this->x() + 40, this->y() + 40);
        other->show();
    }
    else
    {
        Database *database = new Database(QString(), QString());
        GroupEditDialog dialog(database, this);
        if (dialog.exec() == QDialog::Accepted)
        {
            this->m_documentState.load(database);
            this->ui->groupBrowser->populate(database);
            this->setCurrentFile(QString());

            // Call this here so the user doesn't accidentally close
            // the window without saving their new database
            this->databaseWasModified();
        }
        else
        {
            delete database;
        }
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), QString(), tr("Silverlock databases (*.sdbx)"));
    if (!fileName.isEmpty())
    {
        this->loadFileInWindow(fileName);
    }
}

void MainWindow::on_actionClose_triggered()
{
    if (this->m_documentState.hasDocument())
    {
        this->closeFile();
    }
}

bool MainWindow::save()
{
    if (this->m_documentState.hasDocument())
    {
        // If the open document doesn't have a file name set, prompt for one
        if (this->m_documentState.isUntitled())
        {
            return this->saveAs();
        }
        else
        {
            // Otherwise go ahead and save the file
            return this->saveFile(this->m_documentState.currentFile());
        }
    }

    return true;
}

bool MainWindow::saveAs()
{
    if (this->m_documentState.hasDocument())
    {
        // Prompt the user for a file name, and if it was empty, bail out
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
            this->m_documentState.currentFile(), tr("Silverlock databases (*.sdbx);;XML documents (*.xml)"));
        if (fileName.isEmpty())
        {
            return false;
        }

        // Check if the user is using encryption...
        bool encrypt = !fileName.endsWith(".xml", Qt::CaseInsensitive);
        if (!encrypt)
        {
            if (QMessageBox::warning(this, tr("Warning"), tr("<p><b>It is recommended that only advanced users select this option.</b></p><p>If you save the database in XML format, the contents will not be encrypted and any user with access to the file will be able to view the information in your database. It is strongly recommended that you save your files in Silverlock's SDBX format instead.</p><p>Are you sure you want save in the XML format?</p>"),
                QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
            {
                return false;
            }
        }

        // Otherwise save the file normally
        return this->saveFile(fileName, encrypt);
    }

    return true;
}

void MainWindow::on_actionChangeDatabasePassword_triggered()
{
    if (this->m_documentState.hasDocument())
    {
        Database *db = this->m_documentState.database();
        if (db)
        {
            QDialog *dialog = new GroupEditDialog(db, this);
            if (dialog->exec() == QDialog::Accepted)
            {
                this->ui->groupBrowser->populate(db);
                this->populateEntryTable(NULL);
            }

            delete dialog;
        }
    }
}

void MainWindow::on_actionPrint_triggered()
{
    if (this->m_documentState.hasDocument())
    {
        DatabasePrintDialog dialog(this->m_documentState.database(), this);
        dialog.print();
    }
}

void MainWindow::on_actionPrintPreview_triggered()
{
    if (this->m_documentState.hasDocument())
    {
        DatabasePrintDialog dialog(this->m_documentState.database(), this);
        dialog.exec();
    }
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (action)
    {
        this->loadFileInWindow(action->data().toString());
    }
}

void MainWindow::lockWorkspace()
{
    if (this->m_documentState.hasDocument() && !this->m_documentState.isLocked())
    {
        if (this->maybeSave())
        {
            this->m_documentState.lock();

            // Hide the search box text in case there are sensitive terms in the search box
            this->m_searchBox->setEchoMode(QLineEdit::NoEcho);
            this->ui->mainPage->setEnabled(false);
            this->ui->unlockPage->setEnabled(true);
            this->ui->stackedWidget->setCurrentWidget(this->ui->unlockPage);

            // Focus to the enter password line edit
            this->ui->unlockWorkspacePasswordLineEdit->setFocus();

            if (SilverlockPreferences::instance().minimizeAfterLock())
            {
                this->setWindowState(Qt::WindowMinimized);
            }
        }
    }
}

void MainWindow::unlockWorkspace()
{
    if (this->m_documentState.hasDocument() && this->m_documentState.isLocked())
    {
        this->m_searchBox->setEchoMode(QLineEdit::Normal);
        this->ui->unlockPage->setEnabled(false);
        this->ui->mainPage->setEnabled(true);
        this->ui->stackedWidget->setCurrentWidget(this->ui->mainPage);

        this->m_documentState.unlock();
    }
}

void MainWindow::toolbarSearch()
{
    SearchParameters params;
    params.searchPattern = this->m_searchBox->text();
    this->m_searchBox->clear();
    if (!params.searchPattern.isEmpty() && this->m_documentState.hasDocument())
    {
        QList<Entry*> entries = this->m_documentState.database()->findEntries(params);
        this->populateWithSearchResults(entries, params.searchPattern);
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), tr("You have not entered a search pattern."));
    }
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::closeAllWindows();
}

void MainWindow::on_actionCopyFieldValue_triggered()
{
    if (this->ui->entryTable->hasFocus())
    {
        QApplication::clipboard()->setText(this->ui->entryTable->selectedFieldText());
        if (SilverlockPreferences::instance().autoClearClipboardEnabled())
        {
            this->m_clearClipboardTimer->start(SilverlockPreferences::instance().autoClearClipboard() * 1000);
        }

        if (SilverlockPreferences::instance().minimizeAfterClipboard())
        {
            this->setWindowState(Qt::WindowMinimized);
        }
    }
}

void MainWindow::clearClipboard()
{
    QApplication::clipboard()->clear();
}

void MainWindow::on_actionDefaultBrowser_triggered()
{
    if (this->m_documentState.hasDocument())
    {
        Database *db = this->m_documentState.database();

        QList<QUuid> uuids = this->ui->entryTable->selectedUuids();
        if (uuids.count() == 1)
        {
            Entry *entry = db->findEntry(uuids.first());
            if (entry)
            {
                QDesktopServices::openUrl(entry->url());
            }
        }
    }
}

void MainWindow::on_actionInternetExplorer_triggered()
{
#ifdef Q_OS_WIN
    if (this->m_documentState.hasDocument())
    {
        Database *db = this->m_documentState.database();

        QList<QUuid> uuids = this->ui->entryTable->selectedUuids();
        if (uuids.count() == 1)
        {
            Entry *entry = db->findEntry(uuids.first());
            if (entry)
            {
                QProcess process;
                QStringList args;
                args << entry->url().toString();
                process.startDetached(QString("C:/Program Files/Internet Explorer/iexplore.exe"), args);
            }
        }
    }
#endif
}


void MainWindow::on_actionMoveEntries_triggered()
{
    if (this->m_documentState.hasDocument())
    {
        Database *db = this->m_documentState.database();

        QList<QUuid> uuids = this->ui->entryTable->selectedUuids();
        if (uuids.count() > 0)
        {
            // All this makes sure all the entries have the same parent,
            // and there's no reason they shouldn't, but we'll check anyways
            Group *firstParent = NULL;
            foreach (QUuid uuid, uuids)
            {
                Entry *entry = db->findEntry(uuid);
                if (entry)
                {
                    if (!firstParent)
                    {
                        firstParent = entry->parentNode();
                    }

                    if (entry->parentNode() != firstParent)
                    {
                        return;
                    }
                }
                else
                {
                    return;
                }
            }

            SelectGroupDialog dialog(this);
            dialog.populate(db);
            if (dialog.exec() == QDialog::Accepted)
            {
                Group *group = db->findGroup(dialog.selectedUuid(), true);
                if (group && group != firstParent)
                {
                    foreach (QUuid uuid, uuids)
                    {
                        Entry *entry = db->findEntry(uuid);
                        if (entry)
                        {
                            entry->setParentNode(group);
                        }
                    }

                    this->populateEntryTable(firstParent);
                }
            }
        }
    }
}

void MainWindow::on_actionDuplicateEntries_triggered()
{
    if (this->m_documentState.hasDocument())
    {
        Database *db = this->m_documentState.database();

        QList<QUuid> uuids = this->ui->entryTable->selectedUuids();
        foreach (QUuid uuid, uuids)
        {
            Entry *entry = db->findEntry(uuid);
            if (entry)
            {
                Group *parent = entry->parentNode();
                if (parent)
                {
                    Entry *copy = entry->createCopy();
                    copy->setTitle(copy->title() + tr(" - Copy"));
                    copy->setParentNode(parent);

                    this->populateEntryTable(parent);
                }
            }
        }
    }
}

void MainWindow::on_actionEditEntry_triggered()
{
    if (this->m_documentState.hasDocument())
    {
        Database *db = this->m_documentState.database();

        QUuid uuid = this->ui->entryTable->selectedUuid();
        if (!uuid.isNull())
        {
            Entry *entry = db->findEntry(uuid);
            if (entry)
            {
                EntryEditDialog dialog(entry, this);
                if (dialog.exec() == QDialog::Accepted)
                {
                    this->ui->groupBrowser->populate(db);
                    this->populateEntryTable(entry->parentNode());
                    this->populateInfoView(entry);
                }
            }
        }
    }
}

void MainWindow::on_actionDeleteEntries_triggered()
{
    if (this->m_documentState.hasDocument())
    {
        Database *db = this->m_documentState.database();

        int result = QMessageBox::warning(this, tr("Delete Entries"),
            tr("Are you sure you want to delete the selected entries?"),
            QMessageBox::Yes, QMessageBox::No);
        if (result == QMessageBox::Yes)
        {
            Group *parent = NULL;

            QList<QUuid> uuids = this->ui->entryTable->selectedUuids();
            foreach (QUuid uuid, uuids)
            {
                Entry* entry = db->findEntry(uuid);
                if (entry)
                {
                    parent = entry->parentNode();
                    delete entry;
                }
            }

            this->populateEntryTable(parent);
        }
    }
}

void MainWindow::on_actionAddSubgroup_triggered()
{
    if (this->m_documentState.hasDocument())
    {
        Database *db = this->m_documentState.database();

        QUuid uuid = this->ui->groupBrowser->selectedUuid();
        if (!uuid.isNull())
        {
            Group *group = db->findGroup(uuid, true);
            if (group)
            {
                Group *newGroup = new Group();

                GroupEditDialog dialog(newGroup, this);
                if (dialog.exec() == QDialog::Accepted)
                {
                    newGroup->setParentNode(group);

                    this->ui->groupBrowser->populate(db);
                    this->populateEntryTable(group);
                }
                else
                {
                    delete newGroup;
                }
            }
        }
    }
}

void MainWindow::on_actionAddEntry_triggered()
{
    if (this->m_documentState.hasDocument())
    {
        Database *db = this->m_documentState.database();

        QUuid uuid = this->ui->groupBrowser->selectedUuid();
        if (!uuid.isNull())
        {
            Group *group = db->findGroup(uuid, true);
            if (group)
            {
                Entry *newEntry = new Entry();

                EntryEditDialog dialog(newEntry, this);
                if (dialog.exec() == QDialog::Accepted)
                {
                    newEntry->setParentNode(group);

                    this->ui->groupBrowser->populate(db);
                    this->populateEntryTable(group);
                }
                else
                {
                    delete newEntry;
                }
            }
        }
    }
}

void MainWindow::on_actionMoveGroup_triggered()
{
    if (this->m_documentState.hasDocument())
    {
        Database *db = this->m_documentState.database();

        QList<QUuid> uuids = this->ui->groupBrowser->selectedUuids();
        if (uuids.count() == 1)
        {
            Group *sourceGroup = db->findGroup(uuids.first());
            if (sourceGroup)
            {
                SelectGroupDialog dialog(this);
                dialog.populate(db);
                if (dialog.exec() == QDialog::Accepted)
                {
                    if (sourceGroup->findGroup(dialog.selectedUuid(), true))
                    {
                        QMessageBox::critical(this, tr("Error"), tr("A group cannot be moved to itself or one of its subgroups."));
                    }
                    else
                    {
                        Group *targetGroup = db->findGroup(dialog.selectedUuid(), true);
                        if (targetGroup)
                        {
                            if (sourceGroup->parentNode() == targetGroup)
                            {
                                QMessageBox::critical(this, tr("Error"), tr("The source and destination group are the same."));
                            }
                            else
                            {
                                sourceGroup->setParentNode(targetGroup);
                                this->ui->groupBrowser->populate(db);
                            }
                        }
                    }
                }
            }
        }
    }
}

void MainWindow::on_actionEditGroup_triggered()
{
    if (this->m_documentState.hasDocument())
    {
        Database *db = this->m_documentState.database();

        QUuid uuid = this->ui->groupBrowser->selectedUuid();
        if (!uuid.isNull())
        {
            Group *group = db->findGroup(uuid, true);
            if (group)
            {
                QDialog *dialog = new GroupEditDialog(group, this);
                if (dialog->exec() == QDialog::Accepted)
                {
                    this->ui->groupBrowser->populate(db);
                    this->populateEntryTable(group);
                }

                delete dialog;
            }
        }
    }
}

void MainWindow::on_actionDeleteGroups_triggered()
{
    if (this->m_documentState.hasDocument())
    {
        Database *db = this->m_documentState.database();

        int result = QMessageBox::warning(this, tr("Delete Groups"),
            tr("Are you sure you want to delete the selected groups?"),
            QMessageBox::Yes, QMessageBox::No);
        if (result == QMessageBox::Yes)
        {
            QList<QUuid> uuids = this->ui->groupBrowser->selectedUuids();
            foreach (QUuid uuid, uuids)
            {
                Group* group = db->findGroup(uuid);
                if (group)
                {
                    delete group;
                }
            }

            this->ui->groupBrowser->populate(db);
        }
    }
}

void MainWindow::on_actionFind_triggered()
{
    if (this->m_documentState.hasDocument())
    {
        SearchDatabaseDialog dialog(this->m_documentState.database(), this);
        if (dialog.exec() == QDialog::Accepted)
        {
            this->populateWithSearchResults(dialog.entriesFound(), dialog.searchParameters()->searchPattern);
        }
    }
}

void MainWindow::on_actionFindInGroup_triggered()
{
    if (this->m_documentState.hasDocument())
    {
        Group *group = this->m_documentState.database()->findGroup(this->ui->groupBrowser->selectedUuid(), true);

        SearchDatabaseDialog dialog(group, this);
        if (dialog.exec() == QDialog::Accepted)
        {
            this->populateWithSearchResults(dialog.entriesFound(), dialog.searchParameters()->searchPattern);
        }
    }
}

void MainWindow::on_actionAlwaysOnTop_triggered(bool checked)
{
    WindowManager::setTopMost(this, checked);
}

void MainWindow::on_actionCenterToScreen_triggered()
{
    WindowManager::centerMainWindow(this);
}

void MainWindow::on_actionConfigureColumns_triggered()
{
    ConfigureColumnsDialog dialog(this->ui->entryTable, this);
    dialog.exec();
}

void MainWindow::on_actionFullScreen_triggered(bool checked)
{
    if (checked)
    {
        this->showFullScreen();
    }
    else
    {
        this->showNormal();
    }
}

void MainWindow::on_actionPreferences_triggered()
{
    PreferencesDialog preferences(this);
    if (preferences.exec())
    {
        // Update recent file actions in case they were cleared
        this->updateRecentFileActionsAll();
    }
}

void MainWindow::on_actionHelpContents_triggered()
{

}

void MainWindow::on_actionWebsite_triggered()
{
    QDesktopServices::openUrl(QUrl("http://www.petroules.com/products/silverlock/"));
}

void MainWindow::on_actionDonate_triggered()
{
    QDesktopServices::openUrl(QUrl("http://www.petroules.com/donate/"));
}

void MainWindow::on_actionCheckForUpdates_triggered()
{
    UpdateDialog dialog(this);
    dialog.exec();
}

void MainWindow::on_actionAboutSilverlock_triggered()
{
    AboutDialog dialog(this);
    dialog.exec();
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

/*!
    Determines whether the database node is in the current group browser selection.
 */
bool MainWindow::isDatabaseSelected()
{
    bool isDatabaseSelected = false;
    if (this->m_documentState.hasDocument())
    {
        Database *db = this->m_documentState.database();

        foreach (QUuid uuid, this->ui->groupBrowser->selectedUuids())
        {
            isDatabaseSelected = dynamic_cast<Database*>(db->findGroup(uuid, true));
        }
    }

    return isDatabaseSelected;
}

/*!
    Updates the state of interface so that only widgets and menu items
    that are applicable to the current application state are enabled.
 */
void MainWindow::updateInterfaceState()
{
    // A few variables to reduce verbosity
    const bool isDatabaseSelected = this->isDatabaseSelected();
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
#ifdef Q_OS_WIN
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

    this->m_searchBox->setEnabled(hasDocument && !locked);
    this->m_searchButton->setEnabled(hasDocument && !locked);

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
        QString nodeCount = QString("<b>%1:</b> %2 %3 / %4, %5 %6 / %7")
            .arg(group->title())
            .arg(tr("Groups:"))
            .arg(group->groups().count())
            .arg(group->countGroups())
            .arg(tr("Entries:"))
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
    this->m_nodeCountStatusLabel->setText(QString(tr("<b>Search results:</b> (\"%1\"), %2 entries found")).arg(keywords).arg(entries.count()));
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
            pairs.append(QString(tr("<b>Group:</b> %1")).arg(Qt::escape(entry->parentNode()->title())));
        }

        if (!entry->title().isEmpty())
        {
            pairs.append(QString(tr("<b>Title:</b> %1")).arg(Qt::escape(entry->title())));
        }

        if (!entry->url().isEmpty())
        {
            pairs.append(QString("<b>URL:</b> <a href=\"%1\">%1</a>").arg(Qt::escape(entry->url().toString())));
        }

        if (!entry->username().isEmpty())
        {
            pairs.append(QString(tr("<b>Username:</b> %1")).arg(Qt::escape(entry->username())));
        }

        if (!entry->password().isEmpty())
        {
            pairs.append(QString(tr("<b>Password:</b> %1")).arg(Qt::escape(entry->password())));
        }

        // Custom fields go after the primary data but before the unmodifiable details
        QMapIterator<QString, QString> j(entry->customFields());
        while (j.hasNext())
        {
            j.next();
            pairs.append(QString("<b>%1:</b> %2").arg(Qt::escape(j.key())).arg(Qt::escape(j.value())));
        }

        // We'll always have a UUID, and the created/accessed/modified times
        pairs.append(QString("<b>UUID:</b> %1").arg(Qt::escape(entry->uuid().toString())));
        pairs.append(QString(tr("<b>Created:</b> %1")).arg(Qt::escape(entry->created().toLocalTime().toString(Qt::SystemLocaleLongDate))));
        pairs.append(QString(tr("<b>Accessed:</b> %1")).arg(Qt::escape(entry->accessed().toLocalTime().toString(Qt::SystemLocaleLongDate))));
        pairs.append(QString(tr("<b>Modified:</b> %1")).arg(Qt::escape(entry->modifiedTime().toLocalTime().toString(Qt::SystemLocaleLongDate))));

        // Last accessed... NOW!
        entry->setAccessed();

        // Add the recovery questions...
        QString recoveryString;
        QMapIterator<QString, QString> i(entry->recoveryInfo());
        while (i.hasNext())
        {
            i.next();
            recoveryString += QString("<li><b>%1</b>: %2</li>").arg(Qt::escape(i.key())).arg(Qt::escape(i.value()));
        }

        QString info = QString("<p>%1</p> <ul>%2</ul> <hr /> <p>%3</p>")
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
            return this->save();
        }

        QMessageBox::StandardButtons buttons = QMessageBox::Discard | QMessageBox::Cancel;

        // It's important for security to do this here, because if the user locks a database
        // and decides to discard changes for the moment, another user could later come along
        // and close the window and save the changes the original user intended to discard...
        if (!this->m_documentState.isLocked())
        {
            buttons |= QMessageBox::Save;
        }

        QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("Save Changes"),
            tr("The database has been modified.\nDo you want to save your changes?"), buttons);
        if (ret == QMessageBox::Save)
        {
            return this->save();
        }
        else if (ret == QMessageBox::Cancel)
        {
            return false;
        }
    }

    return true;
}

void MainWindow::loadFileInWindow(const QString &fileName)
{
    // If this file is already open in another window, show that window
    MainWindow *existing = this->findMainWindow(fileName);
    if (existing)
    {
        existing->show();
        existing->raise();
        existing->activateWindow();
        return;
    }

    // If this window does not have a file loaded and hasn't been modified,
    // open the file in this window
    if (!this->m_documentState.hasDocument())
    {
        this->loadFile(fileName);
    }
    else
    {
        // Otherwise create a new window to open it
        MainWindow *other = new MainWindow(this->m_filter);
        other->loadFile(fileName);

        // If it didn't load successfully, bail out
        if (!other->m_documentState.hasDocument())
        {
            delete other;
            return;
        }

        other->move(this->x() + 40, this->y() + 40);
        other->show();
    }
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
            QMessageBox::critical(this, tr("Error"), tr("Cannot read file %1:\n%2.").arg(fileName).arg(file.errorString()));
            return;
        }

        DatabaseAuthenticationDialog dialog(this);
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
                QMessageBox::critical(this, tr("Error"), reader.errorString());
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
            QMessageBox::critical(this, tr("Error"), tr("Cannot write file %1:\n%2").arg(fileName).arg(file.errorString()));
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
            QMessageBox::critical(this, tr("Error"), tr("Cannot write file %1").arg(fileName));
        }
    }

    return false;
}

QString MainWindow::closeFile()
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
        SilverlockPreferences::instance().pushRecentFile(fileName);

        this->updateRecentFileActionsAll();
    }

    this->setWindowTitle(QString());
    this->setWindowModified(false);
    this->setWindowFilePath(this->m_documentState.currentFile());
}

void MainWindow::clearCurrentFile()
{
    this->m_documentState.close();
    this->setWindowTitle(VER_PRODUCTNAME_STR);
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
    const int numRecentFiles = qMin(files.size(), maxFiles);
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

/*!
    Finds the window which has the file with the specified file name open.
    Returns NULL if the file is not opened by any window.

    \param fileName The name of the file to find the open window of.
 */
MainWindow* MainWindow::findMainWindow(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QWidget *widget, qApp->topLevelWidgets())
    {
        MainWindow *mainWin = qobject_cast<MainWindow*>(widget);
        if (mainWin && mainWin->m_documentState.currentFile() == canonicalFilePath)
        {
            return mainWin;
        }
    }

    return NULL;
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
            QMessageBox::critical(this, tr("Error"), tr("The password you entered is incorrect. Please verify that you have entered it correctly."));
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
