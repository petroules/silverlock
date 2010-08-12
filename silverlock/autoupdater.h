#ifndef AUTOUPDATER_H
#define AUTOUPDATER_H

class AutoUpdater
{
public:
    virtual ~AutoUpdater();
    virtual void checkForUpdates(bool background) = 0;
};

#endif // AUTOUPDATER_H
