#include "selectgroupdialog.h"
#include "ui_selectgroupdialog.h"

SelectGroupDialog::SelectGroupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectGroupDialog)
{
    this->ui->setupUi(this);
    this->ui->groupBrowser->setMultiselect(false);
}

SelectGroupDialog::~SelectGroupDialog()
{
    delete this->ui;
}
