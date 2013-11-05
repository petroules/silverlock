#include "databaseprintpreviewdialog.h"
#include "ui_databaseprintpreviewdialog.h"
#include <silverlocklib.h>

/*!
    \class DatabasePrintPreviewDialog

    The DatabasePrintPreviewDialog class provides a dialog allowing the user to configure print options
    for the database and previews the results.
 */

/*!
    Constructs a new DatabasePrintPreviewDialog using the information in \a database to generate a preview.

    \param database The database to preview.
    \param parent The parent widget of the dialog.
 */
DatabasePrintPreviewDialog::DatabasePrintPreviewDialog(Database *database, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabasePrintPreviewDialog), m_database(database)
{
    this->ui->setupUi(this);
    this->updatePreview();
}

/*!
    Destroys the dialog.
 */
DatabasePrintPreviewDialog::~DatabasePrintPreviewDialog()
{
    delete this->ui;
}

/*!
    Gets the selection of fields that the user elected to include in the printout.
 */
DatabasePrinterFields DatabasePrintPreviewDialog::databasePrinterFields() const
{
    DatabasePrinterFields fields;
    fields.title = this->ui->titleCheckBox->isChecked();
    fields.username = this->ui->usernameCheckBox->isChecked();
    fields.password = this->ui->passwordCheckBox->isChecked();
    fields.url = this->ui->urlCheckBox->isChecked();
    fields.notes = this->ui->notesCheckBox->isChecked();
    fields.recoveryInfo = this->ui->recoveryInfoCheckBox->isChecked();
    fields.customFields = this->ui->customFieldsCheckBox->isChecked();
    fields.uuid = this->ui->uuidCheckBox->isChecked();
    fields.creationTime = this->ui->creationTimeCheckBox->isChecked();
    fields.accessTime = this->ui->accessTimeCheckBox->isChecked();
    fields.modificationTime = this->ui->modificationTimeCheckBox->isChecked();
    return fields;
}

/*!
    Selects or deselects all field checkboxes depending on the anchor specified by \a link.

    \param link The anchor determining whether to select or deselect the checkboxes. #select-all
    will cause all checkboxes to be selected; #clear-all will cause all the checkbox selections
    to be cleared. Any other value will do nothing.
 */
void DatabasePrintPreviewDialog::selectionLinkActivated(const QString &link)
{
    if (link == "#select-all")
    {
        this->selectAll(true);
    }
    else if (link == "#clear-all")
    {
        this->selectAll(false);
    }
}

/*!
    Selects or deselects all checkboxes.

    \param select If this parameter is \c true, all checkboxes will be selected. If it is \c false,
    all checkbox selections will be cleared.
 */
void DatabasePrintPreviewDialog::selectAll(bool select)
{
    this->ui->titleCheckBox->setChecked(select);
    this->ui->usernameCheckBox->setChecked(select);
    this->ui->passwordCheckBox->setChecked(select);
    this->ui->urlCheckBox->setChecked(select);
    this->ui->notesCheckBox->setChecked(select);
    this->ui->recoveryInfoCheckBox->setChecked(select);
    this->ui->customFieldsCheckBox->setChecked(select);
    this->ui->uuidCheckBox->setChecked(select);
    this->ui->creationTimeCheckBox->setChecked(select);
    this->ui->accessTimeCheckBox->setChecked(select);
    this->ui->modificationTimeCheckBox->setChecked(select);
}

/*!
    Updates the database print preview.
 */
void DatabasePrintPreviewDialog::updatePreview()
{
    QString html;

    // If the user elected to preview their *actual* database, print it to HTML
    if (this->ui->showActualCheckBox->isChecked())
    {
        DatabasePrinter printer(this->m_database);
        html = printer.toHtml(this->databasePrinterFields());
    }
    else
    {
        // Otherwise we'll generate a mock preview for security reasons
        Database mockDatabase(tr("My Database"), "g00dPa$$w0rd%");

        Group mockGroup(tr("My Group"), &mockDatabase);

        Entry mockEntry("Google", &mockGroup);
        mockEntry.setUsername("john.doe");
        mockEntry.setPassword("!$uper$ecretPa$$w0rd~");
        mockEntry.setUrl(QUrl("http://www.google.com/"));
        mockEntry.setNotes(tr("This is my Google account!\n\nI created it on 2008-09-19 & have been using it since."));
        mockEntry.insertRecoveryInfo(tr("What is your mother's maiden name?"), "Doe");
        mockEntry.insertRecoveryInfo(tr("What is your favourite colour?"), "Blue");
        mockEntry.insertCustomField("PIN", "0123");

        // Print the mock database to HTML
        DatabasePrinter printer(&mockDatabase);
        html = printer.toHtml(this->databasePrinterFields());
    }

    // Send the database HTML to the preview pane
    if (!html.isEmpty())
    {
        this->ui->textBrowser->setHtml(html);
    }
    else
    {
        this->ui->textBrowser->setText(tr("There is no database present to preview."));
    }
}
