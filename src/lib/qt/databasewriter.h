#ifndef DATABASEWRITER_H
#define DATABASEWRITER_H

#include "silverlocklib_global.h"
#include <QtCore>

class Database;
class Group;
class QDomDocument;
class QDomNode;

class SILVERLOCK_API DatabaseWriter : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseWriter(QObject *parent = NULL);
    bool write(const Database *const database, QIODevice &device, bool encrypt = true);
    QString errorString();

private:
    void append(QDomDocument &document, QDomNode &element, const Group *const group) const;
    QString m_errorString;
};

#endif // DATABASEWRITER_H
