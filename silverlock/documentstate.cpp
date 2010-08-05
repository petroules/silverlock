#include "documentstate.h"
#include <silverlocklib.h>

/*!
    \class DocumentState

    The DocumentState class is used to keep track of information regarding databases currently
    opened in Silverlock.
 */

/*!
    Constructs a new DocumentState.
 */
DocumentState::DocumentState(QObject *parent) :
    QObject(parent), m_database(NULL), m_currentFile(NULL),
    m_isUntitled(true), m_isLocked(false)
{
}

/*!
    Destroys the document state. See close().
 */
DocumentState::~DocumentState()
{
    this->close();
}

/*!
    Returns a pointer to the currently loaded database, or \c NULL if no database is loaded.
 */
Database* DocumentState::database() const
{
    return this->m_database;
}

/*!
    \property DocumentState::currentFile

    This property holds the file name of the currently loaded database.
 */

QString DocumentState::currentFile() const
{
    return this->m_currentFile;
}

void DocumentState::setCurrentFile(const QString &currentFile)
{
    if (this->m_currentFile != currentFile)
    {
        this->m_currentFile = currentFile;
        emit this->stateChanged();
    }
}

/*!
    \property DocumentState::untitled

    This property holds a value indicating whether the currently loaded database has a name.

    If the database is untitled, it cannot be saved without first prompting the user for a file
    name.
 */

bool DocumentState::isUntitled() const
{
    return this->m_isUntitled;
}

void DocumentState::setUntitled(bool isUntitled)
{
    if (this->m_isUntitled != isUntitled)
    {
        this->m_isUntitled = isUntitled;
        emit this->stateChanged();
    }
}

/*!
    Returns a value indicating whether a database is currently loaded.
 */
bool DocumentState::hasDocument() const
{
    return this->m_database;
}

/*!
    Loads the specified database, closing any previously opened one.

    \param database The database to load.
 */
void DocumentState::load(Database *database)
{
    this->close();

    // Store the database reference and check if it's not NULL
    if ((this->m_database = database))
    {
        QObject::connect(this->m_database, SIGNAL(treeModified()), this, SIGNAL(treeModified()));
    }

    emit this->stateChanged();
}

/*!
    Clears the document state by deleting the currently managed database (if any), setting the
    current file name to an empty string and setting untitled to \c true.
 */
void DocumentState::close()
{
    if (this->m_database)
    {
        delete this->m_database;
        this->m_database = NULL;
        emit this->stateChanged();
    }

    this->setCurrentFile(QString());
    this->setUntitled(true);
}

/*!
    Returns a value indicating whether the interface is locked.
 */
bool DocumentState::isLocked() const
{
    return this->m_isLocked;
}

/*!
    Notifies the document state that the interface is currently in locked state.
 */
void DocumentState::lock()
{
    if (!this->m_isLocked)
    {
        this->m_isLocked = true;
        emit this->stateChanged();
    }
}

/*!
    Notifies the document state that the interface is no longer in the locked state.
 */
void DocumentState::unlock()
{
    if (this->m_isLocked)
    {
        this->m_isLocked = false;
        emit this->stateChanged();
    }
}
