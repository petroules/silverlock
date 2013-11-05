add_subdirectory(lib)
add_subdirectory(app)

if(WIN32 AND MSVC_VERSION GREATER 1500)
    # TODO: temporarily disabled because it won't build as a Portable Library due to missing XML + Crypto classes
    #include_external_msproject(silverlockdotnet "${CMAKE_SOURCE_DIR}/src/lib/dotnet/SilverlockSharp.csproj" TYPE "FAE04EC0-301F-11D3-BF4B-00C04F79EFBC")
endif()
