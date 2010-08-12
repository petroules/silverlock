#include "licensedialog.h"
#include "ui_licensedialog.h"

/*!
    \class LicenseDialog

    The LicenseDialog class provides a dialog that displays licenses for the various software
    components that make up the application.
 */

/*!
    Constructs a new LicenseDialog.

    \param parent The parent widget of the dialog.
 */
LicenseDialog::LicenseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LicenseDialog)
{
    this->ui->setupUi(this);

    this->ui->silverlockLicense->setText(getLicense("gpl-3.0"));
    this->ui->lielLicense->setText(getLicense("lgpl-2.1"));
    this->ui->qtLicense->setText(getLicense("lgpl-2.1"));
    this->ui->qsaLicense->setText(getLicense("lgpl-2.1"));
    this->ui->botanLicense->setText(getLicense("botan"));
    this->ui->sparkleLicense->setText(getLicense("sparkle"));

    this->ui->qtAdditionalLabel->setText(getLicense("qt-exception"));
    this->ui->qsaAdditionalLabel->setText(getLicense("qsa-exception"));
}

/*!
    Destroys the LicenseDialog.
 */
LicenseDialog::~LicenseDialog()
{
    delete this->ui;
}

/*!
    Gets the text of the license file specified by the license name \a license.
 */
QString LicenseDialog::getLicense(const QString &license)
{
    QFile file(QString(":/main/license/%1.txt").arg(license));
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);
        QString text = in.readAll();
        file.close();
        return text;
    }

    return QString();
}
