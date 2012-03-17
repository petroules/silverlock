#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "silverlockapplication.h"
#include "silverlockpreferences.h"
#include "dialogs/configurecolumnsdialog.h"
#include "dialogs/databaseprintpreviewdialog.h"
#include "dialogs/entryeditdialog.h"
#include "dialogs/groupeditdialog.h"
#include "dialogs/newdatabasewizard.h"
#include "dialogs/searchdatabasedialog.h"
#include "dialogs/selectgroupdialog.h"
#include "widgets/itoolbarsearchwidget.h"
#include <silverlocklib.h>
#include <synteza.h>

void MainWindow::newFile()
{
    if (this->m_documentState.hasDocument())
    {
        MainWindow *other = new MainWindow();
        other->move(this->x() + 40, this->y() + 40);
        other->show();
    }
    else
    {
        NewDatabaseWizard wizard(this);
        if (wizard.exec() == QDialog::Accepted)
        {
            this->m_documentState.load(wizard.database());
            this->ui->groupBrowser->populate(wizard.database());
            this->setCurrentFile(QString());

            // Call this here so the user doesn't accidentally close
            // the window without saving their new database
            this->databaseWasModified();
        }
    }
}

void MainWindow::openFile()
{
    qsApp->openFile();
}

void MainWindow::closeFile()
{
    qsApp->closeFile();
}

bool MainWindow::saveFile()
{
    if (this->m_documentState.hasDocument())
    {
        // If the open document doesn't have a file name set, prompt for one
        if (this->m_documentState.isUntitled())
        {
            return this->saveFileAs();
        }
        else
        {
            // Otherwise go ahead and save the file
            return this->saveFile(this->m_documentState.currentFile());
        }
    }

    return true;
}

bool MainWindow::saveFileAs()
{
    if (this->m_documentState.hasDocument())
    {
        // Prompt the user for a file name, and if it was empty, bail out
        NativeSaveFileDialog dialog(this, tr("Save As"), this->m_documentState.currentFile(), tr("Silverlock databases (*.sdbx);;XML documents (*.xml)"));
        if (dialog.exec() == QDialog::Accepted)
        {
            QString fileName = dialog.selectedFile();
            if (fileName.isEmpty())
            {
                return false;
            }

            // Check if the user is using encryption...
            bool encrypt = !fileName.endsWith(".xml", Qt::CaseInsensitive);
            if (!encrypt)
            {
                if (NativeDialogs::warning(this, tr("Warning"), tr("<p><b>It is recommended that only advanced users select this option.</b></p><p>If you save the database in XML format, the contents will not be encrypted and any user with access to the file will be able to view the information in your database. It is strongly recommended that you save your files in Silverlock's SDBX format instead.</p><p>Are you sure you want save in the XML format?</p>"),
                    QString(), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
                {
                    return false;
                }
            }

            // Otherwise save the file normally
            return this->saveFile(fileName, encrypt);
        }

        return false;
    }

    return true;
}

void MainWindow::changeDatabasePassword()
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

void MainWindow::print()
{
    this->showPrintDialog();
}

void MainWindow::printPreview()
{
    if (this->m_documentState.hasDocument())
    {
        DatabasePrintPreviewDialog dialog(this->m_documentState.database(), this);
        if (dialog.exec() == QDialog::Accepted)
        {
            this->showPrintDialog(dialog.databasePrinterFields());
        }
    }
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::closeAllWindows();
}

void MainWindow::lockWorkspace()
{
    if (this->m_documentState.hasDocument() && !this->m_documentState.isLocked() && this->maybeSave())
    {
        this->m_documentState.lock();
        this->lockWorkspace(true);
    }
}

void MainWindow::unlockWorkspace()
{
    if (this->m_documentState.hasDocument() && this->m_documentState.isLocked())
    {
        this->lockWorkspace(false);
        this->m_documentState.unlock();
    }
}

void MainWindow::lockWorkspace(bool lock)
{
    // May contain sensitive terms
    this->m_toolbarSearch->setTextHidden(lock);
    this->ui->mainPage->setEnabled(!lock);
    this->ui->unlockPage->setEnabled(lock);
    this->m_nodeCountStatusLabel->setVisible(!lock);
    this->ui->stackedWidget->setCurrentWidget(lock ? this->ui->unlockPage : this->ui->mainPage);

    this->ui->groupBrowser->setVisible(!lock);
    this->ui->infoView->setVisible(!lock);

    this->ui->groupsDockWidget->setVisible(!lock);
    this->ui->infoDockWidget->setVisible(!lock);

    if (lock)
    {
        // Focus to the enter password line edit
        this->ui->unlockWorkspacePasswordLineEdit->setFocus();
        this->ui->unlockWorkspacePasswordLineEdit->setEchoMode(QLineEdit::Password);
        this->ui->unlockWorkspaceRevealToolButton->setChecked(true);

        if (SilverlockPreferences::instance().minimizeAfterLock())
        {
            this->setWindowState(Qt::WindowMinimized);
        }
    }
}

void MainWindow::copyEntryFieldValue()
{
    if (this->ui->entryTable->hasFocus())
    {
        // Make sure setting the clipboard here doesn't immediately stop our timer
        QObject::disconnect(QApplication::clipboard(), SIGNAL(dataChanged()), this->m_clearClipboardTimer, SLOT(stop()));

        QApplication::clipboard()->setText(this->ui->entryTable->selectedFieldText());
        if (SilverlockPreferences::instance().autoClearClipboardEnabled())
        {
            // We connect this slot so that if the user copies something ELSE
            // while the timer is running, do NOT clear that new data
            QObject::connect(QApplication::clipboard(), SIGNAL(dataChanged()), this->m_clearClipboardTimer, SLOT(stop()));

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
    this->m_clearClipboardTimer->stop();
}

void MainWindow::openEntryUrl()
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

void MainWindow::openEntryUrlIE()
{
#ifdef Q_WS_WIN
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

void MainWindow::moveEntries()
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

void MainWindow::duplicateEntries()
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

void MainWindow::editEntries()
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

void MainWindow::deleteEntries()
{
    if (this->m_documentState.hasDocument())
    {
        Database *db = this->m_documentState.database();

        QList<QUuid> uuids = this->ui->entryTable->selectedUuids();
        QString message;
        if (uuids.count() > 1)
        {
            message = tr("Are you sure you want to delete the selected entries?");
        }
        else if (uuids.count() == 1)
        {
            Entry *entry = db->findEntry(uuids.first());
            message = tr("Are you sure you want to delete the entry \"<b>%1</b>\"?").arg(entry ? entry->title() : QString());
        }
        else
        {
            return;
        }

        int result = NativeDialogs::warning(this, tr("Delete Entries"), message, QString(), QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::Yes)
        {
            Group *parent = NULL;

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

void MainWindow::addGroupSubgroup()
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

void MainWindow::addGroupEntry()
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

void MainWindow::moveGroups()
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
                        NativeDialogs::critical(this, tr("Error"), tr("A group cannot be moved to itself or one of its subgroups."));
                    }
                    else
                    {
                        Group *targetGroup = db->findGroup(dialog.selectedUuid(), true);
                        if (targetGroup)
                        {
                            if (sourceGroup->parentNode() == targetGroup)
                            {
                                NativeDialogs::critical(this, tr("Error"), tr("The source and destination group are the same."));
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

void MainWindow::editGroups()
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

void MainWindow::deleteGroups()
{
    if (this->m_documentState.hasDocument())
    {
        Database *db = this->m_documentState.database();

        int result = NativeDialogs::warning(this, tr("Delete Groups"),
            tr("Are you sure you want to delete the selected groups?"),
            QString(),
            QMessageBox::Yes | QMessageBox::No);
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

void MainWindow::find()
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

void MainWindow::findInGroup()
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
        this->setUnifiedTitleAndToolBarOnMac(false);
        this->showFullScreen();
    }
    else
    {
        this->showNormal();
        this->setUnifiedTitleAndToolBarOnMac(true);
    }
}

void MainWindow::on_actionPreferences_triggered()
{
    qsApp->preferences();
}

void MainWindow::on_actionHelpContents_triggered()
{
    qsApp->showHelpContents();
}

void MainWindow::on_actionWebsite_triggered()
{
    qsApp->launchProductWebsite();
}

void MainWindow::on_actionDonate_triggered()
{
    qsApp->launchDonationsWebsite();
}

void MainWindow::on_actionCheckForUpdates_triggered()
{
    qsApp->checkForUpdates();
}

void MainWindow::on_actionAboutSilverlock_triggered()
{
    qsApp->about();
}
