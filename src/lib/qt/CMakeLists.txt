set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

if(WIN32 AND BUILD_SHARED_LIBS)
    add_definitions(-DSILVERLOCKLIB_EXPORTS -DPETROULESUTILITIES_IMPORTS)
endif()

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_SOURCE_DIR}/lib/petroules-utilities-qt/src"
    "${CMAKE_SOURCE_DIR}/lib/petroules-utilities-qt/lib/qtsolutions/qtsingleapplication/src"
)

include(Botan)

set(TARGET_NAME "silverlockqt")

set(SOURCES
    "${CMAKE_CURRENT_BINARY_DIR}/botan/botan_all.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/botan/botan_all.h"
  
    cmake/Botan.cmake
    cmake/BotanUniversal.cmake
    
    database.cpp
    database.h
    database_keys.h
    databasecrypto.cpp
    databasecrypto.h
    databasenode.cpp
    databasenode.h
    databasereader.cpp
    databasereader.h
    databasewriter.cpp
    databasewriter.h
    entry.cpp
    entry.h
    group.cpp
    group.h
    searchparameters.cpp
    searchparameters.h
    silverlocklib.h
    silverlocklib.rc
    silverlocklib_global.h
    
    tr/silverlocklib_de.ts
    tr/silverlocklib_fr.ts
    
    version.in.h
  
    ../../../config.in.h
)

qt_transform_sources(SOURCES "${SOURCES}")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.in.h" "${CMAKE_CURRENT_BINARY_DIR}/version.h")

if(WIN32)
    add_definitions(-DNOMINMAX -DWINVER=0x0500)
endif()

add_library("${TARGET_NAME}" ${SOURCES})
target_link_libraries("${TARGET_NAME}" petroules-utilities ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTXML_LIBRARY})

if(WIN32)
    # Botan needs some stuff from here
    target_link_libraries("${TARGET_NAME}" advapi32 user32)
endif()

if(IS_RELEASE_BUILD)
    install(TARGETS "${TARGET_NAME}"
        DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_REL} COMPONENT Runtime
    )
endif()
