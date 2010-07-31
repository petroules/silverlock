#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "licensedialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    this->ui->setupUi(this);

    this->setWindowTitle(this->windowTitle().arg(QApplication::applicationName()));
    this->ui->labelApplication->setText(this->ui->labelApplication->text()
        .arg(QApplication::applicationName())
        .arg(QApplication::applicationVersion()));
    this->ui->labelInfo->setText(this->ui->labelInfo->text()
        .arg(QApplication::organizationName())
        .arg(QApplication::organizationDomain())
        .arg(QApplication::applicationName()));
}

AboutDialog::~AboutDialog()
{
    delete this->ui;
}

void AboutDialog::on_licensePushButton_clicked()
{
    LicenseDialog dialog(this);
    dialog.exec();
}
