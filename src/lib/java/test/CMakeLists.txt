get_target_property(JAVALIB_JAR "${java_TARGET_NAME}" JAR_FILE)

set(CMAKE_JAVA_INCLUDE_PATH "${SCPROV}" "${JZLIB}" "${JUNIT}" "${JAVALIB_JAR}")

add_jar("${java_TARGET_NAME}_test"
    ${ns}/DatabaseCryptoTest.java
    ${ns}/FilesystemAssetManager.java
    ${ns}/GenericAssetManager.java
    ${ns}/TestData.java
    ${ns}/TestDatabaseInfo.java
)

add_dependencies("${java_TARGET_NAME}_test" "${java_TARGET_NAME}")
