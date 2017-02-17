#include "../CommonHeader.h"

jmethodID findMethod(JNIEnv *env, jobjectArray methods, const char *name);

jmethodID getMethod(JNIEnv *env, jobject obj, const char *name, const char *sign, jboolean isStatic);

jobject invokeMethod(JNIEnv *env, jobject obj, const char *name, const char *sign, jboolean isStatic, jboolean isVoid, ...);

jobject invokeMethod(JNIEnv *env, jobject obj, jmethodID methodID, jboolean isVoid, jboolean isStatic, ...);

jobject invokeMethodV(JNIEnv *env, jobject obj, jmethodID methodID, jboolean isStatic, jboolean isVoid, va_list args);


