#include "licensedialog.h"
#include "ui_licensedialog.h"

LicenseDialog::LicenseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LicenseDialog)
{
    this->ui->setupUi(this);

    this->ui->silverlockLicense->setText(getLicense("gpl-3.0"));
    this->ui->qtLicense->setText(getLicense("lgpl-2.1"));
    this->ui->qsaLicense->setText(getLicense("lgpl-2.1"));
    this->ui->botanLicense->setText(getLicense("botan"));

    this->ui->qtAdditionalLabel->setText(getLicense("qt-exception"));
    this->ui->qsaAdditionalLabel->setText(getLicense("qsa-exception"));
}

LicenseDialog::~LicenseDialog()
{
    delete this->ui;
}

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
