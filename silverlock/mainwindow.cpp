#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <silverlocklib.h>
#include "aboutdialog.h"
#include "version.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Set information about the application
    QCoreApplication::setOrganizationName(VER_COMPANYNAME_STR);
    QCoreApplication::setOrganizationDomain(VER_COMPANYDOMAIN_STR);
    QCoreApplication::setApplicationName(VER_PRODUCTNAME_STR);
    QCoreApplication::setApplicationVersion(VER_PRODUCTVERSION_STR);

    // Setup the user interface
    this->ui->setupUi(this);

    // We're connecting these manually because they need to return bool
    // and be used in multiple places; we don't want to use long names
    QObject::connect(this->ui->action_Save, SIGNAL(triggered()), this, SLOT(save()));
    QObject::connect(this->ui->actionSave_As, SIGNAL(triggered()), this, SLOT(saveAs()));

    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setUnifiedTitleAndToolBarOnMac(true);
    //???
    //TODO:documentWasModifiedAction > this->setWindowModified(true);
    this->statusBar()->showMessage(tr("Ready"));

#ifdef Q_WS_X11
    // Actions
    this->ui->actionE_xit->setIcon(QIcon::fromTheme("application-exit"));
    this->ui->actionOptions->setIcon(QIcon::fromTheme("preferences-other"));
    this->ui->actionAbout->setIcon(QIcon::fromTheme("help-about"));
#endif

    // Keyboard Shortcuts
    this->ui->action_New->setShortcut(QKeySequence::New);
    this->ui->action_Open->setShortcut(QKeySequence::Open);
    this->ui->actionClose->setShortcut(QKeySequence::Close);
    this->ui->action_Save->setShortcut(QKeySequence::Save);
    this->ui->actionSave_As->setShortcut(QKeySequence::SaveAs);
    this->ui->action_Print->setShortcut(QKeySequence::Print);
    this->ui->actionE_xit->setShortcut(QKeySequence::Quit);
    this->ui->actionOptions->setShortcut(QKeySequence::Preferences);

    test();

    this->setCurrentFile("");
}

void MainWindow::test()
{
    QList<Entry*> a;
    a.append(new Entry("Amazon.com"));
    a.append(new Entry("eBay"));
    a.append(new Entry("Stuffland.com"));
    a.append(new Entry("StuffEmporium.net"));
    a.append(new Entry("toyota secret account"));
    a.append(new Entry("race car"));
    a.append(new Entry("thingthing"));

    for (int i = 0; i < a.count(); i++)
    {
        Entry *x = a.at(i);
        x->setEmailAddress("jake.petroules@gmail.com");
        x->setNotes("This is a larger series of test notes.\nIt is designed to test multiline wrapping stuff of the etc kind.");
        x->setPassword("s3cr3tp4$$");
        x->setUsername("jaekdaSnaek");
        x->setUrl(QUrl("http://www.google.com/"));
    }

    Group *shopping = new Group("Shopping");
    Group *stuff = new Group("Stuff");
    Group *things = new Group("Things");
    stuff->groups().append(things);
    Group *other = new Group("Other");

    shopping->entries().append(a[0]);
    shopping->entries().append(a[1]);

    stuff->entries().append(a[2]);
    stuff->entries().append(a[3]);

    other->entries().append(a[4]);
    other->entries().append(a[5]);

    things->entries().append(a[6]);

    this->db = new Database("Bob's DB", "secr3t");
    this->db->rootGroup()->groups().append(shopping);
    this->db->rootGroup()->groups().append(stuff);
    this->db->rootGroup()->groups().append(other);
    this->db->rootGroup()->groups().append(new Group("Test Cat"));

    this->populateTreeView(this->db);

    QFile file("test.xml");
    if (file.open(QIODevice::WriteOnly))
    {
        this->db->write(file);
    }
}

MainWindow::~MainWindow()
{
    delete this->ui;
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
    if (this->maybeSave())
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::on_action_New_triggered()
{
    MainWindow *other = new MainWindow;
    other->move(this->x() + 40, this->y() + 40);
    other->show();
}

void MainWindow::on_action_Open_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
    {
        MainWindow *existing = findMainWindow(fileName);
        if (existing)
        {
            existing->show();
            existing->raise();
            existing->activateWindow();
            return;
        }

        if (this->isUntitled && !this->isWindowModified())
        {
            this->loadFile(fileName);
        }
        else
        {
            MainWindow *other = new MainWindow();
            other->loadFile(fileName);
            if (other->isUntitled)
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
    if (this->isUntitled)
    {
        return this->saveAs();
    }
    else
    {
        return this->saveFile(this->currentFile);
    }
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), this->currentFile);
    if (fileName.isEmpty())
    {
        return false;
    }

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

}

void MainWindow::on_actionAbout_triggered()
{
    QString str = tr("<p>Silverlock is a secure password management program designed to ease the management of account credentials and related information.</p>");
    str.append(tr("<p>The padlock icon used in this program is a public domain image. Details can be found <a href=\"http://www.openclipart.org/detail/33505\">here</a>.</p>"));
    AboutDialog::show(this, this->windowTitle(), str);
}

/*!
    Performs handling for when an item in the left-hand tree browser is selected.
 */
void MainWindow::on_treeBrowser_itemSelectionChanged()
{
    // Get the selected group UUID
    QUuid uuid = this->selectedGroupUuid();
    if (!uuid.isNull())
    {
        // Find the group for this UUID and populate the detail view with it
        Group *group = this->db->findGroup(uuid);
        if (group != NULL)
        {
            this->populateDetailView(group);
            qDebug() << "Groups: " << group->countGroups() << "Entries: " << group->countEntries();
        }
        else
        {
            this->ui->detailView->clear();
        }
    }
}

/*!
    Shows the context menu for the selected group when the tree browser is right-clicked.
 */
void MainWindow::on_treeBrowser_customContextMenuRequested(QPoint pos)
{
    this->ui->menuGroup->exec(QCursor::pos());
}

/*!
    Populates the left-hand tree view with the specified account database.
 */
void MainWindow::populateTreeView(Database *const db)
{
    // Remove any previous items from the tree browser
    this->ui->treeBrowser->clear();

    // Get the root item of the tree browser and call the recursive
    // populate function with the account database's root category
    QTreeWidgetItem *parentItem = this->ui->treeBrowser->invisibleRootItem();
    this->populateTreeView(parentItem, db->rootGroup());

    // Then expand all the nodes
    qWarning("Make this a settings option");
    this->ui->treeBrowser->expandAll();
}

/*!
    Populates the left-hand tree view item with the specified category and its children. Do not call this method directly.
 */
void MainWindow::populateTreeView(QTreeWidgetItem *parentItem, Group *const cat)
{
    // Create a widget item for the category, set the text, icon, etc., and add it to the parent item
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, cat->name());
    item->setIcon(0, QIcon(":/main/res/category.png"));
    item->setData(0, Qt::UserRole, cat->uuid().toString());
    parentItem->addChild(item);

    // Then add all the category's subcategories as children of itself
    for (int i = 0; i < cat->groups().count(); i++)
    {
        this->populateTreeView(item, cat->groups().at(i));
    }

#ifdef SL_ADD_CHILDREN
    // Then we add this category's accounts as children of itself
    for (int i = 0; i < cat->accounts().count(); i++)
    {
        Account *acc = cat->accounts().at(i);

        // Create a widget item for the account, set the text, icon, etc.,
        // and add it to the item we made for the category passed to this method
        QTreeWidgetItem *account = new QTreeWidgetItem();
        account->setText(0, acc->title());
        account->setIcon(0, QIcon(":/main/res/account.png"));
        account->setData(0, Qt::UserRole, acc->uuid().toString());
        item->addChild(account);
    }
#endif
}

/*!
    Populates the detail with the the specified category and its child categories and accounts.
 */
void MainWindow::populateDetailView(Group *const cat)
{
    // Clears the detail view of any previous data.
    this->ui->detailView->clear();

#ifdef SL_ADD_SUBCAT
    // Add all the subcategories of the category
    for (int i = 0; i < cat->categories().count(); i++)
    {
        Category *category = cat->categories().at(i);
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setIcon(0, QIcon(":/main/res/category.png"));
        item->setText(0, category->name());
        item->setText(6, category->uuid());
        this->ui->detailView->invisibleRootItem()->addChild(item);
    }
#endif

    // Add all the accounts in the category
    for (int i = 0; i < cat->entries().count(); i++)
    {
        Entry *account = cat->entries().at(i);
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setIcon(0, QIcon(":/main/res/account.png"));
        item->setText(0, account->title());
        item->setText(1, account->url().toString());
        item->setText(2, account->username());
        item->setText(3, account->password());
        item->setText(4, account->emailAddress());
        item->setText(5, account->notes());
        item->setText(6, account->uuid().toString());
        this->ui->detailView->invisibleRootItem()->addChild(item);
    }

    // Tell the columns to auto-adjust to their contents
    int cols = this->ui->detailView->header()->count();
    for (int i = 0; i < cols; i++)
    {
        this->ui->detailView->header()->setResizeMode(i, QHeaderView::ResizeToContents);
    }
}

/*!
    Gets the UUID of the group selected in the left-hand tree browser,
    or an empty UUID if none (or the root group) is selected.
 */
QUuid MainWindow::selectedGroupUuid() const
{
    // Get the list of selected items and make sure there's only 1 selected
    QList<QTreeWidgetItem*> selected = this->ui->treeBrowser->selectedItems();
    if (selected.count() == 1)
    {
        // Get the first item if it exists...
        QTreeWidgetItem *item = selected.first();
        if (item)
        {
            // And find and return its UUID
            return QUuid(item->data(0, Qt::UserRole).toString());
        }
    }

    return QUuid();
}

void MainWindow::documentWasModified()
{
    this->setWindowModified(true);
}

bool MainWindow::maybeSave()
{
    if (this->isWindowModified())
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Silverlock"),
                     tr("The database has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard
                     | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
        {
            return save();
        }
        else if (ret == QMessageBox::Cancel)
        {
            return false;
        }
    }

    return true;
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        file.close();
        QMessageBox::warning(this, tr("Silverlock"), tr("Cannot read file %1:\n%2.").arg(fileName).arg(file.errorString()));
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    bool ok;
    QString str = QInputDialog::getText(this, tr("Enter password"), tr("Password:"), QLineEdit::Password, "", &ok);
    if (ok)
    {
        this->db = Database::read(file, str);
    }

    file.close();

    QApplication::restoreOverrideCursor();

    this->setCurrentFile(fileName);
    this->statusBar()->showMessage(tr("File loaded"), 2000);
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, tr("Silverlock"), tr("Cannot write file %1:\n%2").arg(fileName).arg(file.errorString()));
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    // Try to write the database to the IO device
    bool status = false;
    if (this->db->write(file))
    {
        // If it succeeded, we set the filename to the name of the file we saved and show a status message
        this->setCurrentFile(fileName);
        this->statusBar()->showMessage(tr("File saved"), 2000);
        status = true;
    }
    else
    {
        // Otherwise show a failure message
        QMessageBox::warning(this, tr("Silverlock"), tr("Cannot write file %1").arg(fileName));
    }

    QApplication::restoreOverrideCursor();

    return status;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    static int sequenceNumber = 1;

    this->isUntitled = fileName.isEmpty();
    if (this->isUntitled)
    {
        this->currentFile = tr("database%1.adb").arg(sequenceNumber++);
    }
    else
    {
        this->currentFile = QFileInfo(fileName).canonicalFilePath();
    }

    this->setWindowModified(false);
    this->setWindowFilePath(this->currentFile);
}

/*!
    Returns the name of the file, excluding the path.
 */
QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

/*!
    Finds the window which has the file with the specified file name open.
    Returns NULL if the file is not opened by any window.
 */
MainWindow* MainWindow::findMainWindow(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QWidget *widget, qApp->topLevelWidgets())
    {
        MainWindow *mainWin = qobject_cast<MainWindow*>(widget);
        if (mainWin && mainWin->currentFile == canonicalFilePath)
        {
            return mainWin;
        }
    }

    return NULL;
}

void MainWindow::on_actionMove_Entries_triggered()
{

}
