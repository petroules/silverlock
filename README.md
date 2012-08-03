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

Copyright (c) 2010-2012 Petroules Corporation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

### Third party

Licenses for all third party components can be found in the `res/license` folder.

### [Qt by Nokia](http://qt-project.org)

See [res/lgpl-2.1.txt](res/lgpl-2.1.txt) and [res/qt-exception.txt](res/qt-exception.txt).

### [Qt Solutions by Nokia](http://qt.gitorious.org/qt-solutions)

According to [Nokia](https://qt.nokia.com/products/qt-addons/solutions-archive/), the Qt Solutions are licensed under "the BSD license". A specific copy is not provided. We will assume this to mean the original 4-clause BSD license.

See [res/qsa.txt](res/qsa.txt).

### [Botan](http://botan.randombit.net)

See [res/botan.txt](res/botan.txt).

### [create-dmg by YourSway](http://www.yoursway.com/free/#createdmg)

*Used as part of the Silverlock build process*

According to [YourSway's website](http://www.yoursway.com/free/#createdmg), the tool is "licensed under GNU General Public License, because it incorporates portions of GPL-licensed code from Adium repository". The version is not specified, but private correspondence with its author states "GPLv2 or any later version at your discretion".

See [res/gpl-3.0.txt](res/gpl-3.0.txt).
