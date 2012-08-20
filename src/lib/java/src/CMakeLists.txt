set(CMAKE_JAVA_INCLUDE_PATH "${SCPROV}" "${JZLIB}" "${JUNIT}")

add_jar("${java_TARGET_NAME}"
    ${ns}/Database.java
    ${ns}/DatabaseCrypto.java
    ${ns}/DatabaseKeys.java
    ${ns}/DatabaseNode.java
    ${ns}/DatabaseReader.java
    ${ns}/DatabaseWriter.java
    ${ns}/EncryptedData.java
    ${ns}/Entry.java
    ${ns}/Group.java
    ${ns}/MACException.java
    ${ns}/SearchParameters.java

    com/petroules/BufferedReader2.java
    com/petroules/UnicodeReader.java
    com/petroules/Version.java
)

# create_javadoc converts all semicolons to colons
# which breaks the -classpath argument to javadoc.exe
# on Windows, so we can't build docs on Windows right now...
if(NOT WIN32)
    create_javadoc("${java_TARGET_NAME}"
        PACKAGES com.petroules com.petroules.silverlock
        SOURCEPATH "${CMAKE_CURRENT_SOURCE_DIR}"
        CLASSPATH ${CMAKE_JAVA_INCLUDE_PATH}
        INSTALLPATH "${CMAKE_BINARY_DIR}/share/javadoc"
        WINDOWTITLE "Silverlock Java Library"
        DOCTITLE "<h1>Silverlock Java Library</h1>"
        AUTHOR TRUE
        USE TRUE
        VERSION TRUE
    )
endif()

install_jar("${java_TARGET_NAME}" ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
