set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_SOURCE_DIR}/src/lib/qt"
    "${CMAKE_SOURCE_DIR}/lib/petroules-utilities-qt/src"
    "${CMAKE_SOURCE_DIR}/lib/petroules-utilities-qt/lib/qtsolutions/qtsingleapplication/src"
)

if(WIN32 AND BUILD_SHARED_LIBS)
    add_definitions(-DSILVERLOCKLIB_IMPORTS -DPETROULESUTILITIES_IMPORTS -DQT_QTSINGLEAPPLICATION_IMPORT)
endif()

if(APPLE)
    set(TARGET_NAME "${CMAKE_PROJECT_NAME}")
else()
    set(TARGET_NAME "${PROJECT_NAME_UNIX}")
endif()

set(SOURCES
    applicationmenu.cpp
    applicationmenu.h
    databaseprinter.cpp
    databaseprinter.h
    databaseprinterfields.cpp
    databaseprinterfields.h
  
    cmake/install.cmake
    cmake/package.cmake
    
    dialogs/aboutdialog.cpp
    dialogs/aboutdialog.h
    dialogs/aboutdialog.ui
    dialogs/configurecolumnsdialog.cpp
    dialogs/configurecolumnsdialog.h
    dialogs/configurecolumnsdialog.ui
    dialogs/databaseauthenticationdialog.cpp
    dialogs/databaseauthenticationdialog.h
    dialogs/databaseauthenticationdialog.ui
    dialogs/databaseprintpreviewdialog.cpp
    dialogs/databaseprintpreviewdialog.h
    dialogs/databaseprintpreviewdialog.ui
    dialogs/entryeditdialog.cpp
    dialogs/entryeditdialog.h
    dialogs/entryeditdialog.ui
    dialogs/entryeditdialog_helper.cpp
    dialogs/groupeditdialog.cpp
    dialogs/groupeditdialog.h
    dialogs/groupeditdialog.ui
    dialogs/guardeddialog.cpp
    dialogs/guardeddialog.h
    dialogs/licensedialog.cpp
    dialogs/licensedialog.h
    dialogs/licensedialog.ui
    dialogs/newdatabasewizard.cpp
    dialogs/newdatabasewizard.h
    dialogs/newdatabasewizard.ui
    dialogs/preferencesdialog.cpp
    dialogs/preferencesdialog.h
    dialogs/preferencesdialog.ui
    dialogs/searchdatabasedialog.cpp
    dialogs/searchdatabasedialog.h
    dialogs/searchdatabasedialog.ui
    dialogs/selectgroupdialog.cpp
    dialogs/selectgroupdialog.h
    dialogs/selectgroupdialog.ui
    dialogs/updatedialog.cpp
    dialogs/updatedialog.h
    dialogs/updatedialog.ui
    dialogs/welcomedialog.cpp
    dialogs/welcomedialog.h
    dialogs/welcomedialog.ui
    
    documentstate.cpp
    documentstate.h
    Info.plist
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    mainwindow_menuhandlers.cpp
    
    res/db.svg
    res/help.png
    res/iexplore.png
    res/print_preview.png
    res/printer.svg
    
    resources.qrc
    silverlock.manifest
    silverlock.rc.txt
    silverlockapplication.cpp
    silverlockapplication.h
    silverlockpreferences.cpp
    silverlockpreferences.h
    silverlockpreferences_keys.h
    
    tr/silverlock_de.ts
    tr/silverlock_fr.ts
    
    version.in.h

    widgets/entrytablewidget.cpp
    widgets/entrytablewidget.h
    widgets/entrytablewidget.ui
    widgets/entryviewindexes.h
    widgets/expandingspacerwidget.cpp
    widgets/expandingspacerwidget.h
    widgets/groupbrowserwidget.cpp
    widgets/groupbrowserwidget.h
    widgets/groupbrowserwidget.ui
    widgets/toolbarsearchwidget.cpp
    widgets/toolbarsearchwidget.h
    widgets/toolbarsearchwidget.ui

    ../../config.in.h

    ../../res/app.icns
    ../../res/globalresources.qrc
)

qt_transform_sources(SOURCES "${SOURCES}")

set(RC_ICON "${CMAKE_SOURCE_DIR}/res/app.ico")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/silverlock.rc.txt" "${CMAKE_CURRENT_BINARY_DIR}/silverlock.rc")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.in.h" "${CMAKE_CURRENT_BINARY_DIR}/version.h")

if(WIN32)
    add_definitions(-DWINVER=0x0500)
endif()

if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE app.icns)
    set_source_files_properties(../../res/app.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

add_executable("${TARGET_NAME}" WIN32 MACOSX_BUNDLE ${SOURCES})
target_link_libraries("${TARGET_NAME}" silverlockqt petroules-utilities
    ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTXML_LIBRARY})
    
if(WIN32)
    target_link_libraries("${TARGET_NAME}" ${QT_QTMAIN_LIBRARY} user32)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    mark_as_advanced(COCOA_LIBRARY)
    
    find_library(COREFOUNDATION_LIBRARY CoreFoundation)
    mark_as_advanced(COREFOUNDATION_LIBRARY)
    
    target_link_libraries("${TARGET_NAME}" ${COREFOUNDATION_LIBRARY} ${COCOA_LIBRARY})
elseif(UNIX)
    target_link_libraries("${TARGET_NAME}" X11)
endif()

include(install)
include(package)
