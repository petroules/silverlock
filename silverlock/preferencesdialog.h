#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

class SilverlockPreferences;

namespace Ui {
    class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(SilverlockPreferences *preferences, QWidget *parent = 0);
    ~PreferencesDialog();

private:
    void load();
    void save() const;

    Ui::PreferencesDialog *ui;
    SilverlockPreferences *m_preferences;
};

#endif // PREFERENCESDIALOG_H
