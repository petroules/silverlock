//
//  Entry.i.h
//  Silverlock
//
//  Created by Jake Petroules on 1/31/11.
//  Copyright 2011 Petroules Corporation. All rights reserved.
//

#import "Entry.h"

@interface Entry (Protected)

-(void) attachToList;
-(void) detachFromList;
-(void) fromXml: (id) element; // ?

@end
