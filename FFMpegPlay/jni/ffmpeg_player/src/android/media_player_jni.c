#include <stdio.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include <jni.h>
#include <android/log.h>

#define TAG "Andy_log"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG , TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO , TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN , TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR , TAG, __VA_ARGS__)


#ifdef __cplusplus
extern "C" {
#endif

# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))

static jstring m_jni_native_getConfigure(JNIEnv *env, jobject thiz)
{
    LOGE("GET configuration = %s", avcodec_configuration());
    LOGE("get configure begin");

    char info[10000] = { 0 };
    sprintf(info, "info: %s\n", avcodec_configuration());
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

