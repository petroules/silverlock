#ifndef GROUPEDITDIALOG_H
#define GROUPEDITDIALOG_H

#include <QtGui>

class Group;

namespace Ui
{
    class GroupEditDialog;
}

class GroupEditDialog : public QDialog
{
    Q_OBJECT

public:
    GroupEditDialog(Group *group, QWidget *parent = 0);
    ~GroupEditDialog();
    Group* group() const;
    void setGroup(Group *group);

public slots:
    void accept();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::GroupEditDialog *ui;
    Group *m_group;

    void read();
    void write() const;
    QString inputErrorString() const;
};

#endif // GROUPEDITDIALOG_H
