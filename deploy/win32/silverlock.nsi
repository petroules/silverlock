# Set the compressor
SetCompressor /FINAL /SOLID lzma
SetCompressorDictSize 64

;--------------------------------
; Include Modern UI

	!include "MUI2.nsh"
	!include "file-association.nsh"

;--------------------------------
; Constants

	# Set product information...
	!define PRODUCT_NAME "Silverlock"
	!define PRODUCT_UNIXNAME "silverlock"
	!define PRODUCT_VERSION "1.0"
	!define PRODUCT_VERSION_LONG "${PRODUCT_VERSION}.0.0"
	!define PRODUCT_PUBLISHER "Petroules Enterprises"
	!define PRODUCT_WEB_SITE "http://www.petroules.com/products/${PRODUCT_UNIXNAME}/"
	!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${PRODUCT_UNIXNAME}.exe"
	!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
	!define PRODUCT_UNINST_ROOT_KEY "HKLM"
	!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"
	!define COPYRIGHT_YEARS "2010"

	# Paths for files and resources
	!define QT_BIN "C:\Qt\2010.04\qt\bin"
	!define QT_PLUGINS "${QT_BIN}\..\plugins"
	!define APP_BIN "..\..\..\${PRODUCT_UNIXNAME}-build-desktop\bin"
	!define PATH_RES "..\..\res"
	!define PATH_APPRES "..\..\silverlock\res"

	# Warn the user if they try to close the installer
	!define MUI_ABORTWARNING

	# Set installer icons
	!define MUI_ICON "${PATH_APPRES}\app.ico"

;--------------------------------
; Pages and page settings

	# Multi-language settings
	!define MUI_LANGDLL_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
	!define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
	!define MUI_LANGDLL_REGISTRY_VALUENAME "NSIS:Language"

	# Insert pages
	!insertmacro MUI_PAGE_WELCOME
	!define MUI_LICENSEPAGE_CHECKBOX ; Force user to check an "I agree" box to continue installing
	!insertmacro MUI_PAGE_LICENSE $(license)
	!insertmacro MUI_PAGE_COMPONENTS
	!insertmacro MUI_PAGE_DIRECTORY
	Var StartMenuFolder ; Define variable to hold start menu folder
	!define MUI_STARTMENUPAGE_DEFAULTFOLDER "${PRODUCT_NAME}" ; Set default start menu folder
	!insertmacro MUI_PAGE_STARTMENU "${PRODUCT_NAME}" $StartMenuFolder
	!insertmacro MUI_PAGE_INSTFILES
	!define MUI_FINISHPAGE_NOAUTOCLOSE ; Don't automatically skip past the install details page when it's done
	!define MUI_FINISHPAGE_LINK "$(LANG_CaptionWebsite)" ; Finish page link text
	!define MUI_FINISHPAGE_LINK_LOCATION "${PRODUCT_WEB_SITE}" ; Finish page URL
	!insertmacro MUI_PAGE_FINISH

	!insertmacro MUI_UNPAGE_COMPONENTS
	!insertmacro MUI_UNPAGE_INSTFILES
	!define MUI_UNFINISHPAGE_NOAUTOCLOSE ; Don't automatically skip past the uninstall details page when it's done
	!insertmacro MUI_UNPAGE_FINISH

	# Define languages that the installer has
	!insertmacro MUI_LANGUAGE "English"
	!insertmacro MUI_LANGUAGE "German"

;--------------------------------
; Language strings

	# English
	LangString LANG_SecCoreFiles ${LANG_ENGLISH} "Core files required for ${PRODUCT_NAME} to operate." ; Core files section description
	LangString LANG_UnSecCoreFiles ${LANG_ENGLISH} "Removes the ${PRODUCT_NAME} core files."
	LangString LANG_UnSecSettings ${LANG_ENGLISH} "Removes the ${PRODUCT_NAME} settings and preferences files."
	LangString LANG_CaptionMainApp ${LANG_ENGLISH} "${PRODUCT_NAME}"
	LangString LANG_CaptionUninstall ${LANG_ENGLISH} "Uninstall ${PRODUCT_NAME}"
	LangString LANG_CaptionWebsite ${LANG_ENGLISH} "Visit the ${PRODUCT_NAME} website"
	LangString LANG_UninstallError ${LANG_ENGLISH} "There was a serious error uninstalling ${PRODUCT_NAME}. Please visit www.petroules.com for assistance."
	LangString LANG_Website ${LANG_ENGLISH} "Website"
	LangString LANG_Settings ${LANG_ENGLISH} "Settings"
	LicenseLangString license ${LANG_ENGLISH} "${PATH_RES}\license\gpl-3.0.txt"

;--------------------------------
; General

	# Display name and filename of installer
	Name ${PRODUCT_NAME}
	OutFile "${PRODUCT_UNIXNAME}-${PRODUCT_VERSION}-installer.exe"

	# Default installation folder
	InstallDir "$PROGRAMFILES\${PRODUCT_PUBLISHER}\${PRODUCT_NAME}"

	# Registry install key
	InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""

	# Request application privileges for Windows Vista
	RequestExecutionLevel highest

	# Do not show the NSIS message at the bottom
	BrandingText "${PRODUCT_PUBLISHER}"

	# Don't auto-close the window when install finishes
	AutoCloseWindow false

	# Show the language selection dialog
	Function .onInit
		!insertmacro MUI_LANGDLL_DISPLAY
		Exec "$INSTDIR\uninstall.exe"
	FunctionEnd

	# Make sure to show the installer in the proper language
	Function un.onInit
		!insertmacro MUI_UNGETLANGUAGE
	FunctionEnd

;--------------------------------
; Version

	# Insert version tab into the installer file (sorry, this has to stay in English... not like most will look at it anyways)
	VIProductVersion ${PRODUCT_VERSION_LONG}
	VIAddVersionKey "ProductName" "${PRODUCT_NAME}"
	VIAddVersionKey "Comments" "${PRODUCT_NAME}"
	VIAddVersionKey "CompanyName" "${PRODUCT_PUBLISHER}"
	VIAddVersionKey "LegalCopyright" "Copyright © ${COPYRIGHT_YEARS} ${PRODUCT_PUBLISHER}"
	VIAddVersionKey "FileDescription" "${PRODUCT_NAME}"
	VIAddVersionKey "FileVersion" ${PRODUCT_VERSION_LONG}

;--------------------------------
; Installer Sections

	# Core files required to install the application
	Section "${PRODUCT_NAME} ${PRODUCT_VERSION}" SecCoreFiles
	SectionIn RO
		CreateDirectory "$INSTDIR"
		SetOutPath "$INSTDIR"
		File "${APP_BIN}\silverlock.exe"
		File "${APP_BIN}\silverlocklib1.dll"
		File "${APP_BIN}\liel1.dll"
		File "${APP_BIN}\Botan.dll"
		File "${APP_BIN}\QtSolutions_SingleApplication-2.6.dll"
		File "${QT_BIN}\mingwm10.dll"
		File "${QT_BIN}\libgcc_s_dw2-1.dll"
		File "${QT_BIN}\QtCore4.dll"
		File "${QT_BIN}\QtGui4.dll"
		File "${QT_BIN}\QtNetwork4.dll"
		File "${QT_BIN}\QtSvg4.dll"
		File "${QT_BIN}\QtXml4.dll"
		File "D:\Development\Web\PetroulesEnterprisesWebsite\PetroulesEnterprisesWebsite\favicon.ico"
		CreateDirectory "$INSTDIR\iconengines\"
		SetOutPath "$INSTDIR\iconengines"
		File "${QT_PLUGINS}\iconengines\qsvgicon4.dll"
		CreateDirectory "$INSTDIR\imageformats\"
		SetOutPath "$INSTDIR\imageformats"
		File "${QT_PLUGINS}\imageformats\qico4.dll"
		File "${QT_PLUGINS}\imageformats\qsvg4.dll"

		# Create start menu shortcuts only if the user selected them
		!insertmacro MUI_STARTMENU_WRITE_BEGIN "${PRODUCT_NAME}"
		CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\$(LANG_CaptionMainApp).lnk" "$INSTDIR\silverlock.exe" "" "$INSTDIR\silverlock.exe" 0 SW_SHOWNORMAL "" $(LANG_CaptionMainApp)
		!insertmacro MUI_STARTMENU_WRITE_END

		# Create file associations
		${registerExtension} "$INSTDIR\silverlock.exe" ".sdbx" "Silverlock Database"
	SectionEnd

	# Actions performed after the installation has been completed
	Section -Post
		WriteUninstaller "$INSTDIR\uninstall.exe" ; Create the uninstaller program
		WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\silverlock.exe"
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninstall.exe"
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\silverlock.exe"
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "StartMenuFolder" "$StartMenuFolder"
		SectionGetSize ${SecCoreFiles} $0
		WriteRegDWORD ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "EstimatedSize" $0
	SectionEnd

;--------------------------------
; Uninstaller Section

	# Delete empty directories recursively
	var deleteDir
	var dirLength
	Function un.PathDeleteEmptyDirRecurse
	ClearErrors
	loop:
		Sleep 50 ; Without a small delay here, the directory sometimes won't get removed
		RMDir "$deleteDir" ; Remove the directory
		IfErrors end
		strlen $dirLength $deleteDir ; Store the length of the path
		intcmp $dirLength 3 end end ; If the length of the path is <= 3 (e.g. C:\), we're at the root drive
		GetFullPathName $deleteDir "$deleteDir\.." ; <path>\.. results in the parent directory of <path>
		IfErrors end loop
	end:
	FunctionEnd

	# Uninstall path safety check - this makes sure the path isn't any of the following...
	# otherwise the user may get a blank hard drive due to an improper registry key.
	!macro BadPathsCheck
	StrCpy $R0 $INSTDIR "" -2
	StrCmp $R0 ":\" bad
	StrCpy $R0 $INSTDIR "" -14
	StrCmp $R0 "\Program Files" bad
	StrCpy $R0 $INSTDIR "" -8
	StrCmp $R0 "\Windows" bad
	StrCpy $R0 $INSTDIR "" -6
	StrCmp $R0 "\WinNT" bad
	StrCpy $R0 $INSTDIR "" -9
	StrCmp $R0 "\system32" bad
	StrCpy $R0 $INSTDIR "" -8
	StrCmp $R0 "\Desktop" bad
	StrCpy $R0 $INSTDIR "" -23
	StrCmp $R0 "\Documents and Settings" bad
	StrCpy $R0 $INSTDIR "" -13
	StrCmp $R0 "\My Documents" bad done
	bad:
		MessageBox MB_OK|MB_ICONSTOP $(LANG_UninstallError)
		Abort
	done:
	!macroend

	# Macro to remove uninstaller only IF all components are gone
	!macro un.RemoveUninstaller
		ReadRegStr $StartMenuFolder ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "StartMenuFolder"

		; The way this works, +1 means simply go to the next instruction.
		; So unless the files DO exist (in which case it jumps to LeaveUninstaller and does nothing),
		; it continues to next instruction - doing exactly what we want.
		IfFileExists "$INSTDIR\silverlock.exe" LeaveUninstaller +1
			Delete "$INSTDIR\uninstall.exe"
			StrCpy $deleteDir "$INSTDIR"
			call un.PathDeleteEmptyDirRecurse
			StrCpy $deleteDir "$SMPROGRAMS\$StartMenuFolder"
			call un.PathDeleteEmptyDirRecurse
			DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
			DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
		LeaveUninstaller:
	!macroend

	# Uninstalls the core files
	Section "un.${PRODUCT_NAME} ${PRODUCT_VERSION}" UnSecCoreFiles
		!insertmacro BadPathsCheck ; Make sure the uninstall path isn't dangerous
		ReadRegStr $StartMenuFolder ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "StartMenuFolder"

		Delete "$INSTDIR\silverlock.exe"
		Delete "$INSTDIR\silverlocklib1.dll"
		Delete "$INSTDIR\liel1.dll"
		Delete "$INSTDIR\Botan.dll"
		Delete "$INSTDIR\QtSolutions_SingleApplication-2.6.dll"
		Delete "$INSTDIR\mingwm10.dll"
		Delete "$INSTDIR\libgcc_s_dw2-1.dll"
		Delete "$INSTDIR\QtCore4.dll"
		Delete "$INSTDIR\QtGui4.dll"
		Delete "$INSTDIR\QtNetwork4.dll"
		Delete "$INSTDIR\QtSvg4.dll"
		Delete "$INSTDIR\QtXml4.dll"
		Delete "$INSTDIR\favicon.ico"
		# Not removing INSTDIR here because the finalizer does that
		Delete "$INSTDIR\iconengines\qsvgicon4.dll"
		RMDir "$INSTDIR\iconengines"
		Delete "$INSTDIR\imageformats\qico4.dll"
		Delete "$INSTDIR\imageformats\qsvg4.dll"
		RMDir "$INSTDIR\imageformats"

		Delete "$SMPROGRAMS\$StartMenuFolder\$(LANG_CaptionMainApp).lnk"
		# Not removing StartMenuFolder here because the finalizer does that

		# Remove file association
		${unregisterExtension} ".sdbx" "Silverlock Database"

		!insertmacro un.RemoveUninstaller ; Remove the uninstaller if it should be removed
	SectionEnd

	# Optionally uninstalls user settings and preferences
	Section /o "un.${PRODUCT_NAME} $(LANG_Settings)" UnSecSettings ; We have /o here to uncheck this by default
		DeleteRegKey HKCU "Software\${PRODUCT_PUBLISHER}\${PRODUCT_NAME}"
		!insertmacro un.RemoveUninstaller ; Remove the uninstaller if it should be removed
	SectionEnd

;--------------------------------
; Descriptions

	# Assign language strings to installer sections
	!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SecCoreFiles} $(LANG_SecCoreFiles)
	!insertmacro MUI_FUNCTION_DESCRIPTION_END

	# Assign language strings to uninstaller sections
	!insertmacro MUI_UNFUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${UnSecCoreFiles} $(LANG_UnSecCoreFiles)
	!insertmacro MUI_DESCRIPTION_TEXT ${UnSecSettings} $(LANG_UnSecSettings)
	!insertmacro MUI_UNFUNCTION_DESCRIPTION_END