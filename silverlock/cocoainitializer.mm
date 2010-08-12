#include "cocoainitializer.h"

#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>

class CocoaInitializer::Private
{
public:
    NSAutoreleasePool *autoReleasePool;
};

CocoaInitializer::CocoaInitializer()
{
    this->d = new CocoaInitializer::Private();
    NSApplicationLoad();
    this->d->autoReleasePool = [[NSAutoreleasePool alloc] init];
}

CocoaInitializer::~CocoaInitializer()
{
    [this->d->autoReleasePool release];
    delete this->d;
}
