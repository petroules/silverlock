#ifndef DATABASEPRINTPREVIEWDIALOG_H
#define DATABASEPRINTPREVIEWDIALOG_H

#include <QtGui>
#include "databaseprinter.h"

namespace Ui
{
    class DatabasePrintPreviewDialog;
}

class Database;
class Entry;
class Group;

class DatabasePrintPreviewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DatabasePrintPreviewDialog(Database *database, QWidget *parent = NULL);
    ~DatabasePrintPreviewDialog();
    DatabasePrinterFields databasePrinterFields() const;

private:
    Ui::DatabasePrintPreviewDialog *ui;
    Database *m_database;

private slots:
    void selectionLinkActivated(const QString &link);
    void selectAll(bool select);
    void updatePreview();
};

#endif // DATABASEPRINTPREVIEWDIALOG_H
