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
    void load(const SilverlockPreferences &prefs);
    void restoreDefaults();

    Ui::PreferencesDialog *ui;

private slots:
    void on_buttonBox_clicked(QAbstractButton* button);
};

#endif // PREFERENCESDIALOG_H
