#include "entryeditdialog.h"
#include "ui_entryeditdialog.h"

EntryEditDialog::EntryEditDialog(Entry *entry, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EntryEditDialog), m_entry(entry)
{
    this->ui->setupUi(this);
}

EntryEditDialog::~EntryEditDialog()
{
    delete this->ui;
}

void EntryEditDialog::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
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
        this->m_entry->setUrl(QUrl(this->ui->urlLineEdit->text()));
        this->m_entry->setUsername(this->ui->usernameLineEdit->text());
        this->m_entry->setPassword(this->ui->passwordLineEdit->text());
        this->m_entry->setEmailAddress(this->ui->emailLineEdit->text());
        this->m_entry->setNotes(this->ui->notesTextEdit->toPlainText());

        // Recovery questions & answers
        this->m_entry->recoveryInfo().clear();
        for (int i = 0; i < this->ui->tableWidget->rowCount(); i++)
        {
            QString question = this->ui->tableWidget->itemAt(0, i)->text();
            QString answer = this->ui->tableWidget->itemAt(1, i)->text();
            this->m_entry->recoveryInfo().insert(question, answer);
        }
    }
}

/*!
    Closes the dialog and saves any changes if there are no input errors,
    or displays an error message if there are input errors.
 */
void EntryEditDialog::on_buttonBox_accepted()
{
    QString errorString = this->inputErrorString();
    if (errorString.isEmpty())
    {
        this->write();
        this->accept();
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), tr("The following input errors have occurred:") + errorString);
    }
}

/*!
    Closes the dialog and does not save any changes.
 */
void EntryEditDialog::on_buttonBox_rejected()
{
    this->reject();
}

/*!
    Gets an HTML-formatted string containing the list of input errors, or an empty string if there are no errors.
 */
QString EntryEditDialog::inputErrorString() const
{
    QString errorString;

    // We only want to give an error if the URL is not valid AND
    // not empty (because the user can leave the field blank)
    QUrl testUrl(this->ui->urlLineEdit->text());
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
