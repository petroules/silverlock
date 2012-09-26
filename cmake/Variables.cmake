set(PROJECT_ID              "com.petroules.silverlock")
set(PROJECT_UUID            "{CA3E50C3-460E-4E12-9EFB-34030DC15306}")

set(PROJECT_SUMMARY         "Silverlock password manager")
set(PROJECT_DESCRIPTION     "Silverlock is a secure password management application designed to ease the management of account credentials and related information. It allows you to store your usernames, passwords, and other information related to your online accounts and authentications in a single file encrypted with a master password. Without the master password, no one else can access your information. The application is designed with simplicity and ease of use in mind.")

set(SILVERLOCK_PACKAGE_NAME "Silverlock")
set(SILVERLOCK_PACKAGE_PREFIX "silverlock")

set(SILVERLOCK_VERSION_MAJOR 1)
set(SILVERLOCK_VERSION_MINOR 1)
set(SILVERLOCK_VERSION_PATCH 2)
set(SILVERLOCK_VERSION_BUILD 0)
version_string(SILVERLOCK_VERSION_SHORT SILVERLOCK_VERSION_LONG ${SILVERLOCK_VERSION_MAJOR} ${SILVERLOCK_VERSION_MINOR} ${SILVERLOCK_VERSION_PATCH} ${SILVERLOCK_VERSION_BUILD})

git_append_shorttag(SILVERLOCK_VERSION_SHORT)

set(SILVERLOCK_VENDOR "Petroules")
set(SILVERLOCK_CONTACT "info@petroules.com")
set(SILVERLOCK_COPYRIGHT "Copyright (c) 2010-2012 Petroules Corporation")
set(SILVERLOCK_WEBSITE "http://www.petroules.com/")
set(SILVERLOCK_DOMAIN "petroules.com")

set(PROJECT_README          "README.md")
set(PROJECT_LICENSE         "COPYING.md")
