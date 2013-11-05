#ifndef CONFIG_H
#define CONFIG_H

#define PROJECT_ID "com.petroules.silverlock"
#define PROJECT_UUID "{CA3E50C3-460E-4E12-9EFB-34030DC15306}"

#define PROJECT_SUMMARY "Silverlock password manager"
#define PROJECT_DESCRIPTION "Silverlock is a secure password management application designed to ease the management of account credentials and related information. It allows you to store your usernames, passwords, and other information related to your online accounts and authentications in a single file encrypted with a master password. Without the master password, no one else can access your information. The application is designed with simplicity and ease of use in mind."

#define SILVERLOCK_VERSION_MAJOR 1
#define SILVERLOCK_VERSION_MINOR 1
#define SILVERLOCK_VERSION_PATCH 2
#define SILVERLOCK_VERSION_BUILD 0
#define SILVERLOCK_VERSION_SHORT "1.1.2"
#define SILVERLOCK_VERSION_LONG "1.1.2.0"
#define SILVERLOCK_VERSION_CSV 1,1,2,0

#define SILVERLOCK_VENDOR "Petroules"
#define SILVERLOCK_CONTACT "info@petroules.com"
#define SILVERLOCK_COPYRIGHT "Copyright (c) 2010-2012 Petroules Corporation"
#define SILVERLOCK_WEBSITE "http://www.petroules.com/"
#define SILVERLOCK_DOMAIN "petroules.com"

// Defines related to Windows .rc files
#define VER_FILEDESCRIPTION_STR     PROJECT_SUMMARY
#define VER_INTERNALNAME_STR        APP_DISPLAYNAME
#define VER_PRODUCTNAME_STR         APP_DISPLAYNAME
#define VER_COMPANYNAME_STR         SILVERLOCK_VENDOR
#define VER_LEGALCOPYRIGHT_STR      SILVERLOCK_COPYRIGHT // backslash xA9 for real (c) symbol...
#define VER_LEGALTRADEMARKS1_STR    "All Rights Reserved"
#define VER_LEGALTRADEMARKS2_STR    VER_LEGALTRADEMARKS1_STR
#define VER_COMPANYDOMAIN_STR       SILVERLOCK_DOMAIN

#endif // CONFIG_H
