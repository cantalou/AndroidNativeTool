#include <jni.h>
#include <string.h>
#include <string>
#include "util/MD5.h"
#include "util/InvokeHelp.h"


#ifdef __cplusplus
extern "C"
{
#endif

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

JNIEXPORT void JNICALL
Java_com_cantalou_android_nativeutil_NativeHelper_checkSign(JNIEnv *env, jclass type, jobject context) {

}

JNIEXPORT jobject JNICALL
Java_com_cantalou_android_nativeutil_NativeHelper_getValue(JNIEnv *env, jclass type, jobject obj, jstring name_, jstring sign_, jboolean isStatic) {
    const char *name = env->GetStringUTFChars(name_, 0);
    const char *sign = env->GetStringUTFChars(sign_, 0);

    jobject result = get(env, obj, name, sign, isStatic);

    env->ReleaseStringUTFChars(name_, name);
    env->ReleaseStringUTFChars(sign_, sign);
    return result;
}

JNIEXPORT jobject JNICALL
Java_com_cantalou_android_nativeutil_NativeHelper_setValue(JNIEnv *env, jclass type, jobject obj, jstring name_, jstring sign_, jboolean isStatic,
                                                           jobject value) {
    const char *name = env->GetStringUTFChars(name_, 0);
    const char *sign = env->GetStringUTFChars(sign_, 0);

    //set(env, obj, name, sign, isStatic, value);
    //LOGI("Java_com_cantalou_android_nativeutil_NativeHelper_setValue name:%s,sifn:%s", name, sign);

    env->ReleaseStringUTFChars(name_, name);
    env->ReleaseStringUTFChars(sign_, sign);
}

#ifdef __cplusplus
}
#endif