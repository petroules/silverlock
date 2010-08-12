#ifndef SPARKLEAUTOUPDATER_H
#define SPARKLEAUTOUPDATER_H

#include <QtCore>
#include "autoupdater.h"

class SparkleAutoUpdater : public AutoUpdater
{
public:
    SparkleAutoUpdater(const QUrl &updateUrl);
    ~SparkleAutoUpdater();
    void checkForUpdates(bool background);

private:
    class Private;
    Private *d;
};

#endif // SPARKLEAUTOUPDATER_H
