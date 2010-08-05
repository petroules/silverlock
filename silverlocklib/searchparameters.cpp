#include "searchparameters.h"
#include "group.h"

/*!
    \struct SearchParameters

    The SearchParameters structure encapsulates a search pattern and various options
    for an search within a password database.

    Among the options encapsulated are the fields to be searched, which can be individually set to
    \c true or \c false. The fields that can be searched are: title, URL, username, password, notes,
    recovery question/answer, custom fields, group title, and UUID.

    Options to use regular expressions and whether to perform a case-sensitive search are also
    included.
 */

/*!
    Constructs a new SearchParameters.
 */
SearchParameters::SearchParameters(QObject *parent)
    : QObject(parent)
{
    this->setDefaults();
}

/*!
    Sets all fields to their default values.
 */
void SearchParameters::setDefaults()
{
    this->searchTitle = true;
    this->searchUrl = true;
    this->searchUsername = true;
    this->searchPassword = false;
    this->searchNotes = true;
    this->searchRecovery = false;
    this->searchCustomFields = true;
    this->searchGroupTitle = false;
    this->searchUuid = false;
    this->useRegex = false;
    this->caseSensitive = false;
    this->searchPattern = QString();
}

/*!
    Gets the case sensitivity as a Qt::CaseSensitivity enumerated constant for use with Qt classes.
 */
Qt::CaseSensitivity SearchParameters::caseSensitiveEnum() const
{
    return this->caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
}

/*!
    Determines whether at least one data field has been selected to search.
 */
bool SearchParameters::fieldsSelected() const
{
    return this->searchTitle || this->searchUrl || this->searchUsername ||
        this->searchPassword || this->searchNotes || this->searchRecovery ||
        this->searchCustomFields || this->searchGroupTitle || this->searchUuid;
}

/*!
    Gets a list of strings representing the data in a password entry.

    This method is used to search entries by iterating through the list and
    checking whether any of the items match the desired search pattern.

    \param entry The password entry to get the data from.
 */
QList<QString> SearchParameters::getDataList(const Entry &entry) const
{
    QList<QString> dataItems;

    if (this->searchTitle)
    {
        dataItems.append(entry.title());
    }

    if (this->searchUrl)
    {
        dataItems.append(entry.url().toString());
    }

    if (this->searchUsername)
    {
        dataItems.append(entry.username());
    }

    if (this->searchPassword)
    {
        dataItems.append(entry.password());
    }

    if (this->searchNotes)
    {
        dataItems.append(entry.notes());
    }

    if (this->searchRecovery)
    {
        QMapIterator<QString, QString> i(entry.recoveryInfo());
        while (i.hasNext())
        {
            i.next();
            dataItems.append(i.key());
            dataItems.append(i.value());
        }
    }

    if (this->searchCustomFields)
    {
        QMapIterator<QString, QString> i(entry.customFields());
        while (i.hasNext())
        {
            i.next();
            dataItems.append(i.key());
            dataItems.append(i.value());
        }
    }

    // If the entry doesn't have a parent node we won't check its group title
    if (this->searchGroupTitle && entry.parentNode())
    {
        dataItems.append(entry.parentNode()->title());
    }

    if (this->searchUuid)
    {
        dataItems.append(entry.uuid().toString());
    }

    return dataItems;
}
