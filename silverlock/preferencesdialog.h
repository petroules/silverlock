#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include "guardeddialog.h"

namespace Ui
{
    class PreferencesDialog;
}

class SilverlockPreferences;

class PreferencesDialog : public GuardedDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = NULL);
    ~PreferencesDialog();

protected:
    void load();
    void save();
    void getMessages(QStringList &errors, QStringList &warnings, QStringList &information) const;

private:
    void updateFileAssociationState();
    void load(const SilverlockPreferences &prefs);
    void restoreDefaults();

    Ui::PreferencesDialog *ui;
    SilverlockPreferences *m_preferences;

private slots:
    void on_removeAssocPushButton_clicked();
    void on_createAssocPushButton_clicked();
    void on_buttonBox_clicked(QAbstractButton* button);
};

#endif // PREFERENCESDIALOG_H
