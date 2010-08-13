################################
# Licensing                    #
################################

Silverlock is licensed under the GPL 3.0. Silverlock is also made possible by various other software:

* LIEL - Language Independent Extension Library (by Petroules Enterprises)
	Licensed under the standard LGPL 2.1
* Qt 4.6 (by Nokia)
	Licensed under the standard LGPL 2.1 with an exception
* A modified version of QtSingleApplication 2.6 (by Nokia)
	Licensed under the standard LGPL 2.1 with an exception
* A modified version of a modified version (by Nokia) of Botan 1.8.8
	Licensed under the standard LGPL 2.1 with an exception (originally licensed under a FreeBSD-style license)
* create-dmg 1.0.0.2 (by YourSway)
	Licensed under the GPL
	Note that this tool is only used as part of our build process, it is not *incorporated* into our software and thus does not require we license Silverlock under the GPL
	Home page at: http://www.yoursway.com/free/#createdmg

Licenses for all of these products can be found in res/license.

################################
# Build Instructions           #
################################

Prerequisites:
* Qt 4.6
* Qt Creator 2.0

To build Silverlock, you must first download various required libraries and tools (mentioned above in the license section).

http://www.petroules.com/products/liel/
http://download.petroules.com/3rdparty/botan.zip
http://download.petroules.com/3rdparty/qtsingleapplication.zip
http://download.petroules.com/3rdparty/create-dmg.zip

Create a folder called 3rdparty in the root of your development folder, and extract the botan and qtsingleapplication archives to that folder. Unzip LIEL in the root of your development folder. For example, your folder structure should now look like this:

3rdparty
	botan
	qtsingleapplication
liel
silverlock

Navigate to the qtsingleapplication folder and open a terminal/command prompt. Enter:

(For Unix)

chmod +x configure
./configure -library

(For Windows)

configure.bat -library

You will be prompted to accept qtsingleapplication's license (you should choose LGPL). Follow the on-screen instructions - note that if you do not accept the terms of qtsingleapplication's license agreement you cannot continue beyond this point to build Silverlock.

Using Qt Creator, build botan and qtsingleapplication, then build LIEL, then Silverlock.