#ifndef DATABASEPRINTDIALOG_H
#define DATABASEPRINTDIALOG_H

#include <QtGui>

namespace Ui
{
    class DatabasePrintDialog;
}

class Database;
class Entry;
class Group;

class DatabasePrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DatabasePrintDialog(Database *database, QWidget *parent = NULL);
    ~DatabasePrintDialog();

public slots:
    void print();

private:
    Ui::DatabasePrintDialog *ui;
    Database *m_database;

    QString groupText(const Group *group) const;
    QString entryText(const Entry *entry) const;

private slots:
    void selectionLinkActivated(const QString &link);
    void selectAll(bool select);
    void updatePreview();
};

#endif // DATABASEPRINTDIALOG_H
