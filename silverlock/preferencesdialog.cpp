#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "silverlockpreferences.h"

PreferencesDialog::PreferencesDialog(SilverlockPreferences *preferences, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog), m_preferences(NULL)
{
    this->ui->setupUi(this);
    this->setPreferences(preferences);
}

PreferencesDialog::~PreferencesDialog()
{
    delete this->ui;
}

SilverlockPreferences* PreferencesDialog::preferences() const
{
    return this->m_preferences;
}

void PreferencesDialog::setPreferences(SilverlockPreferences *preferences)
{
    if (this->m_preferences != preferences)
    {
        this->m_preferences = preferences;
        this->load();
    }
}

void PreferencesDialog::load()
{
    if (this->m_preferences)
    {
        this->ui->checkBoxExpandTreeViewNodes->setChecked(this->m_preferences->expandTreeViewNodes());
    }
}

void PreferencesDialog::save() const
{
    if (this->m_preferences)
    {
        this->m_preferences->setExpandTreeViewNodes(this->ui->checkBoxExpandTreeViewNodes->isChecked());
    }
}

void PreferencesDialog::accepted()
{
    this->save();
    QDialog::accept();
}
