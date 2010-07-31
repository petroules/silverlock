#include "documentstate.h"
#include <silverlocklib.h>

DocumentState::DocumentState(QObject *parent) :
    QObject(parent), m_database(NULL), m_currentFile(NULL),
    m_isUntitled(true), m_isLocked(false)
{
}

DocumentState::~DocumentState()
{
    this->close();
}

Database* DocumentState::database() const
{
    return this->m_database;
}

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

bool DocumentState::hasDocument() const
{
    return this->m_database;
}

void DocumentState::load(Database *database)
{
    this->close();

    // Store the database reference and check if it's not NULL
    if ((this->m_database = database))
    {
        QObject::connect(this->m_database, SIGNAL(modified()), this, SIGNAL(modified()));
    }

    emit this->stateChanged();
}

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

bool DocumentState::isLocked() const
{
    return this->m_isLocked;
}

void DocumentState::lock()
{
    if (!this->m_isLocked)
    {
        this->m_isLocked = true;
        emit this->stateChanged();
    }
}

void DocumentState::unlock()
{
    if (this->m_isLocked)
    {
        this->m_isLocked = false;
        emit this->stateChanged();
    }
}
