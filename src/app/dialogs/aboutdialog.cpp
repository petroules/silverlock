#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "licensedialog.h"

/*!
    \class AboutDialog

    The AboutDialog class provides a dialog that displays general application information.
 */

/*!
    Constructs a new AboutDialog.

    \param parent The parent widget of the dialog.
 */
AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    this->ui->setupUi(this);

    // Set window title to "About ApplicationName"
    this->setWindowTitle(this->windowTitle().arg(qApp->applicationName()));

    // Set the application and organization names on the labels
    this->ui->labelApplication->setText(this->ui->labelApplication->text()
        .arg(qApp->applicationName())
        .arg(qApp->applicationVersion()));
}

/*!
    Destroys the dialog.
 */
AboutDialog::~AboutDialog()
{
    delete this->ui;
}

/*!
    Displays the license dialog.
 */
void AboutDialog::displayLicenseDialog()
{
    LicenseDialog dialog(this);
    dialog.exec();
}
