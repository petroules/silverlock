//
//  DatabaseNode.pt.h
//  Silverlock
//
//  Created by Jake Petroules on 1/27/11.
//  Copyright 2011 Petroules Corporation. All rights reserved.
//

#import "DatabaseNode.h"

@interface DatabaseNode (Protected)

-(id) init;
-(id) initWithTitle: (NSString*) title;
-(void) setUuid: (CFUUIDRef) uuid;
-(void) setModified;
-(void) setModified: (NSDate*) modified;
-(void) detach;
-(void) fromXml: (id) element; // ?
-(void) attachToList; // abstract/protocol (pure virtual)
-(void) detachFromList; // abstract/protocol (pure virtual)

@end
