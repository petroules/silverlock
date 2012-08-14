#include "databaseprinterfields.h"

/*!
    \struct DatabasePrinterFields

    The DatabasePrinterFields structure provides a set of flags that can be set
    to determine whether the corresponding fields will be included in the printout
    version of the database.
 */

/*!
    Constructs a new DatabasePrinterFields.
 */
DatabasePrinterFields::DatabasePrinterFields()
{
    this->setDefaults();
}

/*!
    Sets all fields to their default values.
 */
void DatabasePrinterFields::setDefaults()
{
    this->title = true;
    this->username = true;
    this->password = true;
    this->url = true;
    this->notes = false;
    this->recoveryInfo = true;
    this->customFields = true;
    this->uuid = false;
    this->creationTime = false;
    this->accessTime = false;
    this->modificationTime = false;
}
