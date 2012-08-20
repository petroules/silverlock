if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set(BOTAN_DEBUG "--enable-debug")
endif()

if(CMAKE_TARGET_ARCHITECTURE_UNIVERSAL)
    set(BOTAN_ARCH_FILE "${CMAKE_SOURCE_DIR}/lib/botan/src/build-data/arch/universal.txt")
    set(BOTAN_CC_FILES
        "${CMAKE_SOURCE_DIR}/lib/botan/src/build-data/cc/gcc.txt"
        "${CMAKE_SOURCE_DIR}/lib/botan/src/build-data/cc/clang.txt"
    )

    # create empty file
    file(WRITE "${BOTAN_ARCH_FILE}" "")

    set(BOTAN_CC_DATA "universalbinary -> \"-force_cpusubtype_ALL -mmacosx-version-min=10.4")
    foreach(osx_arch ${CMAKE_TARGET_ARCHITECTURES})
        set(BOTAN_CC_DATA "${BOTAN_CC_DATA} -arch ${osx_arch}")
    endforeach()

    # Closing quote
    set(BOTAN_CC_DATA "${BOTAN_CC_DATA}\"")

    # Edit the compiler files
    foreach(f ${BOTAN_CC_FILES})
        file(READ "${f}" contents)
        string(REPLACE "</mach_abi_linking>" "${BOTAN_CC_DATA}\n</mach_abi_linking>" contents "${contents}")
        file(WRITE "${f}" "${contents}")
    endforeach()
else()
    if("${CMAKE_TARGET_ARCHITECTURES}" STREQUAL "i386")
        set(BOTAN_CPU "--cpu=i686") # Botan likes i686 for Intel 32-bit instead of i386
    else()
        set(BOTAN_CPU "--cpu=${CMAKE_TARGET_ARCHITECTURES}")
    endif()
endif()

if(ANDROID)
    set(BOTAN_OS "--os=android")
    set(BOTAN_CPU "--cpu=arm")
    set(BOTAN_OPTS "--no-autoload --enable-modules=auto_rng,pbkdf2,dev_random,sha1,aes,cbc,pkcs7")
elseif(WIN32)
    if(MSVC)
        set(BOTAN_OS "--os=windows")
    else()
        set(BOTAN_OS "--os=mingw")
    endif()
elseif(APPLE)
    set(BOTAN_OS "--os=macosx")
else()
    # Linux and other OSes can probably handle themselves...
endif()

# Don't quote this
set(BOTAN_ARGS ${BOTAN_OS} ${BOTAN_CPU} --disable-shared --disable-asm --gen-amalgamation --with-build-dir=${CMAKE_CURRENT_BINARY_DIR}/botan ${BOTAN_OPTS} ${BOTAN_DEBUG})

file(READ "${CMAKE_SOURCE_DIR}/lib/botan/botan_version.py" botan_version_file)
set(BOTAN_ARGFILE "${CMAKE_CURRENT_BINARY_DIR}/botan/args.txt")

if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/botan")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/botan")
endif()

# By default we always want to run the Botan configure program
set(botan_reconfigure TRUE)

# However if a file exists in the configure directory with the
# exact same configure options as we have in CMakeLists, then
# Botan has already been configured and we need not do it again
if(EXISTS "${BOTAN_ARGFILE}")
    file(READ "${BOTAN_ARGFILE}" botan_args_file)
    if("${BOTAN_ARGS}" STREQUAL "${botan_args_file}\n${botan_version_file}")
        set(botan_reconfigure FALSE)
    endif()
endif()

if(botan_reconfigure)
    message(STATUS "Configuring Botan with arguments: ${BOTAN_ARGS}")

    execute_process(
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/botan"
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/lib/botan/configure.py ${BOTAN_ARGS}
        RESULT_VARIABLE botan_configure
        OUTPUT_VARIABLE botan_configure_results
        ERROR_VARIABLE botan_configure_results
    )

    if("${botan_configure}" STREQUAL "0")
        file(WRITE "${BOTAN_ARGFILE}" "${BOTAN_ARGS}\n${botan_version_file}")
    else()
        file(REMOVE "${BOTAN_ARGFILE}")
        message(FATAL_ERROR "${botan_configure} ${botan_configure_results}")
    endif()
endif()

if(CMAKE_TARGET_ARCHITECTURE_UNIVERSAL AND EXISTS "${BOTAN_ARCH_FILE}")
    file(REMOVE "${BOTAN_ARCH_FILE}")
endif()
