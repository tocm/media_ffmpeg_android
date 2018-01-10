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
