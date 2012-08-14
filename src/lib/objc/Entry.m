//
//  Entry.m
//  Silverlock
//
//  Created by Jake Petroules on 1/31/11.
//  Copyright 2011 Petroules Corporation. All rights reserved.
//

#import "Entry.i.h"
#import "DatabaseNode.i.h"

// We aren't supposed to be including protected Group methods
// because Entry is not a subclass of it, however as Objective-C
// lacks friend classes (C++), internal visibility modifier (C#)
// or package visibility (Java), this is a necessary hack.
#import "Group.i.h"

@implementation Entry

-(id) init
{
	return [self initWithTitle: [NSString string]];
}

-(id) initWithTitle: (NSString*) title
{
	return [self initWithTitle: [NSString string] andParent: nil];
}

-(id) initWithTitle: (NSString*) title andParent: (Group*) parent
{
	if (self = [super initWithTitle: title])
	{
		self->m_url = nil;
		self->m_username = [NSString string];
		self->m_password = [NSString string];
		self->m_notes = [NSString string];
		self->m_recoveryInfo = [NSMutableDictionary dictionary];
		self->m_customFields = [NSMutableDictionary dictionary];
	}
	
	return self;
}

-(void) dealloc
{
	[self->m_url release];
	[self->m_username release];
	[self->m_password release];
	[self->m_notes release];
	[self->m_recoveryInfo release];
	[self->m_customFields release];
	
	[self detach];
	
	[super dealloc];
}

-(NSString*) username
{
	return self->m_username;
}

-(void) setUsername: (NSString*) username
{
	if (![self->m_username isEqualToString: username])
	{
		self->m_username = [NSString stringWithString: username];
		[self setModified];
	}
}

-(NSString*) password
{
	return self->m_password;
}

-(void) setPassword: (NSString*) password
{
	if (![self->m_password isEqualToString: password])
	{
		self->m_password = [NSString stringWithString: password];
		[self setModified];
	}
}

-(NSURL*) url
{
	return self->m_url;
}

-(void) setUrl: (NSURL*) url
{
	if (![self->m_url isEqual: url])
	{
		self->m_url = [url copy];
		[self setModified];
	}
}

-(NSString*) notes
{
	return self->m_notes;
}

-(void) setNotes: (NSString*) notes
{
	if (![self->m_notes isEqualToString: notes])
	{
		self->m_notes = notes;
		[self setModified];
	}
}

-(NSDictionary*) recoveryInfo
{
	return [NSDictionary dictionaryWithDictionary: self->m_recoveryInfo];
}

-(NSDictionary*) customFields
{
	return [NSDictionary dictionaryWithDictionary: self->m_customFields];
}

-(void) insertRecoveryQuestion: (NSString*) question withAnswer: (NSString*) answer
{
	// Blank questions are not accepted!
	if ([question length] == 0 || [self->m_recoveryInfo valueForKey: question] != nil)
	{
		return;
	}
	
	[self->m_recoveryInfo setValue: answer forKey: question];
	[self setModified];
}

-(void) removeRecoveryQuestion: (NSString*) question
{
	if ([self->m_recoveryInfo valueForKey: question] != nil)
	{
		[self->m_recoveryInfo removeObjectForKey: question];
		[self setModified];
	}
}

-(void) clearRecoveryQuestions
{
	if ([self->m_recoveryInfo count] > 0)
	{
		[self->m_recoveryInfo removeAllObjects];
		[self setModified];
	}
}

-(void) insertCustomField: (NSString*) key withValue: (NSString*) value
{
	// Blank keys are not accepted!
	if ([key length] == 0 || [self->m_customFields valueForKey: key] != nil)
	{
		return;
	}
	
	[self->m_customFields setValue: value forKey: key];
	[self setModified];
}

-(void) removeCustomField: (NSString*) key
{
	if ([self->m_customFields valueForKey: key] != nil)
	{
		[self->m_customFields removeObjectForKey: key];
		[self setModified];
	}
}

-(void) clearCustomFields
{
	if ([self->m_customFields count] > 0)
	{
		[self->m_customFields removeAllObjects];
		[self setModified];
	}
}

-(id) createCopy
{
	Entry *entry = [[Entry alloc] init];
	[entry setTitle: [self title]];
	[entry setUsername: [self username]];
	[entry setPassword: [self password]];
	[entry setUrl: [self url]];
	[entry setNotes: [self notes]];
	
	NSEnumerator *enumeratorRecoveryInfo = [self->m_recoveryInfo keyEnumerator];
	id question;
	while ((question = [enumeratorRecoveryInfo nextObject]))
	{
		[entry insertRecoveryQuestion: question withAnswer: [self->m_recoveryInfo valueForKey: question]];
	}
	
	NSEnumerator *enumeratorCustomFields = [self->m_customFields keyEnumerator];
	id key;
	while ((key = [enumeratorCustomFields nextObject]))
	{
		[entry insertCustomField: key withValue: [self->m_customFields valueForKey: key]];
	}
	
	return entry;
}

@end

@implementation Entry (Protected)

-(void) attachToList
{
	[[[self parentNode] internalEntriesList] addObject: self];
}

-(void) detachFromList
{
	[[[self parentNode] internalEntriesList] removeObjectIdenticalTo: self];
}

@end