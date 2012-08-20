set(TARGET_NAME "silverlock-launcher")

include_directories("${CMAKE_CURRENT_BINARY_DIR}/../app")

add_executable("${TARGET_NAME}"
    main.cpp
    windowscertificatevalidator.cpp
    windowscertificatevalidator.h

    ../app/silverlock.rc.txt
)

target_link_libraries("${TARGET_NAME}" crypt32 ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARY})
