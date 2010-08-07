To build Silverlock, you must first download LIEL (from Petroules Enterprises) and two additional 3rd party libraries .

These can be downloaded from:

http://www.petroules.com/products/liel/
http://download.petroules.com/3rdparty/botan.zip
http://download.petroules.com/3rdparty/qtsingleapplication.zip

Create a folder called 3rdparty in the root of your development folder, and unzip the botan and qtsingleapplication archives in that folder. Unzip LIEL in the root of your development folder. For example, your folder structure should now look like this:

3rdparty
	botan
	qtsingleapplication
liel
silverlock

Navigate to the qtsingleapplication folder and open a terminal/command prompt. Enter:

chmod +x configure (Windows users ignore this line)
configure -library

You will be prompted to accept qtsingleapplication's license (GPL or LGPL). Follow the on-screen instructions - note that if you do not accept the terms of qtsingleapplication's license agreement you cannot continue beyond this point to build Silverlock.

Build botan and qtsingleapplication, then build LIEL, then Silverlock.