#include "guardeddialog.h"

/*!
    \class GuardedDialog

    The GuardedDialog class provides a base for other dialogs which load and save data and may
    need to display errors or warnings which must be handled by the user before the dialog is
    accepted.
 */

/*!
    Constructs a new GuardedDialog.

    \param parent The parent widget of the dialog.
 */
GuardedDialog::GuardedDialog(QWidget *parent) :
    QDialog(parent)
{
}

/*!
    Destroys the dialog.
 */
GuardedDialog::~GuardedDialog()
{
}

/*!
    Reimplemented from QDialog::accept() to display error messages, warnings and information
    before saving information to external objects.
 */
void GuardedDialog::accept()
{
    // Initialize a few string lists to hold messages given by subclasses
    QStringList errors;
    QStringList warnings;
    QStringList information;
    this->getMessages(errors, warnings, information);

    if (errors.count() > 0)
    {
        QMessageBox::critical(this, tr("Error"), errors.join("\n\n"));
        return;
    }

    if (warnings.count() > 0)
    {
        // Ensures the user is appropriately questioned
        warnings.append(tr("Are you sure you wish to continue anyways?"));

        if (QMessageBox::warning(this, tr("Warning"), warnings.join("\n\n"),
            QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
        {
            return;
        }
    }

    if (information.count() > 0)
    {
        QMessageBox::information(this, tr("Information"), information.join("\n\n"));
    }

    this->save();
    QDialog::accept();
}
