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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), m_preferences(new SilverlockPreferences()), m_database(new Database("New Database", QString()))
{
    this->ui->setupUi(this);
    this->setupDocumentState();
    this->setupSignals();
    this->setupUiAdditional();
    this->setupKeyboardShortcuts();
    this->setupMenuIcons();

    QObject::connect(this->m_database, SIGNAL(modified()), this, SLOT(databaseWasModified()));
    this->ui->groupBrowser->populate(this->m_database);
}

MainWindow::~MainWindow()
{
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

    if (this->m_database)
    {
        delete this->m_database;
        this->m_database = NULL;
    }
}

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

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            this->setVisible(!this->isVisible());
            break;
        default:
            break;
    }
}

/*!
    Performs initialization related to the state of files loaded by the application.
 */
void MainWindow::setupDocumentState()
{
    this->m_isUntitled = true;
    this->setCurrentFile(QString());

    // We need to do this here because the window title
    // will be in all lowercase if we let Qt handle it
    this->setWindowTitle(VER_PRODUCTNAME_STR);
}

void MainWindow::setupSignals()
{
    QObject::connect(this->ui->actionSave, SIGNAL(triggered()), SLOT(save()));
    QObject::connect(this->ui->actionSaveAs, SIGNAL(triggered()), SLOT(saveAs()));
    QObject::connect(this->ui->groupBrowser, SIGNAL(populating()), SLOT(clearViews()));
    QObject::connect(this->ui->entryTable, SIGNAL(populating()), SLOT(clearViews()));

    QObject::connect(this->ui->actionSelectAllEntries, SIGNAL(triggered()), this->ui->entryTable, SLOT(selectAll()));
    QObject::connect(this->ui->actionSelectAllGroups, SIGNAL(triggered()), this->ui->groupBrowser, SLOT(selectAll()));

    QObject::connect(this->ui->actionShowEntryView, SIGNAL(toggled(bool)), this->ui->infoView, SLOT(setVisible(bool)));
}

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
    this->ui->entryTable->setPasswordsShown(false);
    this->updateNodeActions();

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
    this->ui->actionExit->setIcon(QIcon::fromTheme("application-exit"));
    this->ui->actionPreferences->setIcon(QIcon::fromTheme("preferences-other"));
    this->ui->actionAboutSilverlock->setIcon(QIcon::fromTheme("help-about"));
}

void MainWindow::on_actionNew_triggered()
{
    MainWindow *other = new MainWindow();
    other->move(this->x() + 40, this->y() + 40);
    other->show();
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this);
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
        if (this->m_isUntitled && !this->isWindowModified())
        {
            this->loadFile(fileName);
        }
        else
        {
            // Otherwise create a new window to open it
            MainWindow *other = new MainWindow();
            other->loadFile(fileName);

            // If it didn't load successfully, bail out
            if (other->m_isUntitled)
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
    this->close();
}

bool MainWindow::save()
{
    // If the open document doesn't have a file name set, prompt for one
    if (this->m_isUntitled)
    {
        return this->saveAs();
    }
    else
    {
        // Otherwise go ahead and save the file
        return this->saveFile(this->m_currentFile);
    }
}

bool MainWindow::saveAs()
{
    // Prompt the user for a file name, and if it was empty, bail out
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), this->m_currentFile);
    if (fileName.isEmpty())
    {
        return false;
    }

    // Otherwise save the file normally
    return this->saveFile(fileName);
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
    QList<QUuid> uuids = this->ui->entryTable->selectedUuids();
    if (uuids.count() > 0)
    {
        // All this makes sure all the entries have the same parent,
        // and there's no reason they shouldn't, but we'll check anyways
        Group *firstParent = NULL;
        foreach (QUuid uuid, uuids)
        {
            Entry *entry = this->m_database->findEntry(uuid);
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
        dialog.populate(this->m_database);
        if (dialog.exec() == QDialog::Accepted)
        {
            Group *group = this->m_database->findGroup(dialog.selectedUuid(), true);
            if (group)
            {
                foreach (QUuid uuid, uuids)
                {
                    Entry *entry = this->m_database->findEntry(uuid);
                    if (entry)
                    {
                        entry->setParentNode(group);
                    }
                }

                this->ui->entryTable->populate(firstParent);
            }
        }
    }
}

void MainWindow::on_actionDuplicateEntries_triggered()
{
    QList<QUuid> uuids = this->ui->entryTable->selectedUuids();
    foreach (QUuid uuid, uuids)
    {
        Entry *entry = this->m_database->findEntry(uuid);
        if (entry)
        {
            Group *parent = entry->parentNode();
            if (parent)
            {
                Entry *copy = entry->createCopy();
                copy->setTitle(copy->title() + tr(" - Copy"));
                copy->setParentNode(parent);

                this->ui->entryTable->populate(parent);
            }
        }
    }
}

void MainWindow::on_actionEditEntry_triggered()
{
    QUuid uuid = this->ui->entryTable->selectedUuid();
    if (!uuid.isNull() && this->m_database)
    {
        Entry *entry = this->m_database->findEntry(uuid);
        if (entry)
        {
            EntryEditDialog dialog(entry, this);
            if (dialog.exec() == QDialog::Accepted)
            {
                this->ui->groupBrowser->populate(this->m_database);
                this->ui->entryTable->populate(entry->parentNode());
                this->setNodeCount(entry->parentNode());
                this->populateInfoView(entry);
            }
        }
    }
}

void MainWindow::on_actionDeleteEntries_triggered()
{
    int result = QMessageBox::warning(this, tr("Delete Entries"),
        tr("Are you sure you want to delete the selected entries?"),
        QMessageBox::Yes, QMessageBox::No);
    if (result == QMessageBox::Yes)
    {
        Group *parent = NULL;

        QList<QUuid> uuids = this->ui->entryTable->selectedUuids();
        foreach (QUuid uuid, uuids)
        {
            Entry* entry = this->m_database->findEntry(uuid);
            if (entry)
            {
                parent = entry->parentNode();
                delete entry;
            }
        }

        this->ui->entryTable->populate(parent);
    }
}

void MainWindow::on_actionAddSubgroup_triggered()
{
    QUuid uuid = this->ui->groupBrowser->selectedUuid();
    if (!uuid.isNull() && this->m_database)
    {
        Group *group = this->m_database->findGroup(uuid, true);
        if (group)
        {
            Group *newGroup = new Group();

            GroupEditDialog dialog(newGroup, this);
            if (dialog.exec() == QDialog::Accepted)
            {
                newGroup->setParentNode(group);
                this->ui->groupBrowser->populate(this->m_database);
                this->ui->entryTable->populate(group);
            }
            else
            {
                delete newGroup;
            }
        }
    }
}

void MainWindow::on_actionAddEntry_triggered()
{
    QUuid uuid = this->ui->groupBrowser->selectedUuid();
    if (!uuid.isNull() && this->m_database)
    {
        Group *group = this->m_database->findGroup(uuid, true);
        if (group)
        {
            Entry *newEntry = new Entry();

            EntryEditDialog dialog(newEntry, this);
            if (dialog.exec() == QDialog::Accepted)
            {
                newEntry->setParentNode(group);
                this->ui->groupBrowser->populate(this->m_database);
                this->ui->entryTable->populate(group);
            }
            else
            {
                delete newEntry;
            }
        }
    }
}

void MainWindow::on_actionMoveGroup_triggered()
{
    QList<QUuid> uuids = this->ui->groupBrowser->selectedUuids();
    if (uuids.count() == 1)
    {
        Group *sourceGroup = this->m_database->findGroup(uuids.first());
        if (sourceGroup)
        {
            SelectGroupDialog dialog(this);
            dialog.setPreferences(this->m_preferences);
            dialog.populate(this->m_database);
            if (dialog.exec() == QDialog::Accepted)
            {
                if (sourceGroup->findGroup(dialog.selectedUuid(), true))
                {
                    QMessageBox::critical(this, tr("Error"), tr("A group cannot be moved to itself or one of its subgroups."));
                }
                else
                {
                    Group *targetGroup = this->m_database->findGroup(dialog.selectedUuid(), true);
                    if (targetGroup)
                    {
                        if (sourceGroup->parentNode() == targetGroup)
                        {
                            QMessageBox::critical(this, tr("Error"), tr("The source and destination group are the same."));
                        }
                        else
                        {
                            sourceGroup->setParentNode(targetGroup);
                            this->ui->groupBrowser->populate(this->m_database);
                        }
                    }
                }
            }
        }
    }
}

void MainWindow::on_actionEditGroup_triggered()
{
    QUuid uuid = this->ui->groupBrowser->selectedUuid();
    if (!uuid.isNull() && this->m_database)
    {
        Group *group = this->m_database->findGroup(uuid, true);
        if (group)
        {
            QDialog *dialog;
            Database *db = qobject_cast<Database*>(group);
            if (db)
            {
                QMessageBox::critical(this, tr("Error"), "Cannot edit databases");
                return;
            }
            else
            {
                dialog = new GroupEditDialog(group, this);
            }

            if (dialog->exec() == QDialog::Accepted)
            {
                this->ui->groupBrowser->populate(this->m_database);
                this->ui->entryTable->populate(group);
                this->setNodeCount(group);
            }

            delete dialog;
        }
    }
}

void MainWindow::on_actionDeleteGroups_triggered()
{
    int result = QMessageBox::warning(this, tr("Delete Groups"),
        tr("Are you sure you want to delete the selected groups?"),
        QMessageBox::Yes, QMessageBox::No);
    if (result == QMessageBox::Yes)
    {
        QList<QUuid> uuids = this->ui->groupBrowser->selectedUuids();
        foreach (QUuid uuid, uuids)
        {
            Group* group = this->m_database->findGroup(uuid);
            if (group)
            {
                delete group;
            }
        }

        this->ui->groupBrowser->populate(this->m_database);
    }
}

void MainWindow::on_actionFind_triggered()
{
    SearchDatabaseDialog dialog(this->m_database, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        this->populateWithSearchResults(dialog.entriesFound(), dialog.searchParameters()->searchPattern);
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

void MainWindow::on_actionShowPasswords_triggered(bool checked)
{
    this->ui->entryTable->setPasswordsShown(checked);
    this->ui->infoView->clear();
    this->on_entryTable_itemSelectionChanged();
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
    this->updateNodeActions();

    // Safety and logic checks
    if (!this->m_database || this->ui->groupBrowser->selectedUuids().count() > 1)
    {
        this->clearViews();
        return;
    }

    // Get the selected group UUID
    QUuid uuid = this->ui->groupBrowser->selectedUuid();
    if (!uuid.isNull())
    {
        // Find the group for this UUID and populate the entry table with it
        Group *group = this->m_database->findGroup(uuid, true);
        if (group)
        {
            this->ui->entryTable->populate(group);
            this->setNodeCount(group);
        }
        else
        {
            // If we couldn't find the group, just clear the entry table
            this->setNodeCount(NULL);
            this->ui->entryTable->clear();
        }
    }
}

/*!
    Populates the info view with the details of the entry selected in the right hand detail view.
 */
void MainWindow::on_entryTable_itemSelectionChanged()
{
    this->updateNodeActions();

    // Safety and logic checks
    if (!this->m_database || this->ui->entryTable->selectedUuids().count() > 1)
    {
        this->ui->infoView->clear();
        return;
    }

    // Get the selected entry UUID
    QUuid uuid = this->ui->entryTable->selectedUuid();
    if (!uuid.isNull())
    {
        // Find the entry for this UUID and update the info view with its data
        Entry *entry = this->m_database->findEntry(uuid);
        if (entry)
        {
            this->populateInfoView(entry);
        }
        else
        {
            // If we couldn't find the entry, just clear the info view
            this->ui->infoView->clear();
        }
    }
}

/*!
    Tells the window that the database was modified.
 */
void MainWindow::databaseWasModified()
{
    this->setWindowModified(true);
}

void MainWindow::clearViews()
{
    this->setNodeCount(NULL);
    this->ui->entryTable->clear();
    this->ui->infoView->clear();
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

/*!
    Updates the actions in the group and entry menus to reflect
    the abilities of the current selection state.
 */
void MainWindow::updateNodeActions()
{
    bool isDatabaseSelected = false;
    if (this->m_database)
    {
        foreach (QUuid uuid, this->ui->groupBrowser->selectedUuids())
        {
            isDatabaseSelected = qobject_cast<Database*>(this->m_database->findGroup(uuid, true));
        }
    }

    int groups = this->ui->groupBrowser->selectedUuids().count();
    int entries = this->ui->entryTable->selectedUuids().count();

    this->ui->actionAddSubgroup->setEnabled(groups == 1);
    this->ui->actionAddEntry->setEnabled(groups == 1);
    this->ui->actionMoveGroup->setEnabled(groups == 1 && !isDatabaseSelected);
    this->ui->actionEditGroup->setEnabled(groups == 1);
    this->ui->actionDeleteGroups->setEnabled(groups > 0 && !isDatabaseSelected);

    this->ui->actionMoveEntries->setEnabled(entries > 0);
    this->ui->actionDuplicateEntries->setEnabled(entries > 0);
    this->ui->actionEditEntry->setEnabled(entries == 1);
    this->ui->actionDeleteEntries->setEnabled(entries > 0);
}

void MainWindow::populateInfoView(Entry *const entry)
{
    QStringList pairs;
    pairs.append(QString("<b>%1</b> %2").arg(tr("Group:")).arg(entry->parentNode()->title()));
    pairs.append(QString("<b>%1</b> %2").arg(tr("Title:")).arg(entry->title()));
    pairs.append(QString("<b>%1</b> <a href=\"%2\">%2</a>").arg("URL:").arg(entry->url().toString()));
    pairs.append(QString("<b>%1</b> %2").arg(tr("Username:")).arg(entry->username()));
    pairs.append(QString("<b>%1</b> %2").arg(tr("Password:")).arg(this->ui->entryTable->passwordsShown() ? entry->password() : "********"));
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

void MainWindow::populateWithSearchResults(const QList<Entry*> &entries, const QString &keywords)
{
    this->ui->entryTable->populate(entries);
    this->m_nodeCountStatusLabel->setText(QString("<b>Search results:</b> (\"%1\"), %2 entries found").arg(keywords).arg(entries.count()));
}

/*!
    Check if the database needs to be saved, and prompt the user for a choice.

    \return Whether the window should be closed.
 */
bool MainWindow::maybeSave()
{
    if (this->isWindowModified())
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
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, tr("Error"), tr("Cannot read file %1:\n%2.").arg(fileName).arg(file.errorString()));
        return;
    }

    bool ok;
    QString str = QInputDialog::getText(this, tr("Enter password"), tr("Password:"), QLineEdit::Password, QString(), &ok);
    if (ok)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        // Try to read the database, if it succeeds, populate the tree view, otherwise show an error message
        DatabaseReader reader;
        this->m_database = reader.read(file, str);
        if (this->m_database)
        {
            QObject::connect(this->m_database, SIGNAL(modified()), this, SLOT(databaseWasModified()));
            this->ui->groupBrowser->populate(this->m_database);
            QApplication::restoreOverrideCursor();

            this->setCurrentFile(fileName);
            this->statusBar()->showMessage(tr("File loaded"), 2000);
        }
        else
        {
            QApplication::restoreOverrideCursor();
            QMessageBox::critical(this, tr("Error"), QString("%1\n\n%2").arg(tr("The database file is in an incorrect format, or corrupted."), reader.errorString()));
        }
    }
}

/*!
    Attempts to write the database file to \a fileName.

    \param fileName The file to write the database to.
 */
bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, tr("Error"), tr("Cannot write file %1:\n%2").arg(fileName).arg(file.errorString()));
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    // Try to write the database to the IO device
    bool status = false;
    DatabaseWriter writer;
    if (writer.write(this->m_database, file))
    {
        QApplication::restoreOverrideCursor();

        // If it succeeded, we set the filename to the name of the file we saved and show a status message
        this->setCurrentFile(fileName);
        this->statusBar()->showMessage(tr("File saved"), 2000);
        status = true;
    }
    else
    {
        // Otherwise show a failure message
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, tr("Silverlock"), tr("Cannot write file %1").arg(fileName));
    }

    return status;
}

/*!
    Sets the name of the currently loaded file to \a fileName.

    \param fileName The name of the file to set.
 */
void MainWindow::setCurrentFile(const QString &fileName)
{
    static int sequenceNumber = 1;

    this->m_isUntitled = fileName.isEmpty();
    if (this->m_isUntitled)
    {
        this->m_currentFile = tr("database%1.adb").arg(sequenceNumber++);
    }
    else
    {
        this->m_currentFile = QFileInfo(fileName).canonicalFilePath();
    }

    this->setWindowTitle(QString());
    this->setWindowModified(false);
    this->setWindowFilePath(this->m_currentFile);
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
        if (mainWin && mainWin->m_currentFile == canonicalFilePath)
        {
            return mainWin;
        }
    }

    return NULL;
}
