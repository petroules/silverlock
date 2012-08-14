#ifndef NEWDATABASEWIZARD_H
#define NEWDATABASEWIZARD_H

#include <QWizard>

namespace Ui
{
    class NewDatabaseWizard;
}

class Database;

class NewDatabaseWizard : public QWizard
{
    Q_OBJECT

public:
    explicit NewDatabaseWizard(QWidget *parent = 0);
    ~NewDatabaseWizard();
    Database* database() const;

private slots:
    void passwordChanged(QString password);
    void databaseCreated();
    void passwordVisibilityToggled(bool checked);

private:
    int passwordComplexityRating(const QString &password) const;

    Ui::NewDatabaseWizard *ui;
    Database *m_database;
};

#endif // NEWDATABASEWIZARD_H
