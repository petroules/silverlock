#ifndef DATABASEREADER_H
#define DATABASEREADER_H

#include "silverlocklib_global.h"
#include <QtCore>

class Database;
class Group;
class QDomElement;

class SILVERLOCK_API DatabaseReader : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseReader(QObject *parent = 0);
    Database* read(QIODevice &device, const QString &password);
    QString errorString();

private:
    void readGroup(Group *const group, const QDomElement &element);
    QString m_errorString;
};

#endif // DATABASEREADER_H
