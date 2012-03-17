//
//  Group.i.h
//  Silverlock
//
//  Created by Jake Petroules on 1/30/11.
//  Copyright 2011 Petroules Corporation. All rights reserved.
//

#import "Group.h"

@interface Group (Protected)

-(void) attachToList;
-(void) detachFromList;
-(NSMutableArray*) internalGroupsList;
-(NSMutableArray*) internalEntriesList;

@end
