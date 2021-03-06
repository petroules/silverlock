#ifndef DATABASEAUTHENTICATIONDIALOG_H
#define DATABASEAUTHENTICATIONDIALOG_H

#include "guardeddialog.h"

namespace Ui
{
    class DatabaseAuthenticationDialog;
}

class DatabaseAuthenticationDialog : public GuardedDialog
{
    Q_OBJECT
    Q_PROPERTY(QString password READ password WRITE setPassword)

public:
    explicit DatabaseAuthenticationDialog(const QString &fileName, QWidget *parent = NULL);
    ~DatabaseAuthenticationDialog();
    QString password() const;
    void setPassword(const QString &password);

protected:
    void getMessages(QStringList &errors, QStringList &warnings, QStringList &information) const;

private:
    Ui::DatabaseAuthenticationDialog *ui;

private slots:
    void hidePassword(bool checked);
};

#endif // DATABASEAUTHENTICATIONDIALOG_H
