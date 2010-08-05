#ifndef SELECTGROUPDIALOG_H
#define SELECTGROUPDIALOG_H

#include "ui_selectgroupdialog.h"
#include <QtGui>

namespace Ui
{
    class SelectGroupDialog;
}

class Database;
class SilverlockPreferences;

class SelectGroupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectGroupDialog(QWidget *parent = NULL);
    ~SelectGroupDialog();

    /*!
        \copydoc GroupBrowserWidget::selectedUuid
     */
    QUuid selectedUuid() const
    {
        return this->ui->groupBrowser->selectedUuid();
    }

    /*!
        \copydoc GroupBrowserWidget::selectedUuids
     */
    QList<QUuid> selectedUuids() const
    {
        return this->ui->groupBrowser->selectedUuids();
    }

    /*!
        \copydoc GroupBrowserWidget::populate
     */
    void populate(Database *const database)
    {
        this->ui->groupBrowser->populate(database);
    }

    /*!
        \copydoc GroupBrowserWidget::clear
     */
    void clear()
    {
        this->ui->groupBrowser->clear();
    }

private:
    Ui::SelectGroupDialog *ui;
};

#endif // SELECTGROUPDIALOG_H
