#ifndef SILVERLOCKSETTINGS_H
#define SILVERLOCKSETTINGS_H

#include <QtCore>

class SilverlockPreferences
{
public:
    SilverlockPreferences();
    void load();
    void save() const;
    void restoreDefaults();
    bool expandTreeViewNodes() const;
    void setExpandTreeViewNodes(bool expand);
    bool subgroupsInDetailView() const;
    bool entriesInTreeView() const;

private:
    bool m_expandTreeViewNodes;
};

#endif // SILVERLOCKSETTINGS_H
