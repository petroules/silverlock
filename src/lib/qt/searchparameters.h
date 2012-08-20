#ifndef SEARCHPARAMETERS_H
#define SEARCHPARAMETERS_H

#include "silverlocklib_global.h"
#include "entry.h"
#include <QtCore>

struct SILVERLOCK_API SearchParameters : public QObject
{
    Q_OBJECT

public:
    explicit SearchParameters(QObject *parent = NULL);

    /*!
        The search pattern to use when finding entries in a password database.
        This string will be treated as a regular expression if \a useRegex
        is \c true, and will be treated as a normal string if \a useRegex is \c false.
        The default is an empty string.
     */
    QString searchPattern;

    /*!
        Indicates whether the title field should be searched.
        The default is \c true.
     */
    bool searchTitle;

    /*!
        Indicates whether the username field should be searched.
        The default is \c true.
     */
    bool searchUsername;

    /*!
        Indicates whether the password field should be searched.
        The default is \c false.
     */
    bool searchPassword;

    /*!
        Indicates whether the URL field should be searched.
        The default is \c true.
     */
    bool searchUrl;

    /*!
        Indicates whether the notes field should be searched.
        The default is \c true.
     */
    bool searchNotes;

    /*!
        Indicates whether the recovery questions and answers should be searched.
        The default is \c false.
     */
    bool searchRecovery;

    /*!
        Indicates whether custom fields should be searched.
        The default is \c true.
     */
    bool searchCustomFields;

    /*!
        Indicates whether the title of the entry's parent group (if any) should be searched.
        The default is \c false.
     */
    bool searchGroupTitle;

    /*!
        Indicates whether the UUID field should be searched.
        The default is \c false.
     */
    bool searchUuid;

    /*!
        Indicates whether \a searchPattern will be treated as a regular expression.
        The default is \c false.
     */
    bool useRegex;

    /*!
        Indicates whether the search will be case-sensitive.
        The default is \c false.
     */
    bool caseSensitive;

    void setDefaults();
    Qt::CaseSensitivity caseSensitiveEnum() const;
    bool fieldsSelected() const;
    QList<QString> getDataList(const Entry &entry) const;
};

#endif // SEARCHPARAMETERS_H
