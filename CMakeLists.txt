cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_SOURCE_DIR}/lib/petroules-utilities-qt/cmake ${CMAKE_MODULE_PATH})
include(Solar)

# --------------------------------------------------
# PROJECT VARIABLES
# --------------------------------------------------

set(PROJECT_ID              "com.petroules.silverlock")
set(PROJECT_UUID            "{CA3E50C3-460E-4E12-9EFB-34030DC15306}")

project(                    "Silverlock") # sets PROJECT_NAME / CMAKE_PROJECT_NAME
set(PROJECT_NAME_UNIX       "silverlock")
set(PROJECT_SUMMARY         "Silverlock password manager")
set(PROJECT_DESCRIPTION     "Silverlock is a secure password management application designed to ease the management of account credentials and related information. It allows you to store your usernames, passwords, and other information related to your online accounts and authentications in a single file encrypted with a master password. Without the master password, no one else can access your information. The application is designed with simplicity and ease of use in mind.")

set(PROJECT_VERSION_MAJOR   "1")
set(PROJECT_VERSION_MINOR   "1")
set(PROJECT_VERSION_PATCH   "2")
set(PROJECT_VERSION_BUILD   "0")
set(PROJECT_VERSION         "1.1.2")

git_append_shorttag(PROJECT_VERSION)

set(PROJECT_VENDOR          "Petroules")
set(PROJECT_CONTACT         "info@petroules.com")
set(PROJECT_COPYRIGHT       "Copyright (c) 2010-2012 Petroules Corporation")
set(ORG_WEBSITE             "http://www.petroules.com/")
set(ORG_DOMAIN              "petroules.com")

set(PROJECT_README          "README.md")
set(PROJECT_LICENSE         "COPYING.md")

# --------------------------------------------------
# DEPENDENCIES
# --------------------------------------------------

# Separator for paths in the PATH environment variable
if(WIN32)
    set(MONOTONE_PATH_SEP ";")
else()
    set(MONOTONE_PATH_SEP ":")
endif()

use_qt_sdk_locations()

find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtNetwork QtSvg QtXml REQUIRED)
if(QT4_FOUND)
    include(${QT_USE_FILE})
endif()

set(PythonInterp_Versions 2.7 2.6 2.5)
if(WIN32)
    foreach(py_ver ${PythonInterp_Versions})
        string(REGEX REPLACE "\\." "" py_ver "${py_ver}")
        set(py_file "C:/Python${py_ver}/python.exe")
        if(EXISTS "${py_file}")
            message(STATUS "Found Python: ${py_file}")
            set(PYTHON_EXECUTABLE "${py_file}")
            set(PYTHONINTERP_FOUND TRUE)
            break()
        endif()
    endforeach()
else()
    foreach(py_ver ${PythonInterp_Versions})
        set(PythonInterp_FIND_VERSION ${py_ver})
        include(FindPythonInterp)
        if(PYTHONINTERP_FOUND)
            break()
        endif()
    endforeach()
endif()

if(NOT PYTHONINTERP_FOUND)
    message(FATAL_ERROR "Could NOT find suitable Python version (${PythonInterp_Versions})")
endif()

find_program(MTN NAMES "mtn" "mtn.exe" PATHS "/usr/local/bin" "C:/Program Files (x86)/monotone")
if(MTN)
    get_filename_component(MTN_PATH ${MTN} ABSOLUTE)
    get_filename_component(MTN_DIR ${MTN} PATH)
    set(ENV{PATH} "$ENV{PATH}${MONOTONE_PATH_SEP}${MTN_DIR}")
    message(STATUS "Found mtn: ${MTN_PATH}")
    mark_as_advanced(MTN)
    set(MTN_FOUND TRUE)
else()
    message(FATAL_ERROR "Could NOT find Monotone")
    set(MTN_FOUND FALSE)
endif()

if(NOT WIN32 AND NOT APPLE)
    find_program(DPKG NAMES "dpkg-deb" PATHS "/usr/bin")
    if(DPKG)
        get_filename_component(DPKG_PATH ${DPKG} ABSOLUTE)
        message(STATUS "Found dpkg-deb: ${DPKG_PATH}")
        set(DPKG_FOUND TRUE)
    else()
        message(STATUS "Could not find dpkg-deb; will not build Debian packages")
        set(DPKG_FOUND FALSE)
    endif()

    find_program(RPMBUILD NAMES "rpmbuild" PATHS "/usr/bin")
    if(RPMBUILD)
        get_filename_component(RPMBUILD_PATH ${RPMBUILD} ABSOLUTE)
        message(STATUS "Found rpmbuild: ${RPMBUILD_PATH}")
        set(RPMBUILD_FOUND TRUE)
    else()
        message(STATUS "Could not find rpmbuild; will not build RPM packages")
        set(RPMBUILD_FOUND FALSE)
    endif()
endif()

# --------------------------------------------------
# OTHER
# --------------------------------------------------

# Important for Windows
add_definitions(-DUNICODE)

configure_file("${CMAKE_SOURCE_DIR}/config.in.h" "${CMAKE_CURRENT_BINARY_DIR}/config.h")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.5)

# Set output directories for binaries and static and shared libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_JAVA_TARGET_OUTPUT_DIR   ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(doc)
add_subdirectory(lib/petroules-utilities-qt/src)
add_subdirectory(src/app)
add_subdirectory(src/lib/java)
add_subdirectory(src/lib/qt)

if(WIN32)
    add_subdirectory(src/launcher)
    if(MSVC_VERSION GREATER 1500)
        # TODO: temporarily disabled because it won't build as a Portable Library due to missing XML + Crypto classes
        #include_external_msproject(silverlockdotnet "${CMAKE_SOURCE_DIR}/src/lib/dotnet/SilverlockSharp.csproj" TYPE "FAE04EC0-301F-11D3-BF4B-00C04F79EFBC")
    endif()
elseif(APPLE)
    add_subdirectory(src/lib/objc)
endif()

# All of the metadata-type files in the repository root directory
add_custom_target("Meta" SOURCES CHANGELOG.md COPYING.md README.md)

# Configuration and utility scripts
file(GLOB_RECURSE scripts_SOURCES .travis/configure.sh .travis/make.sh .travis.yml .gitignore)
list(APPEND scripts_SOURCES ${CMAKE_SOURCE_DIR}/deploy) # TODO: DELETE ONCE CMAKE PACKAGING IS DONE
group_by_folder(${CMAKE_CURRENT_SOURCE_DIR} "${scripts_SOURCES}")
add_custom_target("Scripts" SOURCES ${scripts_SOURCES})

# All CMake scripts since some IDEs don't show the hierarchy
file(GLOB_RECURSE cmake_SOURCES ${CMAKE_SOURCE_DIR}/*/CMakeLists.txt ${CMAKE_SOURCE_DIR}/*.cmake)
group_by_folder(${CMAKE_CURRENT_SOURCE_DIR} "${cmake_SOURCES}")
add_custom_target("CMake" SOURCES ${cmake_SOURCES} config.in.h)

# Other shared resources (i.e. to be distributed in $prefix/share or elsewhere)
set(d ${CMAKE_SOURCE_DIR}/res)
file(GLOB_RECURSE shared_SOURCES ${d}/*.svg ${d}/*.ico ${d}/*.icns ${d}/*.png ${d}/*.qrc ${d}/*.txt ${d}/*.rtf)
group_by_folder(${d} "${shared_SOURCES}")
add_custom_target("Shared" SOURCES ${shared_SOURCES})

# Test files for the database engine
set(d ${CMAKE_SOURCE_DIR}/test-data)
file(GLOB_RECURSE tests_SOURCES ${d}/*.txt ${d}/*.xml ${d}/*.sdbx)
group_by_folder(${d} "${tests_SOURCES}")
add_custom_target("Tests" SOURCES ${tests_SOURCES})
