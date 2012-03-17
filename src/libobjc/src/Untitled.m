#import <Foundation/Foundation.h>

#import "Database.h"
#import "Group.h"

@interface Boo : NSObject

-(Boo*) init;
-(void) dealloc;
-(void) eventHandler: (NSNotification*) notification;

@end

@implementation Boo

-(Boo*) init
{
	[[NSNotificationCenter defaultCenter] addObserver: self selector: @selector(eventHandler:) name: @"treeModified" object: nil];
	
	return self;
}

-(void) dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver: self name: @"treeModified" object: nil];
	[super dealloc];
}

-(void) eventHandler: (NSNotification*) notification
{
	NSLog(@"Event triggereds %@", [notification name]);
}

@end

int main (int argc, const char * argv[])
{
	// 196-198 for polymorphism/id stuff
	// c functions are the same, as are function pointers and unions (p303)
	// bit fields page 280-282
	// 322 # objects
	// 436 for archving with plists
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	
	Boo *boo = [[Boo alloc] init];
	
	[[NSNotificationCenter defaultCenter] postNotificationName: @"treeModified" object: nil];
	
	[boo release];
	
	Database *database = [[Database alloc] init];
	NSLog(@"%@", [database uuidString]);
    [database release];
	
    [pool drain];
    return 0;
}
