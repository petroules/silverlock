#ifndef DATABASEPRINTERFIELDS_H
#define DATABASEPRINTERFIELDS_H

struct DatabasePrinterFields
{
public:
    DatabasePrinterFields();

    /*!
        Indicates whether the title field will be included in the printout.
        The default is \c true.
     */
    bool title;

    /*!
        Indicates whether the username field will be included in the printout.
        The default is \c true.
     */
    bool username;

    /*!
        Indicates whether the password field will be included in the printout.
        The default is \c true.
     */
    bool password;

    /*!
        Indicates whether the URL field will be included in the printout.
        The default is \c true.
     */
    bool url;

    /*!
        Indicates whether the notes field will be included in the printout.
        The default is \c false.
     */
    bool notes;

    /*!
        Indicates whether the recovery questions and answers will be included in the printout.
        The default is \c true.
     */
    bool recoveryInfo;

    /*!
        Indicates whether the custom fields will be included in the printout.
        The default is \c true.
     */
    bool customFields;

    /*!
        Indicates whether the UUID field will be included in the printout.
        The default is \c false.
     */
    bool uuid;

    /*!
        Indicates whether the entry creation time will be included in the printout.
        The default is \c false.
     */
    bool creationTime;

    /*!
        Indicates whether the entry access time will be included in the printout.
        The default is \c false.
     */
    bool accessTime;

    /*!
        Indicates whether the entry modification time will be included in the printout.
        The default is \c false.
     */
    bool modificationTime;

    void setDefaults();
};

#endif // DATABASEPRINTERFIELDS_H
