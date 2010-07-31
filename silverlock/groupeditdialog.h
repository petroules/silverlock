#ifndef GROUPEDITDIALOG_H
#define GROUPEDITDIALOG_H

#include "guardeddialog.h"

class Group;

namespace Ui
{
    class GroupEditDialog;
}

class GroupEditDialog : public GuardedDialog
{
    Q_OBJECT

public:
    GroupEditDialog(Group *group, QWidget *parent = 0);
    ~GroupEditDialog();
    Group* group() const;
    void setGroup(Group *group);

protected:
    void changeEvent(QEvent *e);
    void load();
    void save();
    void getMessages(QStringList &errors, QStringList &warnings, QStringList &information) const;

private:
    Ui::GroupEditDialog *ui;
    Group *m_group;

private slots:
    void hidePassword(bool checked);
};

#endif // GROUPEDITDIALOG_H
