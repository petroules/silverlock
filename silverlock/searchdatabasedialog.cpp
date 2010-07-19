#include "searchdatabasedialog.h"
#include "ui_searchdatabasedialog.h"

SearchDatabaseDialog::SearchDatabaseDialog(const Database *const database, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchDatabaseDialog), m_database(database), m_searchParameters(new SearchParameters())
{
    this->ui->setupUi(this);
    this->updateWidgets();

    QObject::connect(this->ui->searchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateSearchParameters()));
    QObject::connect(this->ui->titleCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateSearchParameters()));
    QObject::connect(this->ui->urlCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateSearchParameters()));
    QObject::connect(this->ui->usernameCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateSearchParameters()));
    QObject::connect(this->ui->passwordCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateSearchParameters()));
    QObject::connect(this->ui->emailAddressCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateSearchParameters()));
    QObject::connect(this->ui->notesCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateSearchParameters()));
    QObject::connect(this->ui->recoveryInfoCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateSearchParameters()));
    QObject::connect(this->ui->groupTitleCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateSearchParameters()));
    QObject::connect(this->ui->uuidCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateSearchParameters()));
    QObject::connect(this->ui->regexCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateSearchParameters()));
    QObject::connect(this->ui->caseSensitiveCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateSearchParameters()));
}

SearchDatabaseDialog::~SearchDatabaseDialog()
{
    if (this->m_searchParameters)
    {
        delete this->m_searchParameters;
    }

    delete this->ui;
}

const Database *const SearchDatabaseDialog::database() const
{
    return this->m_database;
}

void SearchDatabaseDialog::setDatabase(const Database *const database)
{
    this->m_database = database;
}

SearchParameters* SearchDatabaseDialog::searchParameters() const
{
    return this->m_searchParameters;
}

void SearchDatabaseDialog::setSearchParameters(SearchParameters *searchParameters)
{
    // Cannot nullify the search parameters
    if (!searchParameters)
    {
        return;
    }

    if (this->m_searchParameters)
    {
        delete this->m_searchParameters;
    }

    this->m_searchParameters = searchParameters;
}

const QList<Entry*>& SearchDatabaseDialog::entriesFound() const
{
    return this->m_entries;
}

void SearchDatabaseDialog::accept()
{
    QString errorString = this->inputErrorString();
    if (errorString.isEmpty())
    {
        if (this->m_database)
        {
            this->m_entries = this->m_database->findEntries(*this->m_searchParameters);
        }

        QDialog::accept();
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), tr("The following input errors have occurred:") + errorString);
    }
}

void SearchDatabaseDialog::updateWidgets()
{
    this->ui->searchLineEdit->setText(this->m_searchParameters->searchPattern);
    this->ui->titleCheckBox->setChecked(this->m_searchParameters->searchTitle);
    this->ui->urlCheckBox->setChecked(this->m_searchParameters->searchUrl);
    this->ui->usernameCheckBox->setChecked(this->m_searchParameters->searchUsername);
    this->ui->passwordCheckBox->setChecked(this->m_searchParameters->searchPassword);
    this->ui->emailAddressCheckBox->setChecked(this->m_searchParameters->searchEmailAddress);
    this->ui->notesCheckBox->setChecked(this->m_searchParameters->searchNotes);
    this->ui->recoveryInfoCheckBox->setChecked(this->m_searchParameters->searchRecovery);
    this->ui->groupTitleCheckBox->setChecked(this->m_searchParameters->searchGroupTitle);
    this->ui->uuidCheckBox->setChecked(this->m_searchParameters->searchUuid);
    this->ui->regexCheckBox->setChecked(this->m_searchParameters->useRegex);
    this->ui->caseSensitiveCheckBox->setChecked(this->m_searchParameters->caseSensitive);
}

void SearchDatabaseDialog::updateSearchParameters()
{
    this->m_searchParameters->searchPattern = this->ui->searchLineEdit->text();
    this->m_searchParameters->searchTitle = this->ui->titleCheckBox->isChecked();
    this->m_searchParameters->searchUrl = this->ui->urlCheckBox->isChecked();
    this->m_searchParameters->searchUsername = this->ui->usernameCheckBox->isChecked();
    this->m_searchParameters->searchPassword = this->ui->passwordCheckBox->isChecked();
    this->m_searchParameters->searchEmailAddress = this->ui->emailAddressCheckBox->isChecked();
    this->m_searchParameters->searchNotes = this->ui->notesCheckBox->isChecked();
    this->m_searchParameters->searchRecovery = this->ui->recoveryInfoCheckBox->isChecked();
    this->m_searchParameters->searchGroupTitle = this->ui->groupTitleCheckBox->isChecked();
    this->m_searchParameters->searchUuid = this->ui->uuidCheckBox->isChecked();
    this->m_searchParameters->useRegex = this->ui->regexCheckBox->isChecked();
    this->m_searchParameters->caseSensitive = this->ui->caseSensitiveCheckBox->isChecked();
}

QString SearchDatabaseDialog::inputErrorString() const
{
    QString errorString;

    if (!this->m_searchParameters->fieldsSelected())
    {
        errorString += "<li>" + tr("You have not selected any fields to search in. Please select at least one.") + "</li>";
    }

    if (this->m_searchParameters->searchPattern.isEmpty())
    {
        errorString += "<li>" + tr("You have not entered a search pattern.") + "</li>";
    }

    if (!errorString.isEmpty())
    {
        return "<ul>" + errorString + "</ul>";
    }

    return errorString;
}
