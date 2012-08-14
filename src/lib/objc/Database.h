//
//  Database.h
//  Silverlock
//
//  Created by Jake Petroules on 1/27/11.
//  Copyright 2011 Petroules Corporation. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Group.h"

@interface Database : Group
{
@private
	NSString *m_password;
	int m_compression;
}

-(id) init;
-(id) initWithTitle: (NSString*) title;
-(id) initWithTitle: (NSString*) title andPassword: (NSString*) password;
-(void) dealloc;
+(id) version; // ?
-(NSString*) password;
-(void) setPassword: (NSString*) password;
-(int) compression;
-(void) setCompression: (int) compression;
-(void) setParentNode: (Group*) node;
-(id) toXml: (id) element; // ?

@end
