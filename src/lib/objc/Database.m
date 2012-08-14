//
//  Database.m
//  Silverlock
//
//  Created by Jake Petroules on 1/27/11.
//  Copyright 2011 Petroules Corporation. All rights reserved.
//

#import "Database.i.h"
#import "Group.i.h"
#import "DatabaseNode.i.h"

@implementation Database

-(id) init
{
	return [self initWithTitle: [NSString string]];
}

-(id) initWithTitle: (NSString*) title
{
	return [self initWithTitle: title andPassword: [NSString string]];
}

-(id) initWithTitle: (NSString*) title andPassword: (NSString*) password
{
	if ((self = [super initWithTitle: title]))
	{
		self->m_password = [NSString stringWithString: password];
		self->m_compression = -1;
	}
	
	return self;
}

-(void) dealloc
{
	[self->m_password release];
	[super dealloc];
}

+(id) version
{
}

-(NSString*) password
{
	return self->m_password;
}

-(void) setPassword: (NSString*) password
{
	if ([self->m_password isEqualToString: password])
	{
		self->m_password = [NSString stringWithString: password];
		[self setModified];
	}
}

-(int) compression
{
	return self->m_compression;
}

-(void) setCompression: (int) compression
{
	if (self->m_compression != compression)
	{
		// If the compression is outside the proper range...
		if (compression < -1 || compression > 9)
		{
			// ...default to default compression
			compression = -1;
		}
		
		self->m_compression = compression;
		[self setModified];
	}
}

-(void) setParentNode: (Group*) node
{
	// Intentionally does nothing - databases cannot have parent nodes
}

-(void) attachToList
{
	// VOID
}

-(void) detachFromList
{
	// VOID
}

@end
