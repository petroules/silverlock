#include "silverlockpreferences.h"
#include "silverlockpreferences_keys.h"

/*!
    \class SilverlockPreferences

    The SilverlockPreferences class encapsulates all settings and preferences for the application
    and provides methods to persist them.

    Settings are stored and retrieved using the standard QSettings class. When the class is
    constructed, any existing settings are loaded; any non-existing settings will be set to
    their default values.

    Settings are NOT saved when the class' destructor is called.

    Default settings can be persisted by calling \link restoreDefaults() \endlink and
    \link save() \endlink in succession.
 */

SilverlockPreferences::SilverlockPreferences()
{
    this->load();
}

void SilverlockPreferences::load()
{
    // Restoring the defaults allows us to determine the default values when
    // reading settings without duplicating them - all default values should
    // appear in the restoreDefaults() method; nowhere else
    this->restoreDefaults();

    QSettings settings;
    this->m_expandTreeViewNodes = settings.value(KEY_EXPAND_TREE_VIEW_NODES, this->m_expandTreeViewNodes).toBool();
}

void SilverlockPreferences::save() const
{
    QSettings settings;
    settings.setValue(KEY_EXPAND_TREE_VIEW_NODES, this->m_expandTreeViewNodes);
}

void SilverlockPreferences::restoreDefaults()
{
    this->m_expandTreeViewNodes = true;
}

bool SilverlockPreferences::expandTreeViewNodes() const
{
    return this->m_expandTreeViewNodes;
}

void SilverlockPreferences::setExpandTreeViewNodes(bool expand)
{
    this->m_expandTreeViewNodes = expand;
}
