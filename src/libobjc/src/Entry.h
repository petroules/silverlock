//
//  Entry.h
//  Silverlock
//
//  Created by Jake Petroules on 1/31/11.
//  Copyright 2011 Petroules Corporation. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "DatabaseNode.h"

@interface Entry : DatabaseNode
{
@private
	NSURL *m_url;
	NSString *m_username;
	NSString *m_password;
	NSString *m_notes;
	NSMutableDictionary *m_recoveryInfo;
	NSMutableDictionary *m_customFields;
}

-(id) init;
-(id) initWithTitle: (NSString*) title;
-(id) initWithTitle: (NSString*) title andParent: (Group*) parent;
-(void) dealloc;
-(NSString*) username;
-(void) setUsername: (NSString*) username;
-(NSString*) password;
-(void) setPassword: (NSString*) password;
-(NSURL*) url;
-(void) setUrl: (NSURL*) url;
-(NSString*) notes;
-(void) setNotes: (NSString*) notes;
-(NSDictionary*) recoveryInfo;
-(NSDictionary*) customFields;
-(void) insertRecoveryQuestion: (NSString*) question withAnswer: (NSString*) answer;
-(void) removeRecoveryQuestion: (NSString*) question;
-(void) clearRecoveryQuestions;
-(void) insertCustomField: (NSString*) key withValue: (NSString*) value;
-(void) removeCustomField: (NSString*) key;
-(void) clearCustomFields;
-(id) createCopy;

@end
