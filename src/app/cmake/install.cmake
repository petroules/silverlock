if(IS_RELEASE_BUILD)
    install(TARGETS "${TARGET_NAME}"
        BUNDLE DESTINATION . COMPONENT Runtime
        RUNTIME DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_REL} COMPONENT Runtime
    )

    if(WIN32 OR APPLE)
        if(WIN32)
            set(ESUFFIX ${CMAKE_EXECUTABLE_SUFFIX})
        elseif(APPLE)
            set(ESUFFIX .app)
        endif()

        list(APPEND LIBDIRS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
        list(APPEND LIBDIRS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}")
        list(APPEND LIBDIRS "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
        list(APPEND LIBDIRS "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}")
        list(APPEND PLUGINS qtaccessiblewidgets qcncodecs qjpcodecs qkrcodecs qtwcodecs qsvgicon qgif qico qjpeg qmng qsvg qtiff)
        
        include(DeployQt4)
        install_qt4_executable("${TARGET_NAME}${ESUFFIX}" "${PLUGINS}" "" "${LIBDIRS}" "" "" Runtime)
    endif()
endif()
