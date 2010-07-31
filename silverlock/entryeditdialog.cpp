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
    GuardedDialog(parent),
    ui(new Ui::EntryEditDialog), m_entry(entry)
{
    this->ui->setupUi(this);
    this->load();

    // Hide the password by default
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
    this->load();
}

/*!
    Populates the dialog fields with data from the entry instance.
 */
void EntryEditDialog::load()
{
    if (this->m_entry)
    {
        // Basic information
        this->ui->titleLineEdit->setText(this->m_entry->title());
        this->ui->urlLineEdit->setText(this->m_entry->url().toString());
        this->ui->usernameLineEdit->setText(this->m_entry->username());
        this->ui->passwordLineEdit->setText(this->m_entry->password());
        this->ui->notesTextEdit->setPlainText(this->m_entry->notes());
        this->readRecoveryInfo();
        this->readCustomFields();

        // Read-only info
        this->ui->uuidLineEdit->setText(this->m_entry->uuid().toString());
        this->ui->createdLineEdit->setText(this->m_entry->created().toLocalTime().toString(Qt::SystemLocaleLongDate));
        this->ui->accessedLineEdit->setText(this->m_entry->accessed().toLocalTime().toString(Qt::SystemLocaleLongDate));
        this->ui->modifiedLineEdit->setText(this->m_entry->modifiedTime().toLocalTime().toString(Qt::SystemLocaleLongDate));
    }
}

/*!
    Updates the entry with data from the dialog fields.
 */
void EntryEditDialog::save()
{
    if (this->m_entry)
    {
        // Basic information
        this->m_entry->setTitle(this->ui->titleLineEdit->text());
        this->m_entry->setUrl(QUrl(this->ui->urlLineEdit->text(), QUrl::StrictMode));
        this->m_entry->setUsername(this->ui->usernameLineEdit->text());
        this->m_entry->setPassword(this->ui->passwordLineEdit->text());
        this->m_entry->setNotes(this->ui->notesTextEdit->toPlainText());
        this->writeRecoveryInfo();
        this->writeCustomFields();
    }
}

void EntryEditDialog::getMessages(QStringList &errors, QStringList &warnings, QStringList &information) const
{
    Q_UNUSED(warnings);
    Q_UNUSED(information);

    // We only want to give an error if the URL is not valid AND
    // not empty (because the user can leave the field blank)
    QUrl testUrl(this->ui->urlLineEdit->text(), QUrl::StrictMode);
    if (!testUrl.isValid() && !testUrl.isEmpty())
    {
        errors.append(QString(tr("The URL you entered is improperly formatted: %1")).arg(testUrl.errorString()));
    }

    // Check for recovery questions with duplicate question text
    if (this->checkDuplicateRecovery())
    {
        errors.append(tr("There are some duplicate recovery questions. Please ensure that no recovery question shares the same question text."));
    }

    // Check for custom fields with duplicate key/name text
    if (this->checkDuplicateCustomFields())
    {
        errors.append(tr("There are some duplicate custom fields. Please ensure that no custom fields share the same name."));
    }
}

/*!
    Sets a value indicating whether the password should be hidden using asterisks.

    \param checked \c true to hide the password, \c false to display it verbatim.
 */
void EntryEditDialog::hidePassword(bool checked)
{
    this->ui->passwordLineEdit->setEchoMode(checked ? QLineEdit::Password : QLineEdit::Normal);
}
