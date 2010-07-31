#ifndef SEARCHPARAMETERS_H
#define SEARCHPARAMETERS_H

#include <QtCore>
#include "entry.h"
#include "silverlocklib_global.h"

struct SILVERLOCKLIBSHARED_EXPORT SearchParameters : public QObject
{
    Q_OBJECT

public:
    explicit SearchParameters(QObject *parent = NULL);

    /*!
        The search pattern to use when finding entries in a password database.
        This string will be treated as a regular expression if \a useRegex
        is \c true, and will be treated as a normal string if \a useRegex is \c false.
     */
    QString searchPattern;

    /*!
        Indicates whether the title field should be searched.
     */
    bool searchTitle;
    bool searchUrl;
    bool searchUsername;
    bool searchPassword;
    bool searchNotes;
    bool searchRecovery;
    bool searchCustomFields;
    bool searchGroupTitle;
    bool searchUuid;

    /*!
        Indicates whether \a searchPattern will be treated as a regular expression.
     */
    bool useRegex;

    /*!
        Indicates whether the search will be case-sensitive.
     */
    bool caseSensitive;

    void setDefaults();
    Qt::CaseSensitivity caseSensitiveEnum() const;
    bool fieldsSelected() const;
    QList<QString> getDataList(const Entry &entry) const;
};

#endif // SEARCHPARAMETERS_H
