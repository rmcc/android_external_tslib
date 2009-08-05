#My Own TSLIB Make File
#There are some variables or MACROS in the configure / Makefile which are needed
#How do I get those into this mk file?

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

#May need to add flags from makefile which are generated from .configure
#LOCAL_CFLAGS += -PLUGIN_DIR -TS_CONF
LOCAL_CFLAGS += -DDEBUG

LOCAL_C_INCLUDES := \
	external/tslib/ \
	external/tslib/src \
	external/tslib/android

LOCAL_SRC_FILES := \
	ts_attach.c ts_close.c ts_config.c \
	ts_fd.c ts_load_module.c ts_open.c ts_read.c \
	ts_parse_vars.c ts_read_raw_module.c ts_read_raw.c ts_error.c \
	../android/CalibrateTouchScreen.c

LOCAL_MODULE := libtslib

LOCAL_SHARED_LIBRARIES := libdl libcutils

LOCAL_C_INCLUDES += dalvik/libnativehelper/include/nativehelper

LOCAL_LDLIBS += -lpthread

LOCAL_PRELINK_MODULE := false
include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_STATIC_LIBRARY)

# ------------------------------------------------
# ----------- For TS_PRINT ---------------------
# ------------------------------------------------

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
	external/tslib/ \
	external/tslib/src \
	external/tslib/tests/ \
	external/tslib/android

LOCAL_SRC_FILES := ../tests/ts_print.c

LOCAL_MODULE := tsprint

LOCAL_SHARED_LIBRARIES := libtslib libdl

#include $(BUILD_EXECUTABLE)

# ------------------------------------------------
# ----------- For TS_TEST ------------------------
# ------------------------------------------------

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
	external/tslib/ \
	external/tslib/src \
	external/tslib/tests/ \
	external/tslib/android

LOCAL_SRC_FILES := ../tests/ts_test.c

LOCAL_MODULE := tstest

LOCAL_SHARED_LIBRARIES := libtslib libdl libcutils

#include $(BUILD_EXECUTABLE)

# ------------------------------------------------
# ----------- For TEST_UTILS ---------------------
# ------------------------------------------------

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
	external/tslib/ \
	external/tslib/src \
	external/tslib/tests/ \
	external/tslib/android

LOCAL_SRC_FILES := ../tests/testutils.c

LOCAL_MODULE := tsutils

LOCAL_SHARED_LIBRARIES := libtslib libdl

#include $(BUILD_EXECUTABLE)


# ------------------------------------------------
# ----------- For TS_CALIBRATE -------------------
# ------------------------------------------------

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
	external/tslib/ \
	external/tslib/src \
	external/tslib/tests/ \
	external/tslib/android

LOCAL_C_INCLUDES += dalvik/libnativehelper/include/nativehelper

LOCAL_SRC_FILES := ../android/CalibrateTouchScreen.c

LOCAL_MODULE := tscalib

LOCAL_SHARED_LIBRARIES := libtslib libdl libcutils

#include $(BUILD_EXECUTABLE)

#For some reason it doesn't even go to ETC just the directory above it
#Reason is that files are in the parent direction so this gets put into parent of ETC
$(call add-prebuilt-files, ETC, ../ts.conf)
#$(call add-prebuilt-files, ETC, ../pointercal)
#$(call add-prebuilt-files, ETC, ../pointercaldefault)
