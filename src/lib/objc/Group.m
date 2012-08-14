//
//  Group.m
//  Silverlock
//
//  Created by Jake Petroules on 1/27/11.
//  Copyright 2011 Petroules Corporation. All rights reserved.
//

#import "Group.i.h"
#import "DatabaseNode.i.h"

@interface Group (Private)

-(Group*) findGroupInternal: (CFUUIDRef) uuid;

@end

@implementation Group

-(id) init
{
	return [self initWithTitle: [NSString string]];
}

-(id) initWithTitle: (NSString*) title
{
	return [self initWithTitle: title andParent: nil];
}

-(id) initWithTitle: (NSString*) title andParent: (Group*) parent
{
	if (self = [super initWithTitle: title])
	{
		[self setParentNode: parent];
	}
	
	return self;
}

-(void) dealloc
{
	[self detach];
	
	NSArray *entries = [self entries];
	for (Entry *entry in entries)
	{
		[entry release];
	}
	
	NSArray *groups = [self groups];
	for (Group *subgroup in groups)
	{
		[subgroup release];
	}
	
	[super dealloc];
}

-(NSArray*) groups
{
	return [NSArray arrayWithArray: self->m_groups];
}

-(NSArray*) entries
{
	return [NSArray arrayWithArray: self->m_entries];
}

-(int) countGroups
{
	int total = 0;
	
	for (Group *group in self->m_groups)
	{
		total += [group countGroups];
	}
	
	return total + [self->m_groups count];
}

-(int) countEntries
{
	int total = 0;
	
	for (Group *group in self->m_groups)
	{
		total += [group countEntries];
	}
	
	return total + [self->m_entries count];
}

-(BOOL) containsUuid: (CFUUIDRef) uuid
{
	return [self isGroup: uuid] || [self isEntry: uuid];
}

-(BOOL) isGroup: (CFUUIDRef) uuid
{
	return [self findGroup: uuid] != nil;
}


-(BOOL) isEntry: (CFUUIDRef) uuid
{
	return [self findEntry: uuid] != nil;
}

-(Group*) findGroup: (CFUUIDRef) uuid
{
	return [self findGroup: uuid includeThis: NO];
}

-(Group*) findGroup: (CFUUIDRef) uuid includeThis: (BOOL) includeThis
{
	if (includeThis && CFEqual([self uuid], uuid))
	{
		return self;
	}
	else
	{
		return [self findGroupInternal: uuid];
	}
}

-(Entry*) findEntry: (CFUUIDRef) uuid
{
	for (Entry *entry in self->m_entries)
	{
		if (CFEqual([entry uuid], uuid))
		{
			return entry;
		}
	}
	
	for (Group *group in self->m_groups)
	{
		Entry *candidate = [group findEntry: uuid];
		if (candidate && CFEqual([candidate uuid], uuid))
		{
			return candidate;
		}
	}
	
	return nil;
}

@end

@implementation Group (Protected)

-(void) attachToList
{
	[[self parentNode]->m_groups addObject: self];
}

-(void) detachFromList
{
	[[self parentNode]->m_groups removeObjectIdenticalTo: self];
}

-(NSMutableArray*) internalGroupsList
{
	return self->m_groups;
}

-(NSMutableArray*) internalEntriesList
{
	return self->m_entries;
}

@end

@implementation Group (Private)

-(Group*) findGroupInternal: (CFUUIDRef) uuid
{
	for (Group *group in self->m_groups)
	{
		// If this group is a match, return it
		if (CFEqual([group uuid], uuid))
		{
			return group;
		}
		else
		{
			// Otherwise search its children
			Group *candidate = [group findGroup: uuid includeThis: YES];
			if (candidate && CFEqual([candidate uuid], uuid))
			{
				return candidate;
			}
		}
	}
	
	return nil;
}

@end