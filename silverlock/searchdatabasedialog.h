#ifndef SEARCHDATABASEDIALOG_H
#define SEARCHDATABASEDIALOG_H

#include "guardeddialog.h"
#include <silverlocklib.h>

namespace Ui
{
    class SearchDatabaseDialog;
}

class SearchDatabaseDialog : public GuardedDialog
{
    Q_OBJECT

public:
    explicit SearchDatabaseDialog(const Group *const group, QWidget *parent = NULL);
    ~SearchDatabaseDialog();
    const Group* group() const;
    void setGroup(const Group *group);
    SearchParameters* searchParameters() const;
    void setSearchParameters(SearchParameters *searchParameters);
    const QList<Entry*>& entriesFound() const;

protected:
    void save();
    void getMessages(QStringList &errors, QStringList &warnings, QStringList &information) const;

private:
    Ui::SearchDatabaseDialog *ui;
    const Group *m_group;
    SearchParameters *m_searchParameters;
    QList<Entry*> m_entries;

private slots:
    void updateWidgets();
    void updateSearchParameters();
};

#endif // SEARCHDATABASEDIALOG_H
