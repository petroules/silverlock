set(CMAKE_JAVA_INCLUDE_PATH "${SCPROV}" "${JZLIB}" "${JUNIT}" "${CMAKE_JAVA_TARGET_OUTPUT_DIR}/${java_TARGET_NAME}.jar")

add_jar("${java_TARGET_NAME}_test"
    ${ns}/DatabaseCryptoTest.java
    ${ns}/FilesystemAssetManager.java
    ${ns}/GenericAssetManager.java
    ${ns}/TestData.java
    ${ns}/TestDatabaseInfo.java
)
