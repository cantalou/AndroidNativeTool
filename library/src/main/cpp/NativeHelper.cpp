#include <jni.h>
#include <string.h>
#include <string>
#include "util/MD5.h"


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


#ifdef __cplusplus
}
#endif