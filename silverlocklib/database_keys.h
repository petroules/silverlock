#ifndef DATABASE_KEYS_H
#define DATABASE_KEYS_H

/*!
    \file database_keys.h

    This file contains XML tag and attribute names for the database file format used by the
    DatabaseReader, DatabaseWriter, DatabaseNode and subclasses.
 */

#define XML_DOCTYPE "SilverlockDatabase"
#define XML_DATABASE "Database"
#define XML_VERSION "Version"
#define XML_DBPASSWORD "Password"

#define XML_UUID "UUID"
#define XML_TITLE "Title"
#define XML_CREATED "Created"
#define XML_ACCESSED "Accessed"
#define XML_MODIFIED "Modified"

#define XML_GROUP "Group"
#define XML_ENTRY "Entry"

#define XML_URL "URL"
#define XML_USERNAME "Username"
#define XML_PASSWORD "Password"
#define XML_NOTES "Notes"

#define XML_RECOVERYINFO "RecoveryInfo"
#define XML_QUESTION "Question"
#define XML_ANSWER "Answer"

#define XML_CUSTOMFIELD "CustomField"
#define XML_CFNAME "Name"
#define XML_CFVALUE "Value"

#endif // DATABASE_KEYS_H
