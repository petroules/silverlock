if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set(BOTAN_DEBUG "--enable-debug")
endif()

include(BotanUniversal)

if(CMAKE_TARGET_ARCHITECTURE_UNIVERSAL)
    botan_prepare_universal()
    set(BOTAN_CPU "--cpu=universal")
else()
    set(BOTAN_CPU "--cpu=${CMAKE_TARGET_ARCHITECTURES}")
endif()

if(ANDROID)
    set(BOTAN_OS "--os=android")
    set(BOTAN_CPU "--cpu=arm")
    set(BOTAN_OPTS "--no-autoload --enable-modules=auto_rng,pbkdf2,dev_random,sha1,aes,cbc,pkcs7")
elseif(WIN32 AND NOT MSVC)
    # Botan tends to detect OS as "Windows" even when not in MSVC shell
    set(BOTAN_OS "--os=mingw")
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
    if("${BOTAN_ARGS}\n${botan_version_file}" STREQUAL "${botan_args_file}")
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

if(CMAKE_TARGET_ARCHITECTURE_UNIVERSAL)
    botan_revert_universal()
endif()
