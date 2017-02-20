#include "../CommonHeader.h"

jmethodID findMethod(JNIEnv *env, jobjectArray methods, const char *name);

jmethodID findMethod(JNIEnv *env, jobject obj, const char *name, const char *sign, jboolean isStatic);

jobject invokeMethod(JNIEnv *env, jobject obj, const char *name, const char *sign, jboolean isStatic, jboolean isVoid, ...);

jobject invokeMethod(JNIEnv *env, const char *className, const char *name, const char *sign, jboolean isVoid, ...);

jobject invokeMethod(JNIEnv *env, jobject obj, jmethodID methodID, jboolean isVoid, jboolean isStatic, ...);

jobject invokeMethodV(JNIEnv *env, jobject obj, jmethodID methodID, jboolean isStatic, jboolean isVoid, va_list args);

jfieldID findField(JNIEnv *env, jobjectArray fields, const char *name);

jfieldID findField(JNIEnv *env, jobject obj, const char *name, const char *sign, jboolean isStatic);

jobject get(JNIEnv *env, jobject obj, const char *name, const char *sign, jboolean isStatic);

jobject get(JNIEnv *env, jobject obj, jfieldID, jboolean isStatic);

void set(JNIEnv *env, jobject obj, const char *name, const char *sign, jboolean isStatic, jobject value);

void set(JNIEnv *env, jobject obj, jfieldID, jboolean isStatic, jobject value);



