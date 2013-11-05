#ifndef DATABASEPRINTER_H
#define DATABASEPRINTER_H

#include <QtCore>
#include <silverlocklib.h>
#include "databaseprinterfields.h"

class QTextDocument;

class DatabasePrinter : public QObject
{
    Q_OBJECT

public:
    DatabasePrinter(const Database *database = NULL, QObject *parent = NULL);
    const Database* database() const;
    void setDatabase(const Database *database);
    QTextDocument* toTextDocument(const DatabasePrinterFields &fields) const;
    QString toHtml(const DatabasePrinterFields &fields) const;

private:
    static QString groupToHtml(const Group *group, const DatabasePrinterFields &fields = DatabasePrinterFields());
    static QString entryToHtml(const Entry *entry, const DatabasePrinterFields &fields = DatabasePrinterFields());

private:
    const Database *m_database;
};

#endif // DATABASEPRINTER_H
