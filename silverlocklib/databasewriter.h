#ifndef DATABASEWRITER_H
#define DATABASEWRITER_H

#include "silverlocklib_global.h"
#include <QtXml>

class Database;
class Group;

class SILVERLOCKLIBSHARED_EXPORT DatabaseWriter : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseWriter(QObject *parent = NULL);
    bool write(const Database *const database, QIODevice &device, bool encrypt = true) const;

private:
    void append(QDomDocument &document, QDomNode &element, const Group *const group) const;
};

#endif // DATABASEWRITER_H
