#include "database.h"

Database::Database(const QString &name, const QString &password)
    : m_name(name), m_password(password), m_rootGroup(new Group(name))
{
}

Database::~Database()
{
    delete this->m_rootGroup;
    this->m_rootGroup = NULL;
}

QString Database::name() const
{
    return this->m_name;
}

void Database::setName(const QString &name)
{
    this->m_name = name;
    this->m_rootGroup->setName(name);
}

QString Database::password() const
{
    return this->m_password;
}

void Database::setPassword(const QString &password)
{
    this->m_password = password;
}

Group* Database::rootGroup() const
{
    return this->m_rootGroup;
}

int Database::countGroups()
{
    int total = 0;

    for (int i = 0; i < this->rootGroup()->groups().count(); i++)
    {
        total += this->rootGroup()->groups().at(i)->countGroups();
    }

    return total;
}

int Database::countEntries()
{
    int total = 0;

    for (int i = 0; i < this->rootGroup()->groups().count(); i++)
    {
        total += this->rootGroup()->groups().at(i)->countEntries();
    }

    return total + this->rootGroup()->entries().count();
}

bool Database::isGroup(const QUuid &uuid) const
{
    return this->findGroup(uuid) != NULL;
}

bool Database::isEntry(const QUuid &uuid) const
{
    return this->findEntry(uuid) != NULL;
}

Group* Database::findGroup(const QUuid &uuid) const
{
    return this->findGroup(uuid, this->m_rootGroup);
}

Group* Database::findGroup(const QUuid &uuid, Group *const currentGroup) const
{
    if (currentGroup == NULL)
    {
        return NULL;
    }

    if (currentGroup->uuid() == uuid)
    {
        return currentGroup;
    }

    for (int i = 0; i < currentGroup->groups().count(); i++)
    {
        Group* candidate = this->findGroup(uuid, currentGroup->groups().at(i));
        if (candidate != NULL && candidate->uuid() == uuid)
        {
            return candidate;
        }
    }

    return NULL;
}

Entry* Database::findEntry(const QUuid &uuid) const
{
    return this->findEntry(uuid, this->m_rootGroup);
}

Entry* Database::findEntry(const QUuid &uuid, Group *const currentGroup) const
{
    if (currentGroup == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < currentGroup->entries().count(); i++)
    {
        Entry* entry = currentGroup->entries().at(i);
        if (entry->uuid() == uuid)
        {
            return entry;
        }
    }

    for (int i = 0; i < currentGroup->groups().count(); i++)
    {
        Entry* candidate = this->findEntry(uuid, currentGroup->groups().at(i));
        if (candidate != NULL && candidate->uuid() == uuid)
        {
            return candidate;
        }
    }

    return NULL;
}

void Database::append(QDomDocument &document, QDomElement &element, Group *const group) const
{
    if (group == NULL)
    {
        return;
    }

    QDomElement cat = document.createElement("Category");
    cat.setAttribute("UUID", group->uuid().toString());
    cat.setAttribute("Name", group->name());
    element.appendChild(cat);

    for (int i = 0; i < group->groups().count(); i++)
    {
        this->append(document, cat, group->groups().at(i));
    }

    for (int i = 0; i < group->entries().count(); i++)
    {
        Entry* account = group->entries().at(i);
        QDomElement acc = document.createElement("Account");
        acc.setAttribute("UUID", account->uuid().toString());
        acc.setAttribute("Title", account->title());
        acc.setAttribute("Category", account->url().toString());
        acc.setAttribute("Username", account->username());
        acc.setAttribute("Password", account->password());
        acc.setAttribute("EmailAddress", account->emailAddress());
        acc.setAttribute("Notes", account->notes());

        QDomElement recovery = document.createElement("RecoveryInfo");
        acc.appendChild(recovery);

        QHashIterator<QString, QString> i(account->recoveryInfo());
        while (i.hasNext())
        {
            i.next();
            QDomElement pair = document.createElement("QuestionAnswerPair");
            pair.setAttribute("Question", i.key());
            pair.setAttribute("Answer", i.value());
            recovery.appendChild(pair);
        }

        QDomElement additional = document.createElement("AdditionalData");
        acc.appendChild(additional);

        QHashIterator<QString, QString> j(account->additionalData());
        while (j.hasNext())
        {
            j.next();
            QDomElement pair = document.createElement("AdditionalDataPair");
            pair.setAttribute("Key", j.key());
            pair.setAttribute("Value", j.value());
            additional.appendChild(pair);
        }

        cat.appendChild(acc);
    }
}

Database* Database::read(QIODevice &device, const QString &password)
{
    QDomDocument doc("AccountDatabase");
    if (device.isOpen() && device.isReadable())
    {
        if (doc.setContent(&device))
        {
            QDomElement root = doc.documentElement();
            if (root.tagName() == "AccountDatabase" && root.attribute("Version").toInt() == DATABASE_VERSION)
            {
                QDomNode n = root.firstChild();
                /*while (!n.isNull())
                {
                    QDomElement e = n.toElement();
                    if (!e.isNull())
                    {
                        if (e.tagName() == "contact")
                        {
                            Contact c;

                            c.name = e.attribute( "name", "" );
                            c.phone = e.attribute( "phone", "" );
                            c.eMail = e.attribute( "email", "" );

                            QMessageBox::information( 0, "Contact", c.name + "\n" + c.phone + "\n" + c.eMail );
                        }
                    }

                    n = n.nextSibling();
                }*/
            }
        }
    }

    return NULL;
}

bool Database::write(QIODevice &device) const
{
    QDomDocument document("AccountDatabase");

    QDomElement dbName = document.createElement("AccountDatabase");
    dbName.setAttribute("Version", DATABASE_VERSION);
    dbName.setAttribute("Name", this->m_name);
    dbName.setAttribute("Password", this->m_password);
    document.appendChild(dbName);

    this->append(document, dbName, this->m_rootGroup);

    if (device.isOpen() && device.isWritable())
    {
        QTextStream ts(&device);
        ts << document.toString(4);
        return true;
    }

    return false;
}
