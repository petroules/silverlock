#include "groupeditdialog.h"
#include "ui_groupeditdialog.h"
#include <silverlocklib.h>

/*!
    \class GroupEditDialog

    The GroupEditDialog class represents a dialog that
    allows the user to edit the properties of a Group.
 */

/*!
    Constructs a new GroupEditDialog.

    \param group The group to begin editing.
    \param parent The parent widget of the dialog.
 */
GroupEditDialog::GroupEditDialog(Group *group, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GroupEditDialog), m_group(group)
{
    this->ui->setupUi(this);
    this->read();
}

/*!
    Destroys the GroupEditDialog.
 */
GroupEditDialog::~GroupEditDialog()
{
    delete this->ui;
}

/*!
    \internal
 */
void GroupEditDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange:
            this->ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

/*!
    Gets the group being edited by this dialog.
 */
Group* GroupEditDialog::group() const
{
    return this->m_group;
}

/*!
    Sets the group to be edited by this dialog.
 */
void GroupEditDialog::setGroup(Group *group)
{
    this->m_group = group;
    this->read();
}

/*!
    Populates the dialog fields with data from the group instance.
 */
void GroupEditDialog::read()
{
    if (this->m_group)
    {
        // Basic information
        this->ui->titleLineEdit->setText(this->m_group->title());

        // UUID
        this->ui->uuidLineEdit->setText(this->m_group->uuid().toString());
    }
}

/*!
    Updates the group with data from the dialog fields.
 */
void GroupEditDialog::write() const
{
    if (this->m_group)
    {
        // Basic information
        this->m_group->setTitle(this->ui->titleLineEdit->text());
    }
}

/*!
    Closes the dialog and saves any changes if there are no input errors,
    or displays an error message if there are input errors.
 */
void GroupEditDialog::accept()
{
    QString errorString = this->inputErrorString();
    if (errorString.isEmpty())
    {
        this->write();
        QDialog::accept();
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), tr("The following input errors have occurred:") + errorString);
    }
}

/*!
    Gets an HTML-formatted string containing the list of input errors, or an empty string if there are no errors.
 */
QString GroupEditDialog::inputErrorString() const
{
    QString errorString;

    if (!errorString.isEmpty())
    {
        return "<ul>" + errorString + "</ul>";
    }

    return errorString;
}
