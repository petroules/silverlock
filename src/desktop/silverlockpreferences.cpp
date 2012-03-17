#include "silverlockpreferences.h"
#include "silverlockpreferences_keys.h"
#include "mainwindow.h"
#ifdef Q_WS_MAC
#include "mac/macloginitemsmanager.h"
#endif

SilverlockPreferences* SilverlockPreferences::m_instance = NULL;
SilverlockPreferences* SilverlockPreferences::m_defaults = NULL;

/*!
    \class SilverlockPreferences

    The SilverlockPreferences class encapsulates all settings and preferences for the application
    and provides methods to persist them.

    Settings are stored and retrieved using the standard QSettings class. When the class is
    constructed, any existing settings are loaded; any non-existing settings will be set to
    their default values.

    Only one instance of this class will be created - the constructor is private. Use \a instance()
    to retrieve a reference to the instance (which will be lazily initialized if the instance does
    not yet exist) and call \a destroy() when the application exits. \a destroy() will save the
    preferences to persistent storage, also use the \a save() method for that.

    Default settings can be persisted by calling \link restoreDefaults() \endlink and
    \link save() \endlink in succession.
 */

/*!
    Constructs a new SilverlockPreferences.
 */
SilverlockPreferences::SilverlockPreferences()
    : m_windowSettingsCleared(false)
{
    this->load();
}

/*!
    Gets a reference to the SilverlockPreferences instance, constructing one if it has not yet been
    created.
 */
SilverlockPreferences& SilverlockPreferences::instance()
{
    if (!SilverlockPreferences::m_instance)
    {
        SilverlockPreferences::m_instance = new SilverlockPreferences();
    }

    return *SilverlockPreferences::m_instance;
}

/*!
    Returns an immutable instance of SilverlockPreferences with all values set to default.
 */
const SilverlockPreferences& SilverlockPreferences::defaults()
{
    if (!SilverlockPreferences::m_defaults)
    {
        SilverlockPreferences::m_defaults = new SilverlockPreferences();

        // Set the default values; now once we return it, it's locked into this state
        SilverlockPreferences::m_defaults->restoreDefaults();
    }

    return *SilverlockPreferences::m_defaults;
}

/*!
    Destroys the preferences instance, first saving all the settings.
 */
void SilverlockPreferences::destroy()
{
    if (SilverlockPreferences::m_instance)
    {
        SilverlockPreferences::m_instance->save();

        delete SilverlockPreferences::m_instance;
        SilverlockPreferences::m_instance = NULL;
    }

    if (SilverlockPreferences::m_defaults)
    {
        delete SilverlockPreferences::m_defaults;
        SilverlockPreferences::m_defaults = NULL;
    }
}

/*!
    Loads all preferences from persistent storage.
 */
void SilverlockPreferences::load()
{
    // Restoring the defaults allows us to determine the default values when
    // reading settings without duplicating them - all default values should
    // appear in the restoreDefaults() method; nowhere else
    this->restoreDefaults();

    QSettings settings;
    this->setLockWorkspaceTimeoutEnabled(settings.value(KEY_LOCK_WORKSPACE_TIMEOUT_ENABLED, this->m_lockWorkspaceTimeoutEnabled).toBool());
    this->setLockWorkspaceTimeout(settings.value(KEY_LOCK_WORKSPACE_TIMEOUT, this->m_lockWorkspaceTimeout).toInt());
    this->setAutoClearClipboardEnabled(settings.value(KEY_AUTO_CLEAR_CLIPBOARD_ENABLED, this->m_autoClearClipboardEnabled).toBool());
    this->setAutoClearClipboard(settings.value(KEY_AUTO_CLEAR_CLIPBOARD, this->m_autoClearClipboard).toInt());
    this->setAutoExitEnabled(settings.value(KEY_AUTO_EXIT_ENABLED, this->m_autoExitEnabled).toBool());
    this->setAutoExit(settings.value(KEY_AUTO_EXIT, this->m_autoExit).toInt());
    this->setLockWhenMinimizing(settings.value(KEY_LOCK_WHEN_MINIMIZING, this->m_lockWhenMinimizing).toBool());
    this->setClearClipboardWhenClosing(settings.value(KEY_CLEAR_CLIPBOARD_WHEN_CLOSING, this->m_clearClipboardWhenClosing).toBool());
    this->setMinimizeToTray(settings.value(KEY_MINIMIZE_TO_TRAY, this->m_minimizeToTray).toBool());
    this->setMinimizeAfterClipboard(settings.value(KEY_MINIMIZE_AFTER_CLIPBOARD, this->m_minimizeAfterClipboard).toBool());
    this->setMinimizeAfterLock(settings.value(KEY_MINIMIZE_AFTER_LOCK, this->m_minimizeAfterLock).toBool());
    this->setOpenLastDatabase(settings.value(KEY_OPEN_LAST_DATABASE, this->m_openLastDatabase).toBool());
    this->setUpdateOnStartup(settings.value(KEY_UPDATE_ON_STARTUP, this->m_updateOnStartup).toBool());
    this->setAutoSaveOnClose(settings.value(KEY_AUTO_SAVE_ON_CLOSE, this->m_autoSaveOnClose).toBool());

    // Iterate backwards since our method prepends to the beginning of the list
    QStringList list = settings.value(KEY_RECENT_FILES, QStringList()).toStringList();
    for (int i = qMin(list.count(), this->maxRecentFiles()) - 1; i >= 0; i--)
    {
        this->addRecentFile(list.at(i));
    }

    this->setUpdateInstallerPath(settings.value(KEY_UPDATE_INSTALLER_PATH, this->m_updateInstallerPath).toString());
}

/*!
    Saves all preferences to persistent storage.

    \note Regarding implementation: this method does not modify the class but is not marked const as
    this would allow the instance returned by \a defaults() to persist the settings, which should
    not be allowed.
 */
void SilverlockPreferences::save()
{
    QSettings settings;
    settings.setValue(KEY_LOCK_WORKSPACE_TIMEOUT_ENABLED, this->m_lockWorkspaceTimeoutEnabled);
    settings.setValue(KEY_LOCK_WORKSPACE_TIMEOUT, this->m_lockWorkspaceTimeout);
    settings.setValue(KEY_AUTO_CLEAR_CLIPBOARD_ENABLED, this->m_autoClearClipboardEnabled);
    settings.setValue(KEY_AUTO_CLEAR_CLIPBOARD, this->m_autoClearClipboard);
    settings.setValue(KEY_AUTO_EXIT_ENABLED, this->m_autoExitEnabled);
    settings.setValue(KEY_AUTO_EXIT, this->m_autoExit);
    settings.setValue(KEY_LOCK_WHEN_MINIMIZING, this->m_lockWhenMinimizing);
    settings.setValue(KEY_CLEAR_CLIPBOARD_WHEN_CLOSING, this->m_clearClipboardWhenClosing);
    settings.setValue(KEY_MINIMIZE_TO_TRAY, this->m_minimizeToTray);
    settings.setValue(KEY_MINIMIZE_AFTER_CLIPBOARD, this->m_minimizeAfterClipboard);
    settings.setValue(KEY_MINIMIZE_AFTER_LOCK, this->m_minimizeAfterLock);
    settings.setValue(KEY_OPEN_LAST_DATABASE, this->m_openLastDatabase);
    settings.setValue(KEY_UPDATE_ON_STARTUP, this->m_updateOnStartup);
    settings.setValue(KEY_AUTO_SAVE_ON_CLOSE, this->m_autoSaveOnClose);
    settings.setValue(KEY_RECENT_FILES, this->m_recentFiles);
    settings.setValue(KEY_UPDATE_INSTALLER_PATH, this->m_updateInstallerPath);
}

/*!
    Restores all preferences to their original values.

    \note The recent file list will NOT be affected by this method; that can be cleared with the
    \a clearRecentFiles() method.

    \note File associations will NOT be affected by this method; those settings can be adjusted by
    \a isFileAssociationSet and \a setFileAssociationActive.

    \note Run at startup/login will NOT be affected by this method; that can be adjusted with the
    \a setRunAtStartup method.
 */
void SilverlockPreferences::restoreDefaults()
{
    this->setLockWorkspaceTimeoutEnabled(false);
    this->setLockWorkspaceTimeout(300);
    this->setAutoClearClipboardEnabled(true);
    this->setAutoClearClipboard(15);
    this->setAutoExitEnabled(false);
    this->setAutoExit(900);
    this->setLockWhenMinimizing(false);
    this->setClearClipboardWhenClosing(true);
    this->setMinimizeToTray(false);
    this->setMinimizeAfterClipboard(false);
    this->setMinimizeAfterLock(false);
    this->setOpenLastDatabase(false);
    this->setUpdateOnStartup(true);
    this->setAutoSaveOnClose(false);
    this->setUpdateInstallerPath(QString());
}

bool SilverlockPreferences::lockWorkspaceTimeoutEnabled() const
{
    return this->m_lockWorkspaceTimeoutEnabled;
}

void SilverlockPreferences::setLockWorkspaceTimeoutEnabled(bool lock)
{
    this->m_lockWorkspaceTimeoutEnabled = lock;
}

int SilverlockPreferences::lockWorkspaceTimeout() const
{
    return this->m_lockWorkspaceTimeout;
}

void SilverlockPreferences::setLockWorkspaceTimeout(int seconds)
{
    if (seconds < 1 || seconds > 31536000)
    {
        return;
    }

    this->m_lockWorkspaceTimeout = seconds;
}

bool SilverlockPreferences::autoClearClipboardEnabled() const
{
    return this->m_autoClearClipboardEnabled;
}

void SilverlockPreferences::setAutoClearClipboardEnabled(bool clear)
{
    this->m_autoClearClipboardEnabled = clear;
}

int SilverlockPreferences::autoClearClipboard() const
{
    return this->m_autoClearClipboard;
}

void SilverlockPreferences::setAutoClearClipboard(int seconds)
{
    if (seconds < 1 || seconds > 31536000)
    {
        return;
    }

    this->m_autoClearClipboard = seconds;
}

bool SilverlockPreferences::autoExitEnabled() const
{
    return this->m_autoExitEnabled;
}

void SilverlockPreferences::setAutoExitEnabled(bool exit)
{
    this->m_autoExitEnabled = exit;
}

int SilverlockPreferences::autoExit() const
{
    return this->m_autoExit;
}

void SilverlockPreferences::setAutoExit(int seconds)
{
    if (seconds < 60 || seconds > 31536000)
    {
        return;
    }

    this->m_autoExit = seconds;
}

bool SilverlockPreferences::lockWhenMinimizing() const
{
    return this->m_lockWhenMinimizing;
}

void SilverlockPreferences::setLockWhenMinimizing(bool lock)
{
    this->m_lockWhenMinimizing = lock;
}

bool SilverlockPreferences::clearClipboardWhenClosing() const
{
    return this->m_clearClipboardWhenClosing;
}

void SilverlockPreferences::setClearClipboardWhenClosing(bool clear)
{
    this->m_clearClipboardWhenClosing = clear;
}

bool SilverlockPreferences::minimizeToTray() const
{
    return this->m_minimizeToTray;
}

void SilverlockPreferences::setMinimizeToTray(bool minimize)
{
    this->m_minimizeToTray = minimize;
}

bool SilverlockPreferences::minimizeAfterClipboard() const
{
    return this->m_minimizeAfterClipboard;
}

void SilverlockPreferences::setMinimizeAfterClipboard(bool minimize)
{
    this->m_minimizeAfterClipboard = minimize;
}

bool SilverlockPreferences::minimizeAfterLock() const
{
    return this->m_minimizeAfterLock;
}

void SilverlockPreferences::setMinimizeAfterLock(bool minimize)
{
    this->m_minimizeAfterLock = minimize;
}

/*!
    Gets a value indicating whether run-at-startup is supported on this platform and configuration.
 */
bool SilverlockPreferences::runAtStartupSupported()
{
#if defined(Q_WS_WIN) || defined(Q_WS_MAC)
    return true;
#elif defined(Q_OS_LINUX)
    return !SilverlockPreferences::startupFile().isEmpty();
#else
    return false;
#endif
}

bool SilverlockPreferences::runAtStartup() const
{
    if (!SilverlockPreferences::runAtStartupSupported())
    {
        qWarning() << "Run at startup is not supported on this platform and configuration.";
    }

#ifdef Q_WS_WIN
    QString path = this->applicationPathForRegistry();

    // If we're running on Windows, set the check box if the registry contains the correct key set to the running application's path
    QSettings reg(KEY_WIN_STARTUP_PATH, QSettings::NativeFormat);
    return reg.contains(KEY_WIN_STARTUP_NAME) && reg.value(KEY_WIN_STARTUP_NAME).toString().compare(path, Qt::CaseInsensitive) == 0;
#elif defined(Q_WS_MAC)    
    MacLoginItemsManager manager;
    return manager.containsRunningApplication();
#elif defined(Q_OS_LINUX)
    return QFile::exists(SilverlockPreferences::startupFile());
#else
    return false;
#endif
}

void SilverlockPreferences::setRunAtStartup(bool run)
{
    if (!SilverlockPreferences::runAtStartupSupported())
    {
        Q_UNUSED(run);
        qWarning() << "Run at startup is not supported on this platform and configuration.";
        return;
    }

#ifdef Q_WS_WIN
    QSettings settings(KEY_WIN_STARTUP_PATH, QSettings::NativeFormat);
    if (run)
    {
        settings.setValue(KEY_WIN_STARTUP_NAME, this->applicationPathForRegistry());
    }
    else
    {
        settings.remove(KEY_WIN_STARTUP_NAME);
    }
#elif defined(Q_WS_MAC)
    if (QFile::exists(SilverlockPreferences::macLoginItemsFile()))
    {
        MacLoginItemsManager manager;

        if (run && !SilverlockPreferences::runAtStartup())
        {
            manager.appendRunningApplication();
        }
        else if (!run && SilverlockPreferences::runAtStartup())
        {
            manager.removeRunningApplication();
        }
    }
#elif defined(Q_OS_LINUX)
    switch (LinuxSystemInfo::desktopEnvironment())
    {
        case LinuxSystemInfo::GNOME:
        {
            if (run)
            {
                QFile file(SilverlockPreferences::startupFile());
                if (file.open(QFile::WriteOnly | QFile::Truncate))
                {
                    file.setPermissions(file.permissions() | QFile::ExeOwner | QFile::ExeGroup);

                    QTextStream out(&file);
                    out << "[Desktop Entry]\n";
                    out << "Type=Application\n";
                    out << QString("Exec=%1.desktop\n").arg(QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
                    out << "Hidden=false\n";
                    out << "NoDisplay=false\n";
                    out << "X-GNOME-Autostart-enabled=true\n";
                    out << "Name=Silverlock\n";
                }
            }
            else
            {
                QFile::remove(SilverlockPreferences::startupFile());
            }
        }

        case LinuxSystemInfo::KDE:
        {
            if (run)
            {
                QFile::link(QDir::toNativeSeparators(QCoreApplication::applicationFilePath()), SilverlockPreferences::startupFile());
            }
            else
            {
                QFile::remove(SilverlockPreferences::startupFile());
            }
        }

        default:
            return;
    }
#endif
}

/*!
    \todo For a future Silverlock release...
 */
bool SilverlockPreferences::isFileAssociationSet() const
{
    return false;
}

/*!
    \todo For a future Silverlock release...

    \param active \c true to create file associations; \c false to remove them.
 */
void SilverlockPreferences::setFileAssociationActive(bool active)
{
    Q_UNUSED(active);
}

bool SilverlockPreferences::openLastDatabase() const
{
    return this->m_openLastDatabase;
}

void SilverlockPreferences::setOpenLastDatabase(bool open)
{
    this->m_openLastDatabase = open;
}

bool SilverlockPreferences::updateOnStartup() const
{
    return this->m_updateOnStartup;
}

void SilverlockPreferences::setUpdateOnStartup(bool update)
{
    this->m_updateOnStartup = update;
}

bool SilverlockPreferences::autoSaveOnClose() const
{
    return this->m_autoSaveOnClose;
}

void SilverlockPreferences::setAutoSaveOnClose(bool save)
{
    this->m_autoSaveOnClose = save;
}

const QStringList SilverlockPreferences::recentFileList() const
{
    return this->m_recentFiles;
}

void SilverlockPreferences::clearRecentFiles()
{
    this->m_recentFiles.clear();
}

void SilverlockPreferences::addRecentFile(const QString &fileName)
{
    // Get the absolute path with no symlinks or "." or ".." elements
    QString canonicalPath = QDir(fileName).canonicalPath();

    // No empty file names, please...
    if (canonicalPath.isEmpty())
    {
        return;
    }

    // If we get a fatal or unspecified error opening the file, something probably very bad
    QFile file(canonicalPath);
    if (!file.open(QIODevice::ReadOnly))
    {
        if (file.error() == QFile::FatalError || file.error() == QFile::UnspecifiedError)
        {
            return;
        }
    }

    // Remove all instances of this file previously appearing in the list
    this->m_recentFiles.removeAll(canonicalPath);

    // Add it to the front of the list
    this->m_recentFiles.prepend(canonicalPath);

    // Remove all files after
    while (this->m_recentFiles.count() > this->maxRecentFiles())
    {
        this->m_recentFiles.removeLast();
    }
}

/*!
    Returns the maximum number of paths stored in the recent files list.

    The maximum is currently a fixed 5.
 */
int SilverlockPreferences::maxRecentFiles() const
{
    return 5;
}

void SilverlockPreferences::saveWindowSettings(MainWindow *mainWindow)
{
    // If the user cleared the window settings we won't save them this
    // time so they can be restored when the application restarts
    if (!this->m_windowSettingsCleared)
    {
        QSettings settings;
        settings.setValue(KEY_MAIN_WINDOW_GEOMETRY, mainWindow->saveGeometry());
        settings.setValue(KEY_MAIN_WINDOW_STATE, mainWindow->saveState());
        settings.setValue(KEY_ENTRY_TABLE_STATE, mainWindow->saveEntryTableState());
    }
}

void SilverlockPreferences::restoreWindowSettings(MainWindow *mainWindow)
{
    // Turn this off for now since we might be restoring to fullscreen
    if (mainWindow->unifiedTitleAndToolBarOnMac())
    {
        mainWindow->setUnifiedTitleAndToolBarOnMac(false);
    }

    QSettings settings;
    mainWindow->restoreGeometry(settings.value(KEY_MAIN_WINDOW_GEOMETRY).toByteArray());
    mainWindow->restoreState(settings.value(KEY_MAIN_WINDOW_STATE).toByteArray());
    mainWindow->restoreEntryTableState(settings.value(KEY_ENTRY_TABLE_STATE).toByteArray());

    // We only want the unified title and toolbar when we're not in fullscreen,
    // otherwise it causes problems and can crash the application when returning
    // to normal window state

    // If the main window is not in fullscreen mode, turn on the special toolbar
    if (!mainWindow->isFullScreen() && !mainWindow->unifiedTitleAndToolBarOnMac())
    {
        mainWindow->setUnifiedTitleAndToolBarOnMac(true);
    }
}

void SilverlockPreferences::clearWindowSettings()
{
    QSettings settings;
    settings.remove(KEY_MAIN_WINDOW_GEOMETRY);
    settings.remove(KEY_MAIN_WINDOW_STATE);
    settings.remove(KEY_ENTRY_TABLE_STATE);
    this->m_windowSettingsCleared = true;
}

/*!
    Returns the path of the downloaded update installer.

    This is stored so if the user cancels the installation they can restart
    it later without having to re-download the installation file.
 */
QString SilverlockPreferences::updateInstallerPath() const
{
    return QDir(this->m_updateInstallerPath).canonicalPath();
}

void SilverlockPreferences::setUpdateInstallerPath(const QString &path)
{
    this->m_updateInstallerPath = QDir(path).canonicalPath();
}

// Platform-specific methods...
#ifdef Q_WS_WIN

/*!
    Gets the path of the running application with native separators and enclosed in quotes, suitable
    for entry into the Windows registry.
 */
QString SilverlockPreferences::applicationPathForRegistry()
{
    return QString("\"%1\"").arg(QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
}

#elif defined(Q_WS_MAC)

/*!
    Gets the property list file for login items on Mac OS X.
 */
QString SilverlockPreferences::macLoginItemsFile()
{
    return QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + "/Library/Preferences/loginwindow.plist";
}

#elif defined(Q_OS_LINUX)

/*!
    Gets the absolute path of the run-at-startup file for the current desktop environment.

    Returns an empty string if the desktop environment is not supported.
 */
QString SilverlockPreferences::startupFile()
{
    return SilverlockPreferences::startupFileFor(LinuxSystemInfo::desktopEnvironment());
}

/*!
    Gets the absolute path of the run-at-startup file for the specified desktop environment.

    Returns an empty string if the desktop environment is not supported.
 */
QString SilverlockPreferences::startupFileFor(LinuxSystemInfo::DesktopEnvironment desktopEnvironment)
{
    switch (desktopEnvironment)
    {
        case LinuxSystemInfo::GNOME:
            return QString("%1/config/autostart/%2.desktop").arg(QDir::homePath()).arg(ApplicationInfo::unixName());
        case LinuxSystemInfo::KDE:
            return QString("%1/.kde/Autostart/%2").arg(QDir::homePath()).arg(ApplicationInfo::unixName());
        default:
            return QString();
    }
}

#endif
