#include "databasereader.h"
#include "database.h"
#include "database_keys.h"
#include "databasecrypto.h"
#include "entry.h"
#include "group.h"
#include <QtXml>

/*!
    \class DatabaseReader

    Provides a simple interface to read Silverlock database files from a QIODevice.

    \sa DatabaseWriter
 */

/*!
    Constructs a new DatabaseReader.
 */
DatabaseReader::DatabaseReader(QObject *parent) :
    QObject(parent)
{
}

/*!
    Reads a database from the specified QIODevice and returns a pointer to a Database, using
    \a password as the key to decrypt the contents.

    Databases are encrypted using 256-bit AES.

    The method first checks if the QIODevice is open and readable. If so, it proceeds
    to decrypt the contents using \a password as the key and then read the XML tree
    into a Database object. If the device is not open or readable, the decryption
    fails, or there is a versioning problem, the method returns a \c NULL pointer.

    For error information, see \link errorString() \endlink.

    \param device The device to read the database from.
    \param password The passphrase used to decrypt the database.
 */
Database* DatabaseReader::read(QIODevice &device, const QString &password)
{
    // Clear the last error
    this->m_errorString = QString();

    // Check if the device is not open or is unreadable
    if (!device.isOpen() || !device.isReadable())
    {
        this->m_errorString = device.errorString();
        return NULL;
    }

    // Get the contents of the database file and create a new XML document
    QTextStream ts(&device);
    ts.setCodec("UTF-8");
    QString fileDataString = ts.readAll();

    QDomDocument doc;

    // Try and see if it's a valid XML document; if it is, we'll simply continue on and read
    // it (unencrypted mode), if it's not, we'll try to decrypt it (encrypted mode)
    if (!doc.setContent(fileDataString))
    {
        DatabaseCrypto::CryptoStatus status;
        QString extendedErrorInformation;
        fileDataString = DatabaseCrypto::decrypt(fileDataString, password, &status, &extendedErrorInformation);
        if (status != DatabaseCrypto::NoError)
        {
            this->m_errorString = DatabaseCrypto::statusMessage(status);
            if (!extendedErrorInformation.isEmpty())
            {
                this->m_errorString += " Details: " + extendedErrorInformation;
            }

            return NULL;
        }
    }

    if (doc.setContent(fileDataString))
    {
        QDomElement root = doc.documentElement();
        if (root.tagName() != XML_DATABASE)
        {
            this->m_errorString = tr("Missing root XML element.");
            return NULL;
        }

        QString version = root.attribute(XML_VERSION).simplified();
        if (version != Database::version())
        {
            this->m_errorString = tr("Unsupported database version: %1").arg(version);
            return NULL;
        }

        Database *db = new Database();
        db->fromXml(root);

        // Recursively read in all the groups and their subnodes
        this->readGroup(db, root);

        return db;
    }
    else
    {
        this->m_errorString = tr("Unable to parse the XML tree.");
        return NULL;
    }

    return NULL;
}

/*!
    Reads nodes in \a element as children of \a group.

    \param group The group node to which nodes read in this method should be added to.
    If this parameter is \c NULL, the method returns immediately.
    \param element The element whose child nodes are being read.
 */
void DatabaseReader::readGroup(Group *const group, const QDomElement &element)
{
    if (!group)
    {
        return;
    }

    // Find the first child of the element we were given to process
    QDomNode node = element.firstChild();
    while (!node.isNull())
    {
        // Convert the node into an element, which should be either a group or entry tag
        QDomElement e = node.toElement();
        if (!e.isNull())
        {
            if (e.tagName() == XML_GROUP)
            {
                Group *ourGroup = new Group();
                ourGroup->fromXml(e);
                ourGroup->setParentNode(group);

                // Call the method again to recursively add child nodes to the one we just added
                this->readGroup(ourGroup, e);
            }
            else if (e.tagName() == XML_ENTRY)
            {
                // If we found an entry, the process here is simple - read all the
                // properties and then add it to the group's list of entries
                Entry *entry = new Entry();
                entry->fromXml(e);
                entry->setParentNode(group);
            }
        }

        // Continue on with the loop to process the next sibling node (which can be a group or entry)
        node = node.nextSibling();
    }
}

/*!
    Returns a human-readable description of the last error that occurred when reading a database.
 */
QString DatabaseReader::errorString()
{
    return this->m_errorString;
}
