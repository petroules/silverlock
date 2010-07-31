#include "databaseauthenticationdialog.h"
#include "ui_databaseauthenticationdialog.h"

DatabaseAuthenticationDialog::DatabaseAuthenticationDialog(QWidget *parent) :
    GuardedDialog(parent),
    ui(new Ui::DatabaseAuthenticationDialog)
{
    this->ui->setupUi(this);

    // Hide the password by default
    this->ui->revealToolButton->setChecked(true);
}

DatabaseAuthenticationDialog::~DatabaseAuthenticationDialog()
{
    delete this->ui;
}

QString DatabaseAuthenticationDialog::password() const
{
    return this->ui->passwordLineEdit->text();
}

void DatabaseAuthenticationDialog::setPassword(const QString &password)
{
    this->ui->passwordLineEdit->setText(password);
}

void DatabaseAuthenticationDialog::getMessages(QStringList &errors, QStringList &warnings, QStringList &information) const
{
    Q_UNUSED(errors);
    Q_UNUSED(information);

    if (this->ui->passwordLineEdit->text().isEmpty())
    {
        warnings.append(tr("You have not entered a password."));
    }
}

/*!
    Sets a value indicating whether the password should be hidden using asterisks.

    \param checked \c true to hide the password, \c false to display it verbatim.
 */
void DatabaseAuthenticationDialog::hidePassword(bool checked)
{
    this->ui->passwordLineEdit->setEchoMode(checked ? QLineEdit::Password : QLineEdit::Normal);
}
