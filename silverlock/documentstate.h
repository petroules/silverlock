#ifndef DOCUMENTSTATE_H
#define DOCUMENTSTATE_H

#include <QtCore>

class Database;

class DocumentState : public QObject
{
    Q_OBJECT

public:
    explicit DocumentState(QObject *parent = NULL);
    ~DocumentState();
    Database* database() const;
    QString currentFile() const;
    void setCurrentFile(const QString &currentFile);
    bool isUntitled() const;
    void setUntitled(bool isUntitled);
    bool hasDocument() const;
    void load(Database *database);
    void close();

signals:
    void stateChanged();

    /*!
        Emitted when the database is modified.
     */
    void modified();

private:
    Database *m_database;
    QString m_currentFile;
    bool m_isUntitled;

};

#endif // DOCUMENTSTATE_H
