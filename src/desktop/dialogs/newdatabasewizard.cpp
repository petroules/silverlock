#include "newdatabasewizard.h"
#include "ui_newdatabasewizard.h"
#include <silverlocklib.h>

NewDatabaseWizard::NewDatabaseWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::NewDatabaseWizard)
{
    this->ui->setupUi(this);
    this->passwordChanged(QString());
}

NewDatabaseWizard::~NewDatabaseWizard()
{
    delete this->ui;
}

Database* NewDatabaseWizard::database() const
{
    return this->m_database;
}

void NewDatabaseWizard::passwordChanged(QString password)
{
    this->ui->passwordProgressBar->setValue(this->passwordComplexityRating(password));

    // If the password box is empty...
    this->ui->warningLabel->setVisible(this->ui->passwordLineEdit->text().isEmpty());
}

int NewDatabaseWizard::passwordComplexityRating(const QString &password) const
{
    int rating = 0;

    if (password.contains(QRegExp("[a-z]")))
    {
        rating += 10;
    }

    if (password.contains(QRegExp("[A-Z]")))
    {
        rating += 10;
    }

    if (password.contains(QRegExp("[0-9]")))
    {
        rating += 10;
    }

    rating += qMin(100, qMax(0, (password.length() - 5)) * 10);
    return rating;
}

void NewDatabaseWizard::databaseCreated()
{
    this->m_database = new Database(this->ui->titleLineEdit->text(), this->ui->passwordLineEdit->text());
}

void NewDatabaseWizard::passwordVisibilityToggled(bool checked)
{
    this->ui->passwordLineEdit->setEchoMode(checked ? QLineEdit::Password : QLineEdit::Normal);
}
