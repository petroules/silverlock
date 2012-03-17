//
//  DatabaseNode.m
//  Silverlock
//
//  Created by Jake Petroules on 1/23/11.
//  Copyright 2011 Petroules Corporation. All rights reserved.
//

#import "DatabaseNode.i.h"
#import "Database.h"
#import "Group.h"

@interface DatabaseNode (Private)

-(void) setCreated: (NSDate*) created;
-(void) setAccessed: (NSDate*) accessed;
-(void) attach;
-(void) treeModified;

@end

@implementation DatabaseNode

-(void) dealloc
{
	CFRelease(self->m_uuid);
	[super dealloc];
}

-(NSDate*) created
{
	return self->m_created;
}

-(NSDate*) accessed
{
	return self->m_accessed;
}

/*!
    @method     setAccessed
    @abstract   Sets the last access time of the node to the current date and time.
    @discussion Calling this method will NOT emit the \a treeModified() signal.
*/
-(void) setAccessed
{
	self->m_accessed = [NSDate date];
}

-(NSDate*) modified
{
	return self->m_modified;
}

-(CFUUIDRef) uuid
{
	return self->m_uuid;
}

-(NSString*) uuidString
{
    return [(NSString *)CFUUIDCreateString(kCFAllocatorDefault, [self uuid]) autorelease];
}

-(NSString*) title
{
	return self->m_title;
}

-(void) setTitle: (NSString*) title
{
	if (![self->m_title isEqualToString: title])
	{
		self->m_title = [title copy];
		[self setModified];
	}
}

-(Group*) parentNode
{
	return self->m_parent;
}

-(void) setParentNode: (Group*) node
{
	if (self == node || self->m_parent == node)
	{
		return;
	}
	
	[self detach];
	self->m_parent = node;
	[self attach];
}

-(Group*) rootNode
{
	if (![self isAttached])
	{
		return nil;
	}
	
	Group *parent = self->m_parent;
	while ([parent parentNode] != nil)
	{
		parent = [parent parentNode];
	}
	
	return parent;
}

-(BOOL) hasDatabase
{
	return [[self rootNode] isKindOfClass: [Database class]];
}

-(BOOL) isAttached
{
	return [self parentNode] != nil;
}

@end

//
// Protected methods
//

@implementation DatabaseNode (Protected)

-(id) init
{
	return [self initWithTitle: [NSString string]];
}

-(id) initWithTitle: (NSString*) title
{
	if (self = [super init])
	{
		self->m_parent = nil;
		self->m_created = [NSDate date];
		self->m_accessed = [NSDate date];
		self->m_modified = [NSDate date];
		self->m_uuid = CFUUIDCreate(kCFAllocatorDefault);
		self->m_title = [NSString stringWithString: title];
	}
	
	return self;
}

-(void) setUuid: (CFUUIDRef) uuid
{
	// We're not setting a null UUID or the same one!
	if (uuid && !CFEqual(self->m_uuid, uuid))
	{
		CFRelease(self->m_uuid);
		self->m_uuid = uuid;
		[self setModified];
	}
}

-(void) setModified: (NSDate*) modified
{
	if (modified && ![self->m_modified isEqualToDate: modified])
	{
		self->m_modified = [modified copy];
	}
}

-(void) setModified
{
	self->m_modified = [NSDate date];
	[self treeModified];
}

-(void) detach
{
	if ([self isAttached])
	{
		[self detachFromList];
		
		// Notify the tree that it has been modified and
		// disconnect our modification signal from the parent's
		[self treeModified];
        self->m_parent = nil;
	}
}

@end

//
// Private methods
//

@implementation DatabaseNode (Private)

-(void) setCreated: (NSDate*) created
{
	if (created && ![self->m_created isEqualToDate: created])
	{
		self->m_created = [created copy];
	}
}
-(void) setAccessed: (NSDate*) accessed
{
	if (accessed && ![self->m_accessed isEqualToDate: accessed])
	{
		self->m_accessed = [accessed copy];
	}
}

-(void) attach
{
	if (self->m_parent != nil)
	{
		// Ensure UUID uniquity
		while ([[self rootNode] containsUuid: [self uuid]])
		{
			[self setUuid: CFUUIDCreate(kCFAllocatorDefault)];
		}
		
		// When we add an entry to the parent, the DB node was modified
		[self attachToList];
		
		// Notify the tree that is has been modified
		[self treeModified];
	}
}

-(void) treeModified
{
	id sender = [NSNotificationCenter defaultCenter];
	[sender postNotificationName: @"treeModified" object: self];
	if ([self isAttached])
	{
		[sender postNotificationName: @"treeModified" object: self->m_parent];
	}
}

-(NSString *)getUTCFormateDate:(NSDate *)localDate
{
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
    NSTimeZone *timeZone = [NSTimeZone timeZoneWithName:@"UTC"];
    [dateFormatter setTimeZone:timeZone];
    [dateFormatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
    NSString *dateString = [dateFormatter stringFromDate:localDate];
    [dateFormatter release];
    return dateString;
}

@end