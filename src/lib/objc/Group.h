//
//  Group.h
//  Silverlock
//
//  Created by Jake Petroules on 1/27/11.
//  Copyright 2011 Petroules Corporation. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "DatabaseNode.h"

@class Entry;
@class SearchParameters;

@interface Group : DatabaseNode
{
@private
	NSMutableArray *m_groups;
	NSMutableArray *m_entries;
}

-(id) init;
-(id) initWithTitle: (NSString*) title;
-(id) initWithTitle: (NSString*) title andParent: (Group*) parent;
-(void) dealloc;
-(NSArray*) groups;
-(NSArray*) entries;
-(int) countGroups;
-(int) countEntries;
-(BOOL) containsUuid: (CFUUIDRef) uuid;
-(BOOL) isGroup: (CFUUIDRef) uuid;
-(BOOL) isEntry: (CFUUIDRef) uuid;
-(Group*) findGroup: (CFUUIDRef) uuid;
-(Group*) findGroup: (CFUUIDRef) uuid includeThis: (BOOL) includeThis;
-(Entry*) findEntry: (CFUUIDRef) uuid;
-(NSArray*) findEntries: (SearchParameters*) params;
-(id) toXml: (id) document;

@end
