#include "databasereader.h"
#include "database.h"
#include "database_keys.h"
#include "entry.h"
#include "group.h"
#include "databasecrypto.h"
#include <liel.h>

DatabaseReader::DatabaseReader(QObject *parent) :
    QObject(parent)
{
}

/*!
    Reads a database from the specified QIODevice and returns a pointer to a Database,
    using \a password as the key to decrypt the contents.

    Databases are encrypted using 256-bit AES.

    The method first checks if the QIODevice is open and readable. If so, it proceeds
    to decrypt the contents using \a password as the key and then read the XML tree
    into a Database object. If the device is not open or readable, the decryption
    fails, or there is a versioning problem, the method returns a \c NULL pointer.

    For error information, see \link lastReadError() \endlink.
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
    QString fileDataString = QString(device.readAll());
    QDomDocument doc;

    // Try and see if it's a valid XML document; if it is, we'll simply continue on and read
    // it (unencrypted mode), if it's not, we'll try to decrypt it (encrypted mode)
    if (!doc.setContent(fileDataString))
    {
        DatabaseCrypto::CryptoStatus status;
        fileDataString = DatabaseCrypto::decrypt(fileDataString, password, &status);
        if (status != DatabaseCrypto::NoError)
        {
            if (status == DatabaseCrypto::MissingHeader)
            {
                this->m_errorString = tr("The file was missing its standard header.");
            }
            else if (status == DatabaseCrypto::VerificationFailed)
            {
                this->m_errorString = tr("The message authentication codes were mismatched. The file may have been corrupted or tampered with.");
            }
            else if (status == DatabaseCrypto::DecodingError)
            {
                this->m_errorString = tr("There was a problem decoding the file; either the password was invalid or the file may be corrupt.");
            }
            else if (status == DatabaseCrypto::UnknownError)
            {
                this->m_errorString = tr("An unknown error occurred while decoding the file.");
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

        QVersion version = QVersion(root.attribute(XML_VERSION));
        if (version != Database::version())
        {
            this->m_errorString = tr("Unsupported database version: ") + version.toString();
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

    \param db The database to which nodes belong.
    \param group The group node to which nodes read in this method should be added to.
    If this parameter is \c NULL, a group node read replace the database's root node.
    \param element The element whose child nodes are being read.
    \param process Whether to continue processing. This is used to avoid overwriting an error state.
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
