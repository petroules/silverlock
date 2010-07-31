#include "guardeddialog.h"

GuardedDialog::GuardedDialog(QWidget *parent) :
    QDialog(parent)
{
}

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
