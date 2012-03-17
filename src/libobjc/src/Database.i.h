//
//  Database.pt.h
//  Silverlock
//
//  Created by Jake Petroules on 1/27/11.
//  Copyright 2011 Petroules Corporation. All rights reserved.
//

#import "Database.h"

@interface Database (Protected)

-(void) attachToList;
-(void) detachFromList;
-(void) fromXml: (id) element; // ?

@end
