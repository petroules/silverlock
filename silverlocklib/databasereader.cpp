#include "databasereader.h"
#include "database.h"
#include "database_keys.h"
#include "entry.h"
#include "group.h"
#include "databasecrypto.h"

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

    DatabaseCrypto::CryptoStatus status;
    QString decrypted = DatabaseCrypto::decrypt(QString(device.readAll()), password, &status);
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

    QDomDocument doc(XML_DATABASE);
    if (doc.setContent(decrypted))
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

        Database *db = new Database(root.attribute(XML_TITLE), root.attribute(XML_DBPASSWORD));
        db->setUuid(root.attribute(XML_UUID));

        // Recursively read in all the groups and their subnodes
        bool noError = true;
        this->readGroup(db, root, noError);

        // If we didn't encounter an error during group processing...
        if (noError)
        {
            return db;
        }

        return NULL;
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
void DatabaseReader::readGroup(Group *const group, const QDomElement &element, bool &process)
{
    if (!group || !process)
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
                Group *ourGroup = new Group(e.attribute(XML_TITLE), group);
                ourGroup->setUuid(e.attribute(XML_UUID));

                // Little bit of error checking...
                if (ourGroup->uuid().isNull())
                {
                    if (process)
                    {
                        this->m_errorString = tr("Invalid group UUID.");
                        process = false;
                    }

                    return;
                }

                // Call the method again to recursively add child nodes to the one we just added
                this->readGroup(ourGroup, e, process);
            }
            else if (e.tagName() == XML_ENTRY)
            {
                // If we found an entry, the process here is simple - read all the
                // properties and then add it to the group's list of entries
                Entry *entry = new Entry(e.attribute(XML_TITLE), group);
                entry->setUuid(e.attribute(XML_UUID));
                entry->setUrl(e.attribute(XML_URL));
                entry->setUsername(e.attribute(XML_USERNAME));
                entry->setPassword(e.attribute(XML_PASSWORD));
                entry->setEmailAddress(e.attribute(XML_EMAILADDRESS));
                entry->setNotes(e.attribute(XML_NOTES));

                // Little bit of error checking...
                if (entry->uuid().isNull())
                {
                    if (process)
                    {
                        this->m_errorString = tr("Invalid entry UUID.");
                        process = false;
                    }

                    return;
                }

                // Here we'll process any subnodes of the entry containing additional information
                QDomNode entryNode = e.firstChild();
                while (!entryNode.isNull())
                {
                    QDomElement entryElement = entryNode.toElement();
                    if (!entryElement.isNull())
                    {
                        if (entryElement.tagName() == XML_ADDITIONALDATA)
                        {
                            entry->insertAdditionalData(entryElement.attribute(XML_ADNAME), entryElement.attribute(XML_ADVALUE));
                        }
                        else if (entryElement.tagName() == XML_RECOVERYINFO)
                        {
                            entry->insertRecoveryInfo(entryElement.attribute(XML_QUESTION), entryElement.attribute(XML_ANSWER));
                        }
                    }

                    entryNode = entryNode.nextSibling();
                }
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
