#ifndef SEARCHDATABASEDIALOG_H
#define SEARCHDATABASEDIALOG_H

#include <QtGui>
#include <silverlocklib.h>

namespace Ui {
    class SearchDatabaseDialog;
}

class SearchDatabaseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDatabaseDialog(const Database *const database, QWidget *parent = NULL);
    ~SearchDatabaseDialog();
    const Database *const database() const;
    void setDatabase(const Database *const database);
    SearchParameters* searchParameters() const;
    void setSearchParameters(SearchParameters *searchParameters);
    const QList<Entry*>& entriesFound() const;

public slots:
    void accept();

private:
    Ui::SearchDatabaseDialog *ui;
    const Database *m_database;
    SearchParameters *m_searchParameters;
    QList<Entry*> m_entries;

    QString inputErrorString() const;

private slots:
    void updateWidgets();
    void updateSearchParameters();
};

#endif // SEARCHDATABASEDIALOG_H
