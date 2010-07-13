#ifndef DATABASEWRITER_H
#define DATABASEWRITER_H

#include "silverlocklib_global.h"
#include <QtXml>

class Database;
class GroupNode;

class SILVERLOCKLIBSHARED_EXPORT DatabaseWriter : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseWriter(QObject *parent = 0);
    bool write(const Database *const database, QIODevice &device) const;

private:
    void append(QDomDocument &document, QDomNode &element, const GroupNode *const group) const;
};

#endif // DATABASEWRITER_H
