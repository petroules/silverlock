#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <silverlocklib.h>
#include "aboutdialog.h"
#include "entryeditdialog.h"
#include "groupeditdialog.h"
#include "preferencesdialog.h"
#include "silverlockpreferences.h"
#include "version.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), m_preferences(new SilverlockPreferences()), m_database(NULL)
{
    // Set information about the application
    QCoreApplication::setOrganizationName(VER_COMPANYNAME_STR);
    QCoreApplication::setOrganizationDomain(VER_COMPANYDOMAIN_STR);
    QCoreApplication::setApplicationName(VER_PRODUCTNAME_STR);
    QCoreApplication::setApplicationVersion(VER_PRODUCTVERSION_STR);

    // Setup the user interface
    this->ui->setupUi(this);

    // Initialize the document state
    this->m_isUntitled = true;
    this->setCurrentFile(QString());
    this->setWindowTitle(VER_PRODUCTNAME_STR);

    // User interface
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setUnifiedTitleAndToolBarOnMac(true);
    this->statusBar()->showMessage(tr("Ready"));
    this->statusBar()->addPermanentWidget(this->m_nodeCountStatusLabel = new QLabel(this));
    this->ui->groupBrowser->setPreferences(this->m_preferences);
    this->ui->entryTable->setPreferences(this->m_preferences);
    this->updateNodeActions();

    // Signals and slots
    QObject::connect(this->ui->action_Save, SIGNAL(triggered()), SLOT(save()));
    QObject::connect(this->ui->actionSave_As, SIGNAL(triggered()), SLOT(saveAs()));
    QObject::connect(this->ui->groupBrowser, SIGNAL(populating()), SLOT(clearViews()));
    QObject::connect(this->ui->entryTable, SIGNAL(populating()), SLOT(clearViews()));

    // Keyboard Shortcuts
    this->ui->action_New->setShortcut(QKeySequence::New);
    this->ui->action_Open->setShortcut(QKeySequence::Open);
    this->ui->actionClose->setShortcut(QKeySequence::Close);
    this->ui->action_Save->setShortcut(QKeySequence::Save);
    this->ui->actionSave_As->setShortcut(QKeySequence::SaveAs);
    this->ui->action_Print->setShortcut(QKeySequence::Print);
    this->ui->actionE_xit->setShortcut(QKeySequence::Quit);
    this->ui->actionOptions->setShortcut(QKeySequence::Preferences);

#ifdef Q_WS_X11
    // X11 action icons
    this->ui->actionE_xit->setIcon(QIcon::fromTheme("application-exit"));
    this->ui->actionOptions->setIcon(QIcon::fromTheme("preferences-other"));
    this->ui->actionAbout->setIcon(QIcon::fromTheme("help-about"));
#endif
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

void MainWindow::on_action_New_triggered()
{
    MainWindow *other = new MainWindow();
    other->move(this->x() + 40, this->y() + 40);
    other->show();
}

void MainWindow::on_action_Open_triggered()
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

void MainWindow::on_action_Print_triggered()
{

}

void MainWindow::on_actionPrint_Preview_triggered()
{

}

void MainWindow::on_actionE_xit_triggered()
{
    QApplication::closeAllWindows();
}

void MainWindow::on_actionMove_Entries_triggered()
{

}

void MainWindow::on_actionEdit_Entry_triggered()
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
            }
        }
    }
}

void MainWindow::on_actionDelete_Entries_triggered()
{
    int result = QMessageBox::warning(this, tr("Delete Entries"),
        tr("Are you sure you want to delete the selected entries?"),
        QMessageBox::Yes, QMessageBox::No);
    if (result == QMessageBox::Yes)
    {
        QList<QUuid> uuids = this->ui->entryTable->selectedUuids();
        foreach (QUuid uuid, uuids)
        {
            Entry* entry = this->m_database->findEntry(uuid);
            if (entry)
            {
                delete entry;
            }
        }
    }
}

void MainWindow::on_actionAdd_Subgroup_triggered()
{
    QUuid uuid = this->ui->groupBrowser->selectedUuid();
    if (!uuid.isNull() && this->m_database)
    {
        Group *group = this->m_database->findGroup(uuid);
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

void MainWindow::on_actionAdd_Entry_triggered()
{
    QUuid uuid = this->ui->groupBrowser->selectedUuid();
    if (!uuid.isNull() && this->m_database)
    {
        Group *group = this->m_database->findGroup(uuid);
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

void MainWindow::on_actionMove_Groups_triggered()
{

}

void MainWindow::on_actionEdit_Group_triggered()
{
    QUuid uuid = this->ui->groupBrowser->selectedUuid();
    if (!uuid.isNull() && this->m_database)
    {
        Group *group = this->m_database->findGroup(uuid);
        if (group)
        {
            GroupEditDialog dialog(group, this);
            if (dialog.exec() == QDialog::Accepted)
            {
                this->ui->groupBrowser->populate(this->m_database);
                this->ui->entryTable->populate(group);
            }
        }
    }
}

void MainWindow::on_actionDelete_Groups_triggered()
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

void MainWindow::on_actionAlways_on_Top_triggered(bool checked)
{

}

void MainWindow::on_actionCenter_to_Screen_triggered()
{
    // Move the application window to the center of the screen
    QRect rect = this->frameGeometry();
    rect.moveCenter(QDesktopWidget().availableGeometry().center());
    this->move(rect.topLeft());
}

void MainWindow::on_actionShow_Passwords_triggered(bool checked)
{

}

void MainWindow::on_actionCheck_for_Updates_triggered()
{

}

void MainWindow::on_actionOptions_triggered()
{
    PreferencesDialog preferences(this->m_preferences, this);
    preferences.exec();
}

void MainWindow::on_actionAbout_triggered()
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
        Group *group = this->m_database->findGroup(uuid);
        if (group)
        {
            this->ui->entryTable->populate(group);

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
            // If we couldn't find the group, just clear the entry table
            this->m_nodeCountStatusLabel->setText(QString());
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
    this->m_nodeCountStatusLabel->setText(QString());
    this->ui->entryTable->clear();
    this->ui->infoView->clear();
}

/*!
    Updates the actions in the group and entry menus to reflect
    the abilities of the current selection state.
 */
void MainWindow::updateNodeActions()
{
    int groups = this->ui->groupBrowser->selectedUuids().count();
    int entries = this->ui->entryTable->selectedUuids().count();

    this->ui->actionAdd_Subgroup->setEnabled(groups == 1);
    this->ui->actionAdd_Entry->setEnabled(groups == 1);
    this->ui->actionMove_Groups->setEnabled(groups > 0);
    this->ui->actionEdit_Group->setEnabled(groups == 1);
    this->ui->actionDelete_Groups->setEnabled(groups > 0);

    this->ui->actionMove_Entries->setEnabled(entries > 0);
    this->ui->actionEdit_Entry->setEnabled(entries == 1);
    this->ui->actionDelete_Entries->setEnabled(entries > 0);
}

void MainWindow::populateInfoView(Entry *const entry)
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
