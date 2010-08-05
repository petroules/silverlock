#include "selectgroupdialog.h"
#include "ui_selectgroupdialog.h"

/*!
    \class SelectGroupDialog

    The SelectGroupDialog class provides a dialog allowing the user to select a group within a
    Database.
 */

/*!
    Constructs a new SelectGroupDialog.

    \param parent The parent widget of the dialog.
 */
SelectGroupDialog::SelectGroupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectGroupDialog)
{
    this->ui->setupUi(this);
    this->ui->groupBrowser->setMultiselect(false);
}

/*!
    Destroys the dialog.
 */
SelectGroupDialog::~SelectGroupDialog()
{
    delete this->ui;
}
