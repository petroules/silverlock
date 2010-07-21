#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QtGui>

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
    SilverlockPreferences* preferences() const;
    void setPreferences(SilverlockPreferences *preferences);

public slots:
    void accept();

private:
    void load();
    void save() const;
    void restoreDefaults();

    Ui::PreferencesDialog *ui;
    SilverlockPreferences *m_preferences;

private slots:
    void on_buttonBox_clicked(QAbstractButton* button);
};

#endif // PREFERENCESDIALOG_H
