#ifndef VERSION_H
#define VERSION_H

// These define the file and product versions; each in two different formats;
// one a normal string and one for the Windows RC file
#define VER_FILEVERSION             1,0,0,0
#define VER_FILEVERSION_STR         "1.0.0.0\0"
#define VER_PRODUCTVERSION          1,0,0,0
#define VER_PRODUCTVERSION_STR      "1.0\0"

// Various application metadata; company, name, copyright, etc.
#define VER_COMPANYNAME_STR         "Petroules Enterprises"
#define VER_FILEDESCRIPTION_STR     "Silverlock"
#define VER_INTERNALNAME_STR        "Silverlock"
#define VER_LEGALCOPYRIGHT_STR      "Copyright © 2008-2010 Petroules Enterprises"
#define VER_LEGALTRADEMARKS1_STR    "All Rights Reserved"
#define VER_LEGALTRADEMARKS2_STR    VER_LEGALTRADEMARKS1_STR
#define VER_ORIGINALFILENAME_STR    "silverlock.exe"
#define VER_PRODUCTNAME_STR         "Silverlock"

// Network and URLs
#define VER_COMPANYDOMAIN_STR       "petroules.com"

// Mac OS X identifier
#define MACOSX_PROGID               "com.petroules.Silverlock"

#endif // VERSION_H
