set(BOTAN_SOURCE_DIR "${CMAKE_SOURCE_DIR}/lib/botan")
set(BOTAN_ARCH_FILE "${BOTAN_SOURCE_DIR}/src/build-data/arch/universal.txt")
set(BOTAN_CC_FILES "${BOTAN_SOURCE_DIR}/src/build-data/cc/gcc.txt" "${BOTAN_SOURCE_DIR}/src/build-data/cc/clang.txt")

# Create a string with flags for each architecture (will work out to -arch i386 -arch x86_64 pretty much all the time)
foreach(osx_arch ${CMAKE_TARGET_ARCHITECTURES})
    set(BOTAN_CC_DATA_ARCHS "${BOTAN_CC_DATA_ARCHS} -arch ${osx_arch}")
endforeach()

# Create the full target configuration string
set(BOTAN_CC_DATA "universal -> \"-force_cpusubtype_ALL -mmacosx-version-min=10.4 ${BOTAN_CC_DATA_ARCHS}\"\n")

function(botan_prepare_universal)
    # Create empty file "universal" architecture file
    file(WRITE "${BOTAN_ARCH_FILE}" "")
    
    # Edit the compiler files
    foreach(f ${BOTAN_CC_FILES})
        file(READ "${f}" contents)
        
        string(FIND "${contents}" "${BOTAN_CC_DATA}" already_has)
        
        if(already_has EQUAL -1)
            string(REPLACE "</mach_abi_linking>" "${BOTAN_CC_DATA}</mach_abi_linking>" contents "${contents}")
            file(WRITE "${f}" "${contents}")
        endif()
    endforeach()
endfunction()

function(botan_revert_universal)
    if(EXISTS "${BOTAN_ARCH_FILE}")
        file(REMOVE "${BOTAN_ARCH_FILE}")
    endif()
    
    # Revert the changes to the compiler files
    foreach(f ${BOTAN_CC_FILES})
        file(READ "${f}" contents)
        string(REPLACE "${BOTAN_CC_DATA}" "" contents "${contents}")
        file(WRITE "${f}" "${contents}")
    endforeach()
endfunction()
