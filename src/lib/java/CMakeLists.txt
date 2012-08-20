if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} VERSION_GREATER 2.8.5)
    find_package(Java)
    if(JAVA_FOUND)
        include(UseJava)

        find_jar(JUNIT NAMES junit PATHS "${CMAKE_SOURCE_DIR}/lib" VERSIONS 4.10 DOC "JUnit testing framework JAR")
        find_jar(JZLIB NAMES jzlib PATHS "${CMAKE_SOURCE_DIR}/lib" VERSIONS 1.1.1 DOC "Java Zlib compression library JAR")
        find_jar(SCPROV NAMES scprov-jdk15-1.46.99.3-UNOFFICIAL-ROBERTO-RELEASE PATHS "${CMAKE_SOURCE_DIR}/lib" DOC "Spongy Castle Java crypto library JAR")

        if(NOT JUNIT)
            message(FATAL_ERROR "Could NOT find JUnit library")
        endif()

        if(NOT JZLIB)
            message(FATAL_ERROR "Could NOT find jzlib library")
        endif()

        if(NOT SCPROV)
            message(FATAL_ERROR "Could NOT find Spongy Castle library")
        endif()

        # Main namespace for classes
        set(ns com/petroules/silverlock)

        set(java_TARGET_NAME "silverlockjava")

        add_subdirectory(src)
        add_subdirectory(test)
    else()
        message(STATUS "Could NOT find Java; will not compile Java library")
    endif()
else()
    message(STATUS "Skipping Java library compilation due to outdated CMake version")
endif()
