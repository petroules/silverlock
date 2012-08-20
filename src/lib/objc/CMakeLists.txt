set(TARGET_NAME "silverlockobjc")

add_library("${TARGET_NAME}"
    Database.h
    Database.i.h
    Database.m
    DatabaseNode.h
    DatabaseNode.i.h
    DatabaseNode.m
    Entry.h
    Entry.i.h
    Entry.m
    Group.h
    Group.i.h
    Group.m
    silverlocklibobjc-Info.plist
    Untitled.m
)

if(IOS)
    find_library(FOUNDATION_LIBRARY Foundation)
    mark_as_advanced(FOUNDATION_LIBRARY)
    
    target_link_libraries("${TARGET_NAME}" ${FOUNDATION_LIBRARY})
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    mark_as_advanced(COCOA_LIBRARY)
    
    target_link_libraries("${TARGET_NAME}" ${COCOA_LIBRARY})
endif()