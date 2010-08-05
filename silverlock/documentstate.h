#ifndef DOCUMENTSTATE_H
#define DOCUMENTSTATE_H

#include <QtCore>

class Database;

class DocumentState : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentFile READ currentFile WRITE setCurrentFile)
    Q_PROPERTY(bool untitled READ isUntitled WRITE setUntitled)

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
    bool isLocked() const;
    void lock();
    void unlock();

signals:
    /*!
        Emitted when a database is loaded or closed, the workspace is locked or unlocked, or any
        other property is changed.
     */
    void stateChanged();

    /*!
        Emitted when the database is modified.
     */
    void treeModified();

private:
    Database *m_database;
    QString m_currentFile;
    bool m_isUntitled;
    bool m_isLocked;
};

#endif // DOCUMENTSTATE_H
