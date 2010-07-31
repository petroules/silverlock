#ifndef SILVERLOCKPREFERENCES_KEYS_H
#define SILVERLOCKPREFERENCES_KEYS_H

/*!
    \file silverlockpreferences_keys.h

    This file contains settings keys for the SilverlockPreferences class.
 */

// Main settings (loaded and saved automatically by the class)
#define KEY_LOCK_WORKSPACE_TIMEOUT_ENABLED "LockWorkspaceTimeoutEnabled"
#define KEY_LOCK_WORKSPACE_TIMEOUT "LockWorkspaceTimeout"
#define KEY_AUTO_CLEAR_CLIPBOARD_ENABLED "AutoClearClipboardEnabled"
#define KEY_AUTO_CLEAR_CLIPBOARD "AutoClearClipboard"
#define KEY_AUTO_EXIT_ENABLED "AutoExitEnabled"
#define KEY_AUTO_EXIT "AutoExit"
#define KEY_LOCK_WHEN_MINIMIZING "LockWhenMinimizing"
#define KEY_CLEAR_CLIPBOARD_WHEN_CLOSING "ClearClipboardWhenClosing"
#define KEY_MINIMIZE_TO_TRAY "MinimizeToTray"
#define KEY_MINIMIZE_AFTER_CLIPBOARD "MinimizeAfterClipboard"
#define KEY_MINIMIZE_AFTER_LOCK "MinimizeAfterLock"
#define KEY_OPEN_LAST_DATABASE "OpenLastDatabase"
#define KEY_UPDATE_ON_STARTUP "UpdateOnStartup"
#define KEY_AUTO_SAVE_ON_CLOSE "AutoSaveOnClose"
#define KEY_UPDATE_INSTALLER_PATH "UpdateInstallerPath"

// Miscellaneous settings (loaded and saved manually by the user)
#define KEY_RECENT_FILES "RecentFileList"
#define KEY_WIN_STARTUP_PATH "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define KEY_WIN_STARTUP_NAME "Silverlock"
#define KEY_MAIN_WINDOW_GEOMETRY "WindowSettings/MainWindowGeometry"
#define KEY_MAIN_WINDOW_STATE "WindowSettings/MainWindowState"
#define KEY_ENTRY_TABLE_STATE "WindowSettings/EntryTableState"

#endif // SILVERLOCKPREFERENCES_KEYS_H
