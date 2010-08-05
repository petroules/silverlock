#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "silverlockpreferences.h"

/*!
    \class PreferencesDialog

    The PreferencesDialog class provides a dialog allowing the user to access and modify application
    preferences.
 */

/*!
    Constructs a new PreferencesDialog.

    \param parent The parent widget of the dialog.
 */
PreferencesDialog::PreferencesDialog(QWidget *parent) :
    GuardedDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    this->ui->setupUi(this);
    this->load();

    // TODO: To be restored in a future version (perhaps Silverlock 1.2?)
    this->ui->fileAssociationsGroupBox->setVisible(false);
}

/*!
    Destroys the dialog.
 */
PreferencesDialog::~PreferencesDialog()
{
    delete this->ui;
}

/*!
    Updates the state of the file associations group box.

    \todo For a future Silverlock release...
 */
void PreferencesDialog::updateFileAssociationState()
{
    bool set = this->m_preferences->isFileAssociationSet();
    this->ui->createAssocPushButton->setEnabled(!set);
    this->ui->removeAssocPushButton->setEnabled(set);
    this->ui->fileAssociationStatusLabel->setText(QString("File associations are currently <b>%1</b>").arg(set ? "enabled" : "disabled"));
    this->ui->fileAssociationStatusLabel->setStyleSheet(QString("color: %1;").arg(set ? "green" : "red"));
}

/*!
    Initializes the dialog with the current preferences.
 */
void PreferencesDialog::load()
{
    this->load(SilverlockPreferences::instance());
}

/*!
    Initializes the dialog with the specified set of preferences.
 */
void PreferencesDialog::load(const SilverlockPreferences &prefs)
{
    this->ui->lockWorkspaceTimeoutCheckBox->setChecked(prefs.lockWorkspaceTimeoutEnabled());
    this->ui->lockWorkspaceTimeoutSpinBox->setValue(prefs.lockWorkspaceTimeout());
    this->ui->autoClearClipboardCheckBox->setChecked(prefs.autoClearClipboardEnabled());
    this->ui->autoClearClipboardSpinBox->setValue(prefs.autoClearClipboard());
    this->ui->autoExitCheckBox->setChecked(prefs.autoExitEnabled());
    this->ui->autoExitSpinBox->setValue(prefs.autoExit());
    this->ui->lockWhenMinimizingCheckBox->setChecked(prefs.lockWhenMinimizing());
    this->ui->clearClipboardWhenClosingCheckBox->setChecked(prefs.clearClipboardWhenClosing());
    this->ui->minimizeToTrayCheckBox->setChecked(prefs.minimizeToTray());
    this->ui->minimizeAfterClipboardCheckBox->setChecked(prefs.minimizeAfterClipboard());
    this->ui->minimizeAfterLockCheckBox->setChecked(prefs.minimizeAfterLock());
    this->ui->runAtStartupCheckBox->setChecked(prefs.runAtStartup());
    this->ui->runAtStartupCheckBox->setEnabled(prefs.runAtStartupSupported());
    this->ui->openLastDatabaseCheckBox->setChecked(prefs.openLastDatabase());
    this->ui->updateOnStartupCheckBox->setChecked(prefs.updateOnStartup());
    this->ui->autoSaveOnCloseCheckBox->setChecked(prefs.autoSaveOnClose());

    this->ui->createAssocPushButton->setIcon(this->style()->standardIcon(QStyle::SP_VistaShield));
    this->ui->removeAssocPushButton->setIcon(this->style()->standardIcon(QStyle::SP_VistaShield));
#ifndef Q_OS_WIN
    this->ui->createAssocPushButton->setEnabled(false);
    this->ui->removeAssocPushButton->setEnabled(false);
#else
    this->updateFileAssociationState();
#endif
}

/*!
    Updates the preferences per the options selected in the dialog.
 */
void PreferencesDialog::save()
{
    SilverlockPreferences &prefs = SilverlockPreferences::instance();

    prefs.setLockWorkspaceTimeoutEnabled(this->ui->lockWorkspaceTimeoutCheckBox->isChecked());
    prefs.setLockWorkspaceTimeout(this->ui->lockWorkspaceTimeoutSpinBox->value());
    prefs.setAutoClearClipboardEnabled(this->ui->autoClearClipboardCheckBox->isChecked());
    prefs.setAutoClearClipboard(this->ui->autoClearClipboardSpinBox->value());
    prefs.setAutoExitEnabled(this->ui->autoExitCheckBox->isChecked());
    prefs.setAutoExit(this->ui->autoExitSpinBox->value());
    prefs.setLockWhenMinimizing(this->ui->lockWhenMinimizingCheckBox->isChecked());
    prefs.setClearClipboardWhenClosing(this->ui->clearClipboardWhenClosingCheckBox->isChecked());
    prefs.setMinimizeToTray(this->ui->minimizeToTrayCheckBox->isChecked());
    prefs.setMinimizeAfterClipboard(this->ui->minimizeAfterClipboardCheckBox->isChecked());
    prefs.setMinimizeAfterLock(this->ui->minimizeAfterLockCheckBox->isChecked());
    prefs.setRunAtStartup(this->ui->runAtStartupCheckBox->isChecked());
    prefs.setOpenLastDatabase(this->ui->openLastDatabaseCheckBox->isChecked());
    prefs.setUpdateOnStartup(this->ui->updateOnStartupCheckBox->isChecked());
    prefs.setAutoSaveOnClose(this->ui->autoSaveOnCloseCheckBox->isChecked());

    if (this->ui->clearRecentFilesListPushButton->isChecked())
    {
        prefs.clearRecentFiles();
    }

    if (this->ui->clearWindowSettingsPushButton->isChecked())
    {
        prefs.clearWindowSettings();
    }
}

/*!
    Updates the dialog state to reflect all default settings.

    save() must still be called to commit the changes.
 */
void PreferencesDialog::restoreDefaults()
{
    this->load(SilverlockPreferences::defaults());
}

void PreferencesDialog::getMessages(QStringList &errors, QStringList &warnings, QStringList &information) const
{
    Q_UNUSED(errors);
    Q_UNUSED(warnings);

    if (this->ui->clearWindowSettingsPushButton->isChecked())
    {
        information.append(tr("Clearing the window settings will take effect when you next restart the application."));
    }
}

/*!
    Performs actions for different buttons in the button box.
 */
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

/*!
    Creates file associations in Windows Explorer.

    \todo For a future Silverlock release...
 */
void PreferencesDialog::on_createAssocPushButton_clicked()
{
    this->m_preferences->setFileAssociationActive(true);
    this->updateFileAssociationState();
}

/*!
    Removes file associations in Windows Explorer.

    \todo For a future Silverlock release...
 */
void PreferencesDialog::on_removeAssocPushButton_clicked()
{
    this->m_preferences->setFileAssociationActive(false);
    this->updateFileAssociationState();
}
