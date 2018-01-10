# media_ffmpeg_android
How to build the ffmpeg shared library on android platform

（1）编译FFmpeg类库 【ubuntu】

a) 下载安装NDK
下载NDK之后直接解压缩就可以使用了。在Windows下使用的时候需要用到Cygwin。在这里我自己使用Linux编译类库。
注意：
    1. 试了WIN7 + cgywin 遇到只能生成静态库，一直不能生成动态so 库，问题原因未找到。
    2. 试VM+ ubuntukylin-17.04-desktop-amd64 + ffmpeg-3.4.1.tar 最新版，可以编成功.


b) 修改FFmpeg的configure
下载FFmpeg源代码之后，首先需要对源代码中的configure文件进行修改。由于编译出来的动态库文件名的版本号在.so之后（例如“libavcodec.so.5.100.1”），而android平台不能识别这样文件名，所以需要修改这种文件名。在configure文件中找到下面几行代码：
[plain] view plain copy

	1. SLIBNAME_WITH_MAJOR='$(SLIBNAME).$(LIBMAJOR)'  
	2. LIB_INSTALL_EXTRA_CMD='$$(RANLIB)"$(LIBDIR)/$(LIBNAME)"'  
	3. SLIB_INSTALL_NAME='$(SLIBNAME_WITH_VERSION)'  
	4. SLIB_INSTALL_LINKS='$(SLIBNAME_WITH_MAJOR)$(SLIBNAME)'  

替换为下面内容就可以了：
[plain] view plain copy

	1. SLIBNAME_WITH_MAJOR='$(SLIBPREF)$(FULLNAME)-$(LIBMAJOR)$(SLIBSUF)'  
	2. LIB_INSTALL_EXTRA_CMD='$$(RANLIB)"$(LIBDIR)/$(LIBNAME)"'  
	3. SLIB_INSTALL_NAME='$(SLIBNAME_WITH_MAJOR)'  
	4. SLIB_INSTALL_LINKS='$(SLIBNAME)'  

configure是一个ffmpeg 自带shell脚本，根据命令行参数不同来裁剪模块，生成特定的config.h文件，这是关键的一步【参看C)步骤】。

c) 先进入ffmpeg 目录下执行./configure 
andy@ubuntu:~/workspace/media/ffmpeg_android/ffmpeg$ ./configure

d) 编写Android so动态库的config 脚本
注意：
	1. 用UE或EditPlus等文本编辑软件新建一个build_android_lib.sh文件（千万别用系统的notepad，因为windows和Linux的换行方式不一样），并在系统环境变量里面设置NDK_ROOT.
	2. 编写config脚本时候，如果使用window cygwin 来编译，其中的路径需要使用windows形式的路径，不能使用/cygwindriver/d/android 这种格式的路径。
          例如：export NDK="D:/android-ndk-work-folder/android-ndk-r10c"


1. 以下是ubuntu 下的执行脚本文件，创建在ffmpeg 的上一层目录下。然后执行如下命令，成功执行完会生成lib, include 目录

andy@ubuntu:~/workspace/media/ffmpeg_android$ ls
build_android_lib.sh  fflib  ffmpeg
andy@ubuntu:~/workspace/media/ffmpeg_android$ sudo ./build_android_lib.sh

build_android_lib.h 文件内容如下：
#!/bin/bash
cd ffmpeg
make clean
export NDK=/home/andy/workspace/tools/android-ndk-r14b
export PREBUILT=$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt
export PLATFORM=$NDK/platforms/android-9/arch-arm
export PREFIX=../fflib
build_lib(){
  ./configure --disable-x86asm \
--prefix=$PREFIX \
--target-os=linux \
--arch=arm \
--sysroot=$PLATFORM \
--cc=$PREBUILT/linux-x86_64/bin/arm-linux-androideabi-gcc \
--nm=$PREBUILT/linux-x86_64/bin/arm-linux-androideabi-nm \
--cross-prefix=$PREBUILT/linux-x86_64/bin/arm-linux-androideabi- \
--enable-shared \
--enable-runtime-cpudetect \
--enable-cross-compile \
--enable-runtime-cpudetect \
--enable-gpl \
--enable-small \
--disable-static \
--disable-asm \
--disable-stripping \
--disable-ffprobe \
--disable-ffplay \
--disable-ffmpeg \
--disable-ffserver \
--disable-debug \
--disable-linux-perf \
--extra-cflags="-fPIC -DANDROID -D__thumb__ -mthumb -Wfatal-errors -Wno-deprecated -mfloat-abi=softfp -marm -march=armv7-a"
}
build_lib
make
make install
cd ..


（2）编写JNI 文件 【C 文件或者CPP】
注意：如果选择CPP文件，不能直接调用ffmpeg 接口函数，需要添加一个C文件做中间层，只限于C文件调用ffmpeg 接口。

C 文件实现：
#include <stdio.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include <jni.h>
#include <android/log.h>
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "(>_<)", format, ##__VA_ARGS__)
#ifdef __cplusplus
extern "C" {
#endif
# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
static jstring m_jni_native_getConfigure(JNIEnv *env, jobject thiz)
{
    //LOGE("GET configuration = %s", avcodec_configuration());
    LOGE("get configure begin");
    char info[10000] = { 0 };
    sprintf(info, "%s\n", avcodec_configuration());
    return (*env)->NewStringUTF(env, info);
}
static JNINativeMethod gMethods[] = {
    { "native_getConfigure", "()Ljava/lang/String;", (void*)m_jni_native_getConfigure }
};
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;
    jint retGvm;
    LOGE("--------JNI_Onload ----------");
       if ((*vm)->GetEnv(vm,(void**) &env, JNI_VERSION_1_4) != JNI_OK)
   // if (vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK)
    {
        return result;
    }
       jclass clazz = (*env)->FindClass(env,"com/andy/ffmpegplayer/jni/MediaJni");
   // jclass clazz = (env)->FindClass("com/andy/ffmpegplayer/jni/MediaJni");
    if (!clazz)
    {
    }
       // if (env->RegisterNatives(clazz, gMethods, sizeof(gMethods) / sizeof(gMethods[0])) < 0)
       if((*env)->RegisterNatives(env, clazz, gMethods, NELEM(gMethods)) < 0)
    {
        LOGE("m_upjni_initContext Failed");
        return -1;
    }
    //retGvm = env->GetJavaVM(&m_upjni_jvm);
    result = JNI_VERSION_1_4;
    return result;
}
void JNI_OnUnload(JavaVM* vm, void* reserved)
{
    LOGE("--------JNI_OnUnload ----------");
    return;
}
#ifdef __cplusplus
};
#endif

C++文件实现：
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <jni.h>
#include <android/native_window_jni.h>
#include "media_jni.h"
#include "um_engine.h"

static jstring m_jni_native_getConfigure(JNIEnv *env, jobject thiz)
{
    char info[10000] = { 0 };
    LOGE("GET ffmpeg av configuration (JNI(CPP) -> C -> LIB)BEGIN");
    sprintf(info, "%s %s\n", "JNI(CPP) -> C -> LIB \n", um_engine_getConfigure());
    return env->NewStringUTF(info);
}
static JNINativeMethod gMethods[] = {
    { "native_getConfigure", "()Ljava/lang/String;", (void*)m_jni_native_getConfigure }
};
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;
    jint retGvm;
    if (vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK)
    {
        return result;
    }
    jclass clazz = (env)->FindClass("com/andy/ffmpegplayer/jni/MediaJni");
    if (!clazz)
    {
    }
    if (env->RegisterNatives(clazz, gMethods, sizeof(gMethods) / sizeof(gMethods[0])) < 0)
    {
        // LOG_ERR("m_upjni_initContext Failed");
        return -1;
    }
    //retGvm = env->GetJavaVM(&m_upjni_jvm);
    result = JNI_VERSION_1_4;
    return result;
}
void JNI_OnUnload(JavaVM* vm, void* reserved)
{
    return;
}

For C++ 的中间层C文件
#include <stdio.h>
#include "um_engine.h"
const char* um_engine_getConfigure()
{
   
    return  (const char*)avcodec_configuration();
    
}


（3）编写Android.mk 和 Application.mk 文件
关于引入外部动态库，有两种方式：
方式一：
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

方式二：
LOCAL_PATH := $(call my-dir)
MY_PORT_PATH := ${LOCAL_PATH}/ffmpeg_player/src/android
MY_INCLUDES_PATH := ${LOCAL_PATH}/ffmpeg_player/src/android/ffmpeg/include
MY_L_LIBS_PATH := ${LOCAL_PATH}/ffmpeg_player/src/android/ffmpeg/libs
# FFmpeg library
include $(CLEAR_VARS)
LOCAL_MODULE := avcodec
LOCAL_SRC_FILES := ${MY_L_LIBS_PATH}/libavcodec-57.so
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := avdevice
LOCAL_SRC_FILES := ${MY_L_LIBS_PATH}/libavdevice-57.so
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := avfilter
LOCAL_SRC_FILES := ${MY_L_LIBS_PATH}/libavfilter-6.so
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := avformat
LOCAL_SRC_FILES := ${MY_L_LIBS_PATH}/libavformat-57.so
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := avutil
LOCAL_SRC_FILES := ${MY_L_LIBS_PATH}/libavutil-55.so
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := postproc
LOCAL_SRC_FILES := ${MY_L_LIBS_PATH}/libpostproc-54.so
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := swresample
LOCAL_SRC_FILES := ${MY_L_LIBS_PATH}/libswresample-2.so
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := swscale
LOCAL_SRC_FILES := ${MY_L_LIBS_PATH}/libswscale-4.so
include $(PREBUILT_SHARED_LIBRARY)
# Program
include $(CLEAR_VARS)
LOCAL_MODULE := mediaJni
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS := -D__STDC_CONSTANT_MACROS
LOCAL_CPP_EXTENSION := cpp
LOCAL_SRC_FILES := ${MY_PORT_PATH}/ffmpeg_player_jni.c
LOCAL_C_INCLUDES += $(MY_INCLUDES_PATH)
LOCAL_LDLIBS := -lz -llog -landroid
LOCAL_SHARED_LIBRARIES := avcodec avdevice avfilter avformat avutil postproc swresample swscale
include $(BUILD_SHARED_LIBRARY)

Application.mk
APP_ABI := all
APP_PLATFORM=android-18




