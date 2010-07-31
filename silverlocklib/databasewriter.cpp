#include "databasewriter.h"
#include "database.h"
#include "database_keys.h"
#include "group.h"
#include "entry.h"
#include "databasecrypto.h"

DatabaseWriter::DatabaseWriter(QObject *parent) :
    QObject(parent)
{
}

/*!
    Writes the database to the specified QIODevice and returns whether it succeeded.

    The database is written in an XML format encrypted with 256-bit AES.

    The method first checks if the QIODevice is open and writable. If so, it proceeds
    to write the database contents to the device with a 4 character tab width. If the
    device is not open or writable, the method immediately returns false.
 */
bool DatabaseWriter::write(const Database *const database, QIODevice &device, bool encrypt) const
{
    QDomDocument document(XML_DOCTYPE);

    this->append(document, document, database);

    if (device.isOpen() && device.isWritable())
    {
        QString fileDataString = document.toString(4);

        if (encrypt)
        {
            DatabaseCrypto::CryptoStatus status;
            fileDataString = DatabaseCrypto::encrypt(fileDataString, database->password(), &status);
            if (status != DatabaseCrypto::NoError)
            {
                return false;
            }
        }

        QTextStream ts(&device);
        ts << fileDataString;
        return true;
    }

    return false;
}

/*!
    Writes \a group to the \a document as a child of \a element.

    \param document The QDomDocument representing the XML document to write data to.
    \param domNode The DOM node which \a group should have its element created as a child of.
    \param group The group node whose contents are to be appended to the XML document.
 */
void DatabaseWriter::append(QDomDocument &document, QDomNode &domNode, const Group *const group) const
{
    if (!group)
    {
        return;
    }

    QDomElement element = group->toXml(document);
    domNode.appendChild(element);

    foreach (Group *childGroup, group->groups())
    {
        this->append(document, element, childGroup);
    }

    foreach (Entry *account, group->entries())
    {
        element.appendChild(account->toXml(document));
    }
}
