#ifndef GROUPEDITDIALOG_H
#define GROUPEDITDIALOG_H

#include <QtCore>
#include <QtGui>
#include <group.h>

namespace Ui {
    class GroupEditDialog;
}

class GroupEditDialog : public QDialog {
    Q_OBJECT
public:
    GroupEditDialog(Group *group, QWidget *parent = 0);
    ~GroupEditDialog();
    Group* group() const;
    void setGroup(Group *group);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::GroupEditDialog *ui;
    Group *m_group;

    void read();
    void write() const;
    QString inputErrorString() const;

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
};

#endif // GROUPEDITDIALOG_H
