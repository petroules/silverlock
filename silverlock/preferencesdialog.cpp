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

void PreferencesDialog::restoreDefaults()
{
    // Create a new instance so we don't persist the defaults
    // (allows the user to still click cancel and not save them)
    SilverlockPreferences *resetter = new SilverlockPreferences();
    resetter->restoreDefaults();

    // Hold a reference to the current preferences and temporarily
    // set the new instance and load the settings from it
    SilverlockPreferences *backup = this->m_preferences;
    this->m_preferences = resetter;
    this->load();

    // Restore the old instance and delete the new one
    this->m_preferences = backup;
    delete resetter;
}

void PreferencesDialog::accept()
{
    this->save();
    QDialog::accept();
}

void PreferencesDialog::on_buttonBox_clicked(QAbstractButton* button)
{
    if (this->m_preferences && this->ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole)
    {
        if (QMessageBox::warning(this, tr("Warning"), tr("Are you sure you wish to reset all preferences to their default settings?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
        {
            this->restoreDefaults();
        }
    }
}
