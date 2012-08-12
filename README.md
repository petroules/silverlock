[Silverlock](http://www.petroules.com/products/silverlock) [![Build Status](https://secure.travis-ci.org/petroules/silverlock.png)](http://travis-ci.org/petroules/silverlock)
============

Silverlock is a secure password management application designed to ease the management of account credentials and related information. It allows you to store your usernames, passwords, and other information related to your online accounts and authentications in a single file encrypted with a master password. Without the master password, no one else can access your information. The application is designed with simplicity and ease of use in mind.

Quick start
-----------

Clone the repo, `git clone git@github.com:petroules/silverlock.git`, or [download the latest release](http://www.petroules.com/products/silverlock).

Bug tracker
-----------

If you've found a bug, please create an issue here on GitHub!

https://github.com/petroules/silverlock/issues

Developers
----------

Requirements:

* A compiler:
   * [GCC](http://gcc.gnu.org) 4.5+ (for Unix) OR
	* [MinGW](http://www.mingw.org) 4.5+ with [MSYS](http://www.mingw.org/wiki/MSYS) (for Windows) OR
	* [Microsoft Visual C++ 2008/2010](http://www.microsoft.com/visualstudio) (for Windows)
	* Other (unsupported)
* [Python](http://python.org) 2.6 or 2.7
* [7-Zip](http://www.7-zip.org)
* [Qt](http://qt-project.org/) 4.8+ with Qt Creator (installing the [Qt SDK](https://qt.nokia.com/products/qt-sdk) will provide both)
* OpenSSL 1.0+ binaries for Windows from [Shining Light Productions](http://www.slproweb.com/products/Win32OpenSSL.html)

### Building for Desktop (Windows, Mac OS X, Linux)

Open `src/silverlock.pro` in Qt Creator and click build.

Authors
-------

**Jake Petroules**

+ http://twitter.com/jakepetroules
+ http://github.com/jakepetroules

Copyright and license
---------------------

See the COPYING file.
