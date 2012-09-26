set(app_ext "${CMAKE_EXECUTABLE_SUFFIX}")
set(app_dest_dir bin/)
set(plugin_dest_dir bin)
set(qtconf_dest_dir bin)

if(APPLE)
    set(plugin_dest_dir "${TARGET_NAME}.app/Contents/MacOS")
    set(qtconf_dest_dir "${TARGET_NAME}.app/Contents/Resources")
    set(app_dest_dir "")
    set(app_ext ".app")
endif()

# for NSIS to create Start Menu shortcuts
set(CPACK_PACKAGE_EXECUTABLES "${TARGET_NAME};${CMAKE_PROJECT_NAME}")

# ------------------------
# Various install commands
# ------------------------

# The app gets installed in the root of the installation dir on OS X, and bin elsewhere
install(TARGETS "${TARGET_NAME}"
    BUNDLE DESTINATION . COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime
)

# Install Qt plugins - cull using 'REGEX "..." EXCLUDE'
install(DIRECTORY "${QT_PLUGINS_DIR}/imageformats" DESTINATION ${plugin_dest_dir}/plugins COMPONENT Runtime)

# Install qt.conf
install(CODE "
    file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"\")
    " COMPONENT Runtime)

list(APPEND LIBDIRS "${QT_BINARY_DIR}") # DLLs are in the bin directory on Windows
list(APPEND LIBDIRS "${QT_LIBRARY_DIR}")
list(APPEND LIBDIRS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
list(APPEND LIBDIRS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}")
list(APPEND LIBDIRS "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
list(APPEND LIBDIRS "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}")

# Install dependencies
install(CODE "
    file(GLOB_RECURSE QTPLUGINS
      \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
    include(BundleUtilities)
    fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/${app_dest_dir}${TARGET_NAME}${app_ext}\" \"\${QTPLUGINS}\" \"${LIBDIRS}\")
    " COMPONENT Runtime)
