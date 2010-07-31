#ifndef ENTRYVIEWINDEXES_H
#define ENTRYVIEWINDEXES_H

/*!
    \file entryviewindexes.h

    This file contains constants for indexes related to the entry view.
 */

// Column indexes
#define COLUMN_TITLE    0
#define COLUMN_URL      1
#define COLUMN_USERNAME 2
#define COLUMN_PASSWORD 3
#define COLUMN_NOTES    4
#define COLUMN_UUID     5

// Not a column, but specifies the number of columns
#define COLUMN_COUNT    6

// Corresponds to Qt::UserRole, for storing the actual text of a cell in
// the entry view so that it can be restored when an item is unobscured
#define DATA_REALTEXT   32

// The text used to obscure a column's contents
#define OBSCURETEXT "********"

#endif // ENTRYVIEWINDEXES_H
