//
//  DatabaseNode.h
//  Silverlock
//
//  Created by Jake Petroules on 1/23/11.
//  Copyright 2011 Petroules Corporation. All rights reserved.
//

#import <Foundation/Foundation.h>

@class Group;

@interface DatabaseNode : NSObject
{
@protected
	Group *m_parent;
	
@private
	NSDate *m_created;
	NSDate *m_accessed;
	NSDate *m_modified;
	CFUUIDRef m_uuid;
	NSString *m_title;
}

-(void) dealloc;
-(NSDate*) created;
-(NSDate*) accessed;
-(void) setAccessed;
-(NSDate*) modified;
-(CFUUIDRef) uuid;
-(NSString*) uuidString;
-(NSString*) title;
-(void) setTitle: (NSString*) title;
-(Group*) parentNode;
-(void) setParentNode: (Group*) node;
-(Group*) rootNode;
-(BOOL) hasDatabase;
-(id) toXml; // ?
-(BOOL) isAttached;

@end
