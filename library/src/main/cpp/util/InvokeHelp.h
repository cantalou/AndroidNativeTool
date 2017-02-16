#include <jni.h>

/**
 * 调用静态函数
 */
jobject invoke_static(JNIEnv *env, jclass jclazz, jstring method_name, jstring sign);

/**
 * 调用静态函数
 */
jobject invoke(JNIEnv *env, jobject obj, const char *name, const char *sign, ...);

/**
 * 调用实例函数
 */
jobject invoke_virtual(JNIEnv *env, jobject obj, jstring method_name, jstring sign);