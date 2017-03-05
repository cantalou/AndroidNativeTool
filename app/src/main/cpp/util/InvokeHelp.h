#include "../CommonHeader.h"

jmethodID findMethod(JNIEnv *env, jobjectArray methods, const char *name);

jmethodID findMethod(JNIEnv *env, jobject obj, const char *name, const char *sign, jboolean isStatic);

void *invokeMethod(JNIEnv *env, jobject obj, const char *name, const char *sign, jboolean isStatic, jboolean isVoid, ...);

void *invokeMethod(JNIEnv *env, const char *className, const char *name, const char *sign, jboolean isVoid, ...);

void *invokeMethod(JNIEnv *env, jobject obj, jmethodID methodID, const char type, jboolean isStatic, ...);

void *invokeMethodV(JNIEnv *env, jobject obj, jmethodID methodID, const char type, jboolean isStatic, va_list args);

jfieldID findField(JNIEnv *env, jobjectArray fields, const char *name);

jfieldID findField(JNIEnv *env, jobject obj, const char *name, const char *sign, jboolean isStatic);

void *get(JNIEnv *env, jobject obj, const char *name, const char *sign, jboolean isStatic);

void *get(JNIEnv *env, jobject obj, jfieldID, const char type, jboolean isStatic);

void set(JNIEnv *env, jobject obj, const char *name, const char *sign, jboolean isStatic, jobject value);

void set(JNIEnv *env, jobject obj, jfieldID, jboolean isStatic, jobject value);



