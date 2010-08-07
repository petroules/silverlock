#include "searchdatabasedialog.h"
#include "ui_searchdatabasedialog.h"

/*!
    \class SearchDatabaseDialog

    The SearchDatabaseDialog class provides a dialog allowing the user to search a Database or
    Group for entries matching a specified criteria.
 */

/*!
    Constructs a new SearchDatabaseDialog.

    \param group The group or database to search.
    \param parent The parent widget of the dialog.
 */
SearchDatabaseDialog::SearchDatabaseDialog(const Group *const group, QWidget *parent) :
    GuardedDialog(parent),
    ui(new Ui::SearchDatabaseDialog), m_group(group), m_searchParameters(NULL)
{
    this->ui->setupUi(this);

    // We've got to do this after, or the setupUi method causes
    // the checkboxes to overwrite the SearchParameters properties
    this->setSearchParameters(new SearchParameters(this));
}

/*!
    Destroys the dialog.
 */
SearchDatabaseDialog::~SearchDatabaseDialog()
{
    delete this->ui;
}

/*!
    Gets a pointer to the group being searched.
 */
const Group* SearchDatabaseDialog::group() const
{
    return this->m_group;
}

/*!
    Sets the group to be searched.
 */
void SearchDatabaseDialog::setGroup(const Group *group)
{
    this->m_group = group;
}

/*!
    Retrieves a pointer to the current set of search parameters.
 */
SearchParameters* SearchDatabaseDialog::searchParameters() const
{
    return this->m_searchParameters;
}

/*!
    Sets the current set of search parameters to \a searchParameters. The SearchDatabaseDialog
    takes ownership of the SearchParameters.
 */
void SearchDatabaseDialog::setSearchParameters(SearchParameters *searchParameters)
{
    // Cannot nullify the search parameters
    if (!searchParameters)
    {
        return;
    }

    this->m_searchParameters = searchParameters;
    this->m_searchParameters->setParent(this);
    this->updateWidgets();
}

/*!
    Returns the list of entries found by the search.
 */
const QList<Entry*>& SearchDatabaseDialog::entriesFound() const
{
    return this->m_entries;
}

/*!
    Performs the search and populates the list of entries found.
 */
void SearchDatabaseDialog::save()
{
    if (this->m_group)
    {
        this->m_entries = this->m_group->findEntries(*this->m_searchParameters);
    }
}

void SearchDatabaseDialog::getMessages(QStringList &errors, QStringList &warnings, QStringList &information) const
{
    Q_UNUSED(warnings);
    Q_UNUSED(information);

    if (!this->m_searchParameters->fieldsSelected())
    {
        errors.append(tr("You have not selected any fields to search in. Please select at least one."));
    }

    if (this->m_searchParameters->searchPattern.isEmpty())
    {
        errors.append(tr("You have not entered a search pattern."));
    }
}

/*!
    Updates the GUI widgets per the state of the search parameters.
 */
void SearchDatabaseDialog::updateWidgets()
{
    this->ui->searchLineEdit->setText(this->m_searchParameters->searchPattern);
    this->ui->titleCheckBox->setChecked(this->m_searchParameters->searchTitle);
    this->ui->urlCheckBox->setChecked(this->m_searchParameters->searchUrl);
    this->ui->usernameCheckBox->setChecked(this->m_searchParameters->searchUsername);
    this->ui->passwordCheckBox->setChecked(this->m_searchParameters->searchPassword);
    this->ui->notesCheckBox->setChecked(this->m_searchParameters->searchNotes);
    this->ui->recoveryInfoCheckBox->setChecked(this->m_searchParameters->searchRecovery);
    this->ui->customFieldsCheckBox->setChecked(this->m_searchParameters->searchCustomFields);
    this->ui->groupTitleCheckBox->setChecked(this->m_searchParameters->searchGroupTitle);
    this->ui->uuidCheckBox->setChecked(this->m_searchParameters->searchUuid);
    this->ui->regexCheckBox->setChecked(this->m_searchParameters->useRegex);
    this->ui->caseSensitiveCheckBox->setChecked(this->m_searchParameters->caseSensitive);
}

/*!
    Updates the search parameters per the state of the GUI widgets.
 */
void SearchDatabaseDialog::updateSearchParameters()
{
    this->m_searchParameters->searchPattern = this->ui->searchLineEdit->text();
    this->m_searchParameters->searchTitle = this->ui->titleCheckBox->isChecked();
    this->m_searchParameters->searchUrl = this->ui->urlCheckBox->isChecked();
    this->m_searchParameters->searchUsername = this->ui->usernameCheckBox->isChecked();
    this->m_searchParameters->searchPassword = this->ui->passwordCheckBox->isChecked();
    this->m_searchParameters->searchNotes = this->ui->notesCheckBox->isChecked();
    this->m_searchParameters->searchRecovery = this->ui->recoveryInfoCheckBox->isChecked();
    this->m_searchParameters->searchCustomFields = this->ui->customFieldsCheckBox->isChecked();
    this->m_searchParameters->searchGroupTitle = this->ui->groupTitleCheckBox->isChecked();
    this->m_searchParameters->searchUuid = this->ui->uuidCheckBox->isChecked();
    this->m_searchParameters->useRegex = this->ui->regexCheckBox->isChecked();
    this->m_searchParameters->caseSensitive = this->ui->caseSensitiveCheckBox->isChecked();
}
