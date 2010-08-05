#include "databasewriter.h"
#include "database.h"
#include "database_keys.h"
#include "databasecrypto.h"
#include "group.h"
#include "entry.h"

/*!
    \class DatabaseWriter

    Provides a simple interface to write Silverlock database files to a QIODevice.

    \sa DatabaseReader
 */

/*!
    Constructs a new DatabaseWriter.
 */
DatabaseWriter::DatabaseWriter(QObject *parent) :
    QObject(parent)
{
}

/*!
    Writes the database to the specified QIODevice and returns a value indicating whether it
    succeeded.

    The database is written in an XML format encrypted with 256-bit AES.

    The method first checks if the QIODevice is open and writable. If so, it proceeds
    to write the database contents to the device with a 4 character tab width. If the
    device is not open or writable, the method immediately returns \c false.

    For error information, see \link errorString() \endlink.

    \param database The database to export to \a device.
    \param device The device to write the database to.
    \param encrypt Whether to encrypt the database contents.
 */
bool DatabaseWriter::write(const Database *const database, QIODevice &device, bool encrypt)
{
    // Clear the last error
    this->m_errorString = QString();

    // Check if the device is not open or is unwritable
    if (!device.isOpen() || !device.isWritable())
    {
        this->m_errorString = device.errorString();
        return false;
    }

    QDomDocument document;
    this->append(document, document, database);
    QString fileDataString = document.toString(4);

    if (encrypt)
    {
        DatabaseCrypto::CryptoStatus status;
        fileDataString = DatabaseCrypto::encrypt(fileDataString, database->password(), &status);
        if (status != DatabaseCrypto::NoError)
        {
            this->m_errorString = DatabaseCrypto::statusMessage(status);
            return false;
        }
    }

    QTextStream ts(&device);
    ts << fileDataString;
    return true;
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

/*!
    Returns a human-readable description of the last error that occurred when writing a database.
 */
QString DatabaseWriter::errorString()
{
    return this->m_errorString;
}
