#include "entryeditdialog.h"
#include "ui_entryeditdialog.h"
#include <silverlocklib.h>

/*!
    \class EntryEditDialog

    The EntryEditDialog class represents a dialog that
    allows the user to edit the properties of a Entry.
 */

/*!
    Constructs a new EntryEditDialog.

    \param entry The entry to begin editing.
    \param parent The parent widget of the dialog.
 */
EntryEditDialog::EntryEditDialog(Entry *entry, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EntryEditDialog), m_entry(entry)
{
    this->ui->setupUi(this);
    this->read();

    // Connect our show/hide password button and hide the password by default
    QObject::connect(this->ui->revealToolButton, SIGNAL(toggled(bool)), this, SLOT(hidePassword(bool)));
    this->ui->revealToolButton->setChecked(true);
}

/*!
    Destroys the EntryEditDialog.
 */
EntryEditDialog::~EntryEditDialog()
{
    delete this->ui;
}

/*!
    \internal
 */
void EntryEditDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
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
    Gets the entry being edited by this dialog.
 */
Entry* EntryEditDialog::entry() const
{
    return this->m_entry;
}

/*!
    Sets the entry to be edited by this dialog.
 */
void EntryEditDialog::setEntry(Entry *entry)
{
    this->m_entry = entry;
    this->read();
}

/*!
    Populates the dialog fields with data from the entry instance.
 */
void EntryEditDialog::read()
{
    if (this->m_entry)
    {
        // Basic information
        this->ui->titleLineEdit->setText(this->m_entry->title());
        this->ui->urlLineEdit->setText(this->m_entry->url().toString());
        this->ui->usernameLineEdit->setText(this->m_entry->username());
        this->ui->passwordLineEdit->setText(this->m_entry->password());
        this->ui->emailLineEdit->setText(this->m_entry->emailAddress());
        this->ui->notesTextEdit->setPlainText(this->m_entry->notes());

        // Recovery questions & answers
        // Clear any previous data and set the row count equal to the number of Q&A's we have
        this->ui->tableWidget->clear();
        this->ui->tableWidget->setRowCount(this->m_entry->recoveryInfo().count());

        // Go through each Q&A pair with a hash iterator...
        QHashIterator<QString, QString> i(this->m_entry->recoveryInfo());
        int index = 0;
        while (i.hasNext())
        {
            i.next();

            // Create table widget items for the question and answer and set them on the table
            this->ui->tableWidget->setItem(index, 0, new QTableWidgetItem(i.key()));
            this->ui->tableWidget->setItem(index, 1, new QTableWidgetItem(i.value()));

            index++;
        }

        // UUID
        this->ui->uuidLineEdit->setText(this->m_entry->uuid().toString());
    }
}

/*!
    Updates the entry with data from the dialog fields.
 */
void EntryEditDialog::write() const
{
    if (this->m_entry)
    {
        // Basic information
        this->m_entry->setTitle(this->ui->titleLineEdit->text());
        this->m_entry->setUrl(QUrl(this->ui->urlLineEdit->text(), QUrl::StrictMode));
        this->m_entry->setUsername(this->ui->usernameLineEdit->text());
        this->m_entry->setPassword(this->ui->passwordLineEdit->text());
        this->m_entry->setEmailAddress(this->ui->emailLineEdit->text());
        this->m_entry->setNotes(this->ui->notesTextEdit->toPlainText());

        // Recovery questions & answers
        if (this->checkRecoveryModified())
        {
            this->m_entry->clearRecoveryInfo();
            for (int i = 0; i < this->ui->tableWidget->rowCount(); i++)
            {
                QString question = this->ui->tableWidget->item(i, 0)->text();
                QString answer = this->ui->tableWidget->item(i, 1)->text();
                this->m_entry->insertRecoveryInfo(question, answer);
            }
        }
    }
}

/*!
    Closes the dialog and saves any changes if there are no input errors,
    or displays an error message if there are input errors.
 */
void EntryEditDialog::accept()
{
    QString errorString = this->inputErrorString();
    if (errorString.isEmpty())
    {
        this->write();
        QDialog::accept();
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), tr("The following input errors have occurred:") + errorString);
    }
}

/*!
    Gets an HTML-formatted string containing the list of input errors, or an empty string if there are no errors.
 */
QString EntryEditDialog::inputErrorString() const
{
    QString errorString;

    // We only want to give an error if the URL is not valid AND
    // not empty (because the user can leave the field blank)
    QUrl testUrl(this->ui->urlLineEdit->text(), QUrl::StrictMode);
    if (!testUrl.isValid() && !testUrl.isEmpty())
    {
        errorString += "<li>" + tr("The URL you entered is improperly formatted: ") + testUrl.errorString() + "</li>";
    }

    if (!errorString.isEmpty())
    {
        return "<ul>" + errorString + "</ul>";
    }

    return errorString;
}

/*!
    Sets a value indicating whether the password should be hidden using asterisks.

    \param checked \c true to hide the password, \c false to display it verbatim.
 */
void EntryEditDialog::hidePassword(bool checked)
{
    this->ui->passwordLineEdit->setEchoMode(checked ? QLineEdit::Password : QLineEdit::Normal);
}

/*!
    Helper method to check whether the user actually made any changes to the recovery
    questions/answers.

    Because of the way recovery questions/answers are stored in an Entry, this lets us
    easily just clear the array in the Entry, and re-add everything from the table view,
    without emitting a modified signal when nothing has actually been modified.
 */
bool EntryEditDialog::checkRecoveryModified() const
{
    // Make sure to sort the table so it's in the same order as the entry for comparing
    this->ui->tableWidget->sortItems(0, Qt::AscendingOrder);

    // If the number of items differ we know they're different right away
    if (this->ui->tableWidget->rowCount() != this->m_entry->recoveryInfo().count())
    {
        return true;
    }

    // Now let's loop through all the items and check if the question or answer
    // for each pair differs from its counterpart in the table... we can do this
    // linearly since we sorted the table earlier
    QHashIterator<QString, QString> i(this->m_entry->recoveryInfo());
    int index = 0;
    while (i.hasNext())
    {
        i.next();

        // Get the question and answer from the table...
        QString question = this->ui->tableWidget->item(index, 0)->text();
        QString answer = this->ui->tableWidget->item(index, 1)->text();

        // ...and if either of them don't match the entry itself, there was a modification
        if (i.key() != question || i.value() != answer)
        {
            return true;
        }

        index++;
    }

    return false;
}
