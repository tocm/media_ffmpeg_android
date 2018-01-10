LOCAL_PATH := $(call my-dir)
MY_CORE_PATH := $(LOCAL_PATH)/ffmpeg_player/src/core
MY_PORT_PATH := $(LOCAL_PATH)/ffmpeg_player/src/android
MY_L_LIBS_PATH := $(LOCAL_PATH)/ffmpeg_player/src/android/ffmpeg/libs

include $(CLEAR_VARS)
LOCAL_MODULE := mediaJni
LOCAL_ARM_MODE := arm
LOCAL_LDLIBS := -llog
LOCAL_CXXFLAGS := -DHAVE_PTHREADS -w -DENABLE_LOG
$(info ${MY_L_LIBS_PATH})

OEM_LDLIBS += -L$(MY_L_LIBS_PATH) -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lpostproc -lswresample -lswscale
$(info ${LOCAL_LDLIBS})

MY_LOCAL_C_INCLUDES := $(MY_PORT_PATH) \
	$(MY_CORE_PATH) \
	$(MY_PORT_PATH)/ffmpeg/include 

LOCAL_C_INCLUDES := $(MY_LOCAL_C_INCLUDES)

MY_SRC_FILES := $(MY_PORT_PATH)/media_jni.cpp \
	$(MY_CORE_PATH)/um_engine.c

LOCAL_SRC_FILES := $(MY_SRC_FILES)
LOCAL_LDLIBS += $(OEM_LDLIBS)
include $(BUILD_SHARED_LIBRARY)
#$(call import-module,cpufeatures)
