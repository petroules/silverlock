#include "sparkleautoupdater.h"

#include <Cocoa/Cocoa.h>
#include <Sparkle/Sparkle.h>

class SparkleAutoUpdater::Private
{
public:
    SUUpdater *updater;
};

SparkleAutoUpdater::SparkleAutoUpdater(const QUrl &updateUrl)
{
    this->d = new Private;
    this->d->updater = [SUUpdater sharedUpdater];
    [this->d->updater retain];

    NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:updateUrl.toString().toUtf8().data()]];
    [this->d->updater setFeedURL:url];
}

SparkleAutoUpdater::~SparkleAutoUpdater()
{
    [this->d->updater release];
    delete this->d;
}

void SparkleAutoUpdater::checkForUpdates(bool background)
{
    if (background)
    {
        [this->d->updater checkForUpdatesInBackground];
    }
    else
    {
        [this->d->updater checkForUpdates:nil];
    }
}
