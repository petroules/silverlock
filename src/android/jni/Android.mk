LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_LDLIBS := -llog
LOCAL_MODULE := native
LOCAL_C_INCLUDES := botan_all.h
LOCAL_SRC_FILES := native.cpp botan_all.cpp

# -marm prevents some assembler compilation errors that show up when using Botan SHA1 module (possibly others?)
LOCAL_CPPFLAGS += -O3 -finline-functions -D_REENTRANT -Wno-long-long -fpermissive -W -Wall -marm
LOCAL_CPP_FEATURES += exceptions rtti
include $(BUILD_SHARED_LIBRARY)