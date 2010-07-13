#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "silverlockpreferences.h"

PreferencesDialog::PreferencesDialog(SilverlockPreferences *preferences, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog), m_preferences(preferences)
{
    this->ui->setupUi(this);
}

PreferencesDialog::~PreferencesDialog()
{
    delete this->ui;
}

void PreferencesDialog::load()
{
    this->ui->checkBoxExpandTreeViewNodes->setChecked(this->m_preferences->expandTreeViewNodes());
}

void PreferencesDialog::save() const
{
    this->m_preferences->setExpandTreeViewNodes(this->ui->checkBoxExpandTreeViewNodes->isChecked());
}
