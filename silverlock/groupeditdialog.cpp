#include "groupeditdialog.h"
#include "ui_groupeditdialog.h"
#include <silverlocklib.h>

/*!
    \class GroupEditDialog

    The GroupEditDialog class provides a dialog allowing the user to modify the properties of a
    Group.
 */

/*!
    Constructs a new GroupEditDialog.

    \param group The group to begin editing.
    \param parent The parent widget of the dialog.
 */
GroupEditDialog::GroupEditDialog(Group *group, QWidget *parent) :
    GuardedDialog(parent),
    ui(new Ui::GroupEditDialog), m_group(group)
{
    this->ui->setupUi(this);
    this->load();

    // Hide the password by default
    this->ui->revealToolButton->setChecked(true);
}

/*!
    Destroys the GroupEditDialog.
 */
GroupEditDialog::~GroupEditDialog()
{
    delete this->ui;
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
    this->load();
}

/*!
    Populates the dialog fields with data from the group instance.
 */
void GroupEditDialog::load()
{
    if (this->m_group)
    {
        // Basic information
        this->ui->titleLineEdit->setText(this->m_group->title());

        // Read-only info
        this->ui->uuidLineEdit->setText(this->m_group->uuid().toString());
        this->ui->createdLineEdit->setText(this->m_group->created().toLocalTime().toString(Qt::SystemLocaleLongDate));
        this->ui->accessedLineEdit->setText(this->m_group->accessed().toLocalTime().toString(Qt::SystemLocaleLongDate));
        this->ui->modifiedLineEdit->setText(this->m_group->modified().toLocalTime().toString(Qt::SystemLocaleLongDate));

        Database *db = dynamic_cast<Database*>(this->m_group);
        if (db)
        {
            this->ui->passwordLineEdit->setText(db->password());
        }

        this->ui->passwordLabel->setVisible(db);
        this->ui->passwordLineEdit->setVisible(db);
        this->ui->revealToolButton->setVisible(db);
        this->setWindowTitle(db ? tr("Edit Database") : tr("Edit Group"));
    }
}

/*!
    Updates the group with data from the dialog fields.
 */
void GroupEditDialog::save()
{
    if (this->m_group)
    {
        // Basic information
        this->m_group->setTitle(this->ui->titleLineEdit->text());

        Database *db = dynamic_cast<Database*>(this->m_group);
        if (db)
        {
            db->setPassword(this->ui->passwordLineEdit->text());
        }
    }
}

void GroupEditDialog::getMessages(QStringList &errors, QStringList &warnings, QStringList &information) const
{
    Q_UNUSED(errors);
    Q_UNUSED(information);

    // If we're editing a database and the password box is empty...
    if (this->m_group && dynamic_cast<Database*>(this->m_group) && this->ui->passwordLineEdit->text().isEmpty())
    {
        warnings.append(tr("For security purposes, it is not recommended to use a blank password. Using a blank password is the same as using no password at all."));
    }
}

/*!
    Sets a value indicating whether the password should be hidden using asterisks.

    \param checked \c true to hide the password, \c false to display it verbatim.
 */
void GroupEditDialog::hidePassword(bool checked)
{
    this->ui->passwordLineEdit->setEchoMode(checked ? QLineEdit::Password : QLineEdit::Normal);
}
