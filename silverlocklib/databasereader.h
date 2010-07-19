#ifndef DATABASEREADER_H
#define DATABASEREADER_H

#include "silverlocklib_global.h"
#include <QtXml>

class Database;
class Group;

class SILVERLOCKLIBSHARED_EXPORT DatabaseReader : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseReader(QObject *parent = 0);
    Database* read(QIODevice &device, const QString &password);
    QString errorString();

private:
    void readGroup(Group *const group, const QDomElement &element, bool &process);
    QString m_errorString;
};

#endif // DATABASEREADER_H
