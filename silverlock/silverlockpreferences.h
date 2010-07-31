#ifndef SILVERLOCKSETTINGS_H
#define SILVERLOCKSETTINGS_H

#include <QtCore>

class MainWindow;

class SilverlockPreferences
{
public:
    static SilverlockPreferences& instance();
    static const SilverlockPreferences& defaults();
    static void destroy();
    void load();
    void save();
    void restoreDefaults();
    bool lockWorkspaceTimeoutEnabled() const;
    void setLockWorkspaceTimeoutEnabled(bool lock);
    int lockWorkspaceTimeout() const;
    void setLockWorkspaceTimeout(int seconds);
    bool autoClearClipboardEnabled() const;
    void setAutoClearClipboardEnabled(bool clear);
    int autoClearClipboard() const;
    void setAutoClearClipboard(int seconds);
    bool autoExitEnabled() const;
    void setAutoExitEnabled(bool exit);
    int autoExit() const;
    void setAutoExit(int seconds);
    bool lockWhenMinimizing() const;
    void setLockWhenMinimizing(bool lock);
    bool clearClipboardWhenClosing() const;
    void setClearClipboardWhenClosing(bool clear);
    bool minimizeToTray() const;
    void setMinimizeToTray(bool minimize);
    bool minimizeAfterClipboard() const;
    void setMinimizeAfterClipboard(bool minimize);
    bool minimizeAfterLock() const;
    void setMinimizeAfterLock(bool minimize);
    bool runAtStartupSupported() const;
    bool runAtStartup() const;
    void setRunAtStartup(bool run);
    bool isFileAssociationSet() const;
    void setFileAssociationActive(bool active);
    bool openLastDatabase() const;
    void setOpenLastDatabase(bool open);
    bool updateOnStartup() const;
    void setUpdateOnStartup(bool update);
    bool autoSaveOnClose() const;
    void setAutoSaveOnClose(bool save);
    const QStringList recentFileList() const;
    void clearRecentFiles();
    void pushRecentFile(const QString &fileName);
    int maxRecentFiles() const;
    void saveWindowSettings(MainWindow *mainWindow);
    void restoreWindowSettings(MainWindow *mainWindow);
    void clearWindowSettings();
    QString updateInstallerPath() const;
    void setUpdateInstallerPath(const QString &path);

private:
    SilverlockPreferences();
    static SilverlockPreferences *m_instance;
    static SilverlockPreferences *m_defaults;
    bool m_lockWorkspaceTimeoutEnabled;
    int m_lockWorkspaceTimeout;
    bool m_autoClearClipboardEnabled;
    int m_autoClearClipboard;
    bool m_autoExitEnabled;
    int m_autoExit;
    bool m_lockWhenMinimizing;
    bool m_clearClipboardWhenClosing;
    bool m_minimizeToTray;
    bool m_minimizeAfterClipboard;
    bool m_minimizeAfterLock;
    bool m_openLastDatabase;
    bool m_updateOnStartup;
    bool m_autoSaveOnClose;
    QStringList m_recentFiles;
    bool m_windowSettingsCleared;
    QString m_updateInstallerPath;

#ifdef Q_OS_WIN
    QString applicationPathForRegistry() const;
#endif
};

#endif // SILVERLOCKSETTINGS_H
