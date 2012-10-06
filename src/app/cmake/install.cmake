if(IS_RELEASE_BUILD)
    install(TARGETS "${TARGET_NAME}"
        BUNDLE DESTINATION . COMPONENT Runtime
        RUNTIME DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_REL} COMPONENT Runtime
    )

    if(WIN32 OR APPLE)
        install_qt4_executable("${TARGET_NAME}${CMAKE_USER_EXECUTABLE_SUFFIX}" "qtaccessiblewidgets qcncodecs qjpcodecs qkrcodecs qtwcodecs qsvgicon qgif qico qjpeg qmng qsvg qtiff" "" "${ALL_LIBRARY_DIRS}" "" "" Runtime)
    endif()
endif()
