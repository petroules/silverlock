#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <silverlocklib.h>
#include "aboutdialog.h"
#include "entryeditdialog.h"
#include "groupeditdialog.h"
#include "preferencesdialog.h"
#include "searchdatabasedialog.h"
#include "selectgroupdialog.h"
#include "silverlockpreferences.h"
#include "version.h"
#include "configurecolumnsdialog.h"

/*!
    \class MainWindow

    The MainWindow class represents the main window of the application.
 */

/*!
    Constructs a new MainWindow.
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), m_preferences(new SilverlockPreferences()), m_nodeCountStatusLabel(NULL)
{
    this->ui->setupUi(this);
    this->clearCurrentFile();
    this->setupSignals();
    this->setupUiAdditional();
    this->setupKeyboardShortcuts();
    this->setupMenuIcons();
}

/*!
    Destroys the MainWindow.
 */
MainWindow::~MainWindow()
{
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

    if (this->m_preferences)
    {
        delete this->m_preferences;
        this->m_preferences = NULL;
    }
}

/*!
    \internal
 */
void MainWindow::changeEvent(QEvent* e)
{
    QMainWindow::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange:
            this->ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

/*!
    \internal
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    // Do the prompting and saving, and it'll tell us whether the window should be closed or not
    if (this->maybeSave())
    {
        this->m_preferences->save();
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
            this->setVisible(!this->isVisible());
            break;
        default:
            break;
    }
}

/*!
    Connects various slots and signals.
 */
void MainWindow::setupSignals()
{
    QObject::connect(this->ui->actionSave, SIGNAL(triggered()), SLOT(save()));
    QObject::connect(this->ui->actionSaveAs, SIGNAL(triggered()), SLOT(saveAs()));
    QObject::connect(this->ui->groupBrowser, SIGNAL(populating()), SLOT(clearViews()));
    QObject::connect(this->ui->entryTable, SIGNAL(populating()), SLOT(clearViews()));

    QObject::connect(this->ui->actionSelectAllEntries, SIGNAL(triggered()), this->ui->entryTable, SLOT(selectAll()));
    QObject::connect(this->ui->actionSelectAllGroups, SIGNAL(triggered()), this->ui->groupBrowser, SLOT(selectAll()));

    QObject::connect(this->ui->actionShowEntryView, SIGNAL(toggled(bool)), this->ui->infoView, SLOT(setVisible(bool)));

    QObject::connect(&this->m_documentState, SIGNAL(stateChanged()), this, SLOT(updateInterfaceState()));
    QObject::connect(&this->m_documentState, SIGNAL(modified()), this, SLOT(databaseWasModified()));
}

/*!
    Performs additional UI setup; creation of widgets and setting of properties.
 */
void MainWindow::setupUiAdditional()
{
    QSystemTrayIcon *icon = new QSystemTrayIcon(this);
    icon->setIcon(this->windowIcon());
    icon->setToolTip(this->windowTitle());
    icon->setContextMenu(this->ui->menuFile);
    icon->show();
    QObject::connect(icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setUnifiedTitleAndToolBarOnMac(true);

    this->statusBar()->showMessage(tr("Ready"));
    this->statusBar()->addPermanentWidget(this->m_nodeCountStatusLabel = new QLabel(this));

    this->ui->groupBrowser->setPreferences(this->m_preferences);
    this->ui->entryTable->setPreferences(this->m_preferences);
    this->updateInterfaceState();

    // Add actions for toolbars
    this->ui->menuToolbars->addAction(this->ui->mainToolBar->toggleViewAction());

    // Set menu actions to correct initial states
    this->ui->actionShowEntryView->setChecked(true);
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

void MainWindow::on_actionNew_triggered()
{
    if (this->m_documentState.hasDocument())
    {
        MainWindow *other = new MainWindow();
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
            MainWindow *other = new MainWindow();
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
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), this->m_documentState.currentFile(), tr("Silverlock databases (*.sdbx)"));
        if (fileName.isEmpty())
        {
            return false;
        }

        // Otherwise save the file normally
        return this->saveFile(fileName);
    }

    return true;
}

void MainWindow::on_actionPrint_triggered()
{

}

void MainWindow::on_actionPrintPreview_triggered()
{

}

void MainWindow::on_actionExit_triggered()
{
    QApplication::closeAllWindows();
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
            dialog.setPreferences(this->m_preferences);
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
                dialog.setPreferences(this->m_preferences);
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

void MainWindow::on_actionAlwaysOnTop_triggered(bool checked)
{

}

void MainWindow::on_actionCenterToScreen_triggered()
{
    // Move the application window to the center of the screen
    QRect rect = this->frameGeometry();
    rect.moveCenter(QDesktopWidget().availableGeometry().center());
    this->move(rect.topLeft());
}

void MainWindow::on_actionConfigureColumns_triggered()
{
    //this->ui->entryTable->setPasswordsShown(checked);
    //this->ui->infoView->clear();
    //this->on_entryTable_itemSelectionChanged();

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
    PreferencesDialog preferences(this->m_preferences, this);
    preferences.exec();
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

}

void MainWindow::on_actionAboutSilverlock_triggered()
{
    QString str = tr("<p>Silverlock is a secure password management program designed to ease the management of account credentials and related information.</p>");
    str.append(tr("<p>The padlock icon used in this program is a public domain image. Details can be found <a href=\"http://www.openclipart.org/detail/33505\">here</a>.</p>"));
    AboutDialog::show(this, tr("About Silverlock"), str);
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
        if (this->ui->groupBrowser->selectedUuids().count() > 1)
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
        if (this->ui->entryTable->selectedUuids().count() > 1)
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
    Updates the state of interface so that only widgets and menu items
    that are applicable to the current application state are enabled.
 */
void MainWindow::updateInterfaceState()
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

    int groups = this->ui->groupBrowser->selectedUuids().count();
    int entries = this->ui->entryTable->selectedUuids().count();

    // Update all the menu actions
    this->ui->actionClose->setEnabled(this->m_documentState.hasDocument());
    this->ui->actionSave->setEnabled(this->m_documentState.hasDocument());
    this->ui->actionSaveAs->setEnabled(this->m_documentState.hasDocument());
    this->ui->actionPrint->setEnabled(this->m_documentState.hasDocument());
    this->ui->actionPrintPreview->setEnabled(this->m_documentState.hasDocument());

    this->ui->actionAddSubgroup->setEnabled(groups == 1);
    this->ui->actionAddEntry->setEnabled(groups == 1);
    this->ui->actionMoveGroup->setEnabled(groups == 1 && !isDatabaseSelected);
    this->ui->actionEditGroup->setEnabled(groups == 1);
    this->ui->actionDeleteGroups->setEnabled(groups > 0 && !isDatabaseSelected);

    this->ui->actionMoveEntries->setEnabled(entries > 0);
    this->ui->actionDuplicateEntries->setEnabled(entries > 0);
    this->ui->actionEditEntry->setEnabled(entries == 1);
    this->ui->actionDeleteEntries->setEnabled(entries > 0);

    // Update the main widgets
    this->ui->groupBrowser->setEnabled(this->m_documentState.hasDocument());
    this->ui->entryTable->setEnabled(this->m_documentState.hasDocument());
    this->ui->infoView->setEnabled(this->m_documentState.hasDocument());
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
    this->ui->entryTable->clear();
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
    this->m_nodeCountStatusLabel->setText(QString("<b>Search results:</b> (\"%1\"), %2 entries found").arg(keywords).arg(entries.count()));
}

void MainWindow::populateEntryTable(Group *const group)
{
    if (group)
    {
        this->ui->entryTable->populate(group);
        this->setNodeCount(group);
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
        pairs.append(QString("<b>%1</b> %2").arg(tr("Group:")).arg(entry->parentNode()->title()));
        pairs.append(QString("<b>%1</b> %2").arg(tr("Title:")).arg(entry->title()));
        pairs.append(QString("<b>%1</b> <a href=\"%2\">%2</a>").arg("URL:").arg(entry->url().toString()));
        pairs.append(QString("<b>%1</b> %2").arg(tr("Username:")).arg(entry->username()));
        pairs.append(QString("<b>%1</b> %2").arg(tr("Password:")).arg(entry->password()));
        pairs.append(QString("<b>%1</b> %2").arg(tr("Email address:")).arg(entry->emailAddress()));
        pairs.append(QString("<b>%1</b> %2").arg("UUID:").arg(entry->uuid().toString()));

        QString recoveryString;
        QHashIterator<QString, QString> i(entry->recoveryInfo());
        while (i.hasNext())
        {
            i.next();
            recoveryString += QString("<li><b>%1</b>: %2</li>").arg(i.key()).arg(i.value());
        }

        QString info = QString("<p>%1</p><ul>%2</ul><p>%3</p>")
            .arg(pairs.join(", "))
            .arg(recoveryString)
            .arg(entry->notes().replace("\n", "<br />"));
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
        QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("Save Changes"),
            tr("The database has been modified.\nDo you want to save your changes?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
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

        bool ok;
        QString str = QInputDialog::getText(this, tr("Enter database password"), tr("Database password:"), QLineEdit::Password, QString(), &ok);
        if (ok)
        {
            QApplication::setOverrideCursor(Qt::WaitCursor);

            // Try to read the database, if it succeeds, populate the tree view, otherwise show an error message
            DatabaseReader reader;
            this->m_documentState.load(reader.read(file, str));
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
 */
bool MainWindow::saveFile(const QString &fileName)
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
        if (writer.write(this->m_documentState.database(), file))
        {
            QApplication::restoreOverrideCursor();

            // If it succeeded, we set the filename to the name of the file we saved and show a status message
            this->setCurrentFile(fileName);
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

void MainWindow::closeFile()
{
    if (this->m_documentState.hasDocument())
    {
        this->clearCurrentFile();

        this->ui->groupBrowser->clear();
        this->clearViews();
    }
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
