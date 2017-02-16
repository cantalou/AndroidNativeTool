#include <jni.h>
#include <string>
#include "util/InvokeHelp.h"
#include "util/MD5.h"

#define  TAG    "InvokeHelp"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)

extern "C"

JNIEXPORT jobject JNICALL
Java_com_cantalou_android_nativeutil_NativeHelper_test(JNIEnv *env, jclass jclazz, jobject obj, jstring name_, jstring sign_) {
    const char *sign = sign_ != NULL ? env->GetStringUTFChars(sign_, 0) : NULL;
    jobject result = invoke(env, obj, env->GetStringUTFChars(name_, 0), sign);
    return result;
}


/**
 * 生成MD5摘要
 */
JNIEXPORT jstring JNICALL
Java_com_cantalou_android_nativeutil_NativeHelper_MD5(JNIEnv *env, jclass jclazz, jstring jcontent) {

    char *jstr = (char *) env->GetStringUTFChars(jcontent, 0);
    if (jstr == NULL) {
        return NULL;
    }

    unsigned char dest[16] = {0};
    MD5_CTX context = {0};
    MD5Init(&context);
    MD5Update(&context, (unsigned char *) jstr, strlen(jstr));
    MD5Final(dest, &context);
    env->ReleaseStringUTFChars(jcontent, jstr);

    char destination[32] = {0};
    int i;
    for (i = 0; i < 16; i++) {
        sprintf(destination, "%s%02X", destination, dest[i]);
    }
    return env->NewStringUTF(destination);
}

