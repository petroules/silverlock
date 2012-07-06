### Version 1.1.2
March 17, 2011

* Updated copyright texts to read Petroules Corporation.
* Fixed a bug that prevented Silverlock from starting on Windows if the system was not connected to the Internet.

### Version 1.1.1
January 11, 2011

* Changed Windows installer to use correct merge module architecture (x86) instead of the previous Itanium.
* Improved update check error reporting and added a dialog allowing the user to ignore SSL errors if present.
* Added a launcher app for Windows which performs certificate validation for the update server's certificate to prevent a major problem with SSL when the user doesn't have the root certificate in their store.

### Version 1.1
January 8, 2011

* Fixed an issue with the automatic updater failing to connect if the system did not have OpenSSL installed.
* Improved automatic clipboard clearing.
* Fixed a couple minor interface/security-related issues.
* Fixed a bug causing the compression tab to be left enabled on groups.
* Updated license information for dependencies.
* Several other bug fixes and code improvements and tweaks.
* Silverlock for Windows now uses the native Windows Installer service.
* Added a new welcome screen.
* Changed the group browser and information view windows to dock widgets, allowing greater customization of the user interface layout.
* Made the toolbar buttons larger and added some new ones.
* Added a new create database wizard.
* Silverlock for Windows is now built using Microsoft Visual C++ instead of MinGW - this means it *might* run a little faster.
* Silverlock for Mac OS X is now built using 64-bit Cocoa instead of 32-bit Carbon - this means we only support the new 64-bit Intel Macs running Mac OS X 10.5 "Leopard" or later.
* Botan upgraded to latest development version.
* Botan version now shown on license dialog.
* Botan and QtSingleApplication are now statically linked.
* Completely restructured the files and build setup.
* Moved check for updates menu item to application menu.

### Version 1.0
August 14, 2010

* First public release for Windows, OS X and Linux (32-bit)