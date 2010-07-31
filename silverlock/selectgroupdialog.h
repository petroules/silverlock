#ifndef SELECTGROUPDIALOG_H
#define SELECTGROUPDIALOG_H

#include <QtGui>
#include "ui_selectgroupdialog.h"

class Database;
class SilverlockPreferences;

namespace Ui {
    class SelectGroupDialog;
}

class SelectGroupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectGroupDialog(QWidget *parent = 0);
    ~SelectGroupDialog();
    QUuid selectedUuid() const { return this->ui->groupBrowser->selectedUuid(); }
    QList<QUuid> selectedUuids() const { return this->ui->groupBrowser->selectedUuids(); }
    void populate(Database *const database) { this->ui->groupBrowser->populate(database); }
    void clear() { this->ui->groupBrowser->clear(); }

private:
    Ui::SelectGroupDialog *ui;
};

#endif // SELECTGROUPDIALOG_H
