#include <string.h>
#include <jni.h>
#include "InvokeHelp.h"
#include "Log.h"

#define LOG true
#define TAG "InvokeHelp"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)

/**
 * 判断字符串是否相等
 */
jboolean stringEquals(const char *str1, const char *str2) {
    if (strlen(str1) != strlen(str2)) {
        return false;
    }
    return strncmp(str1, str2, strlen(str1)) == 0;
}

jboolean stringEquals(JNIEnv *env, jstring str1, const char *str2) {
    const char *str1_ = env->GetStringUTFChars(str1, 0);
    jboolean result = stringEquals(str1_, str2);
    env->ReleaseStringUTFChars(str1, str1_);
    return result;
}

jmethodID getMethod(JNIEnv *env, jobject obj, const char *name, const char *sign, jboolean isStatic) {
    jmethodID result = NULL;
    if (sign != NULL && strlen(sign) > 0) {
        if (isStatic) {
            result = env->GetStaticMethodID((jclass) obj, name, sign);
        } else {
            jclass clazz = env->GetObjectClass(obj);
            result = env->GetMethodID(clazz, name, sign);
            env->DeleteLocalRef(clazz);
        }
    } else {

        //Class.getMethods()方法
        jclass clazz = env->GetObjectClass(obj);
        jobjectArray publicMethods = (jobjectArray) invokeMethod(env, clazz, "getMethods", "()[Ljava/lang/reflect/Method;", false, false);
        result = findMethod(env, publicMethods, name);
        env->DeleteLocalRef(publicMethods);

        //Class.getDeclaredMethods()方法
        if (result == NULL) {
            jobjectArray declareMethods = (jobjectArray) invokeMethod(env, clazz, "getDeclaredMethods", "()[Ljava/lang/reflect/Method;", false, false);
            result = findMethod(env, declareMethods, name);
            env->DeleteLocalRef(declareMethods);
        }

        env->DeleteLocalRef(clazz);
    }
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    return result;
}

/**
 * 调用方法
 */
jobject invokeMethod(JNIEnv *env, jobject obj, const char *name, const char *sign, jboolean isStatic, jboolean isVoid, ...) {
    jobject result = NULL;
    va_list args;
    va_start(args, isVoid);
    jmethodID methodID = getMethod(env, obj, name, sign, isStatic);
    if (methodID != NULL) {
        result = invokeMethodV(env, obj, methodID, isStatic, isVoid, args);
    } else {

    }
    va_end(args);
    return result;
}

/**
 * 调用方法
 */
jobject invokeMethod(JNIEnv *env, jobject obj, jmethodID methodID, jboolean isStatic, jboolean isVoid, ...) {
    jobject result = NULL;
    va_list args;
    va_start(args, isVoid);
    result = invokeMethodV(env, obj, methodID, isStatic, isVoid, args);
    va_end(args);
    return result;
}

/**
 * 调用方法
 */
jobject invokeMethodV(JNIEnv *env, jobject obj, jmethodID methodID, jboolean isStatic, jboolean isVoid, va_list args) {
    jobject result = NULL;
    if (isStatic) {
        if (isVoid) {
            env->CallStaticVoidMethodV((jclass) obj, methodID, args);
        } else {
            result = env->CallStaticObjectMethodV((jclass) obj, methodID, args);
        }
    } else {
        if (isVoid) {
            env->CallVoidMethodV(obj, methodID, args);
            return NULL;
        } else {
            result = env->CallObjectMethodV(obj, methodID, args);
        }
    }
    return result;
}

/**
 * 在方法数组中查找相同名称的对象
 */
jmethodID findMethod(JNIEnv *env, jobjectArray methods, const char *name) {
    jmethodID methodID = NULL;
    for (int i = 0, len = env->GetArrayLength(methods); i < len && methodID == NULL; ++i) {
        jobject method = env->GetObjectArrayElement(methods, i);
        jstring methodName = (jstring) invokeMethod(env, method, "getName", "()Ljava/lang/String;", false, false);
        const char *methodName_ = env->GetStringUTFChars(methodName, 0);
        if (stringEquals(name, methodName_)) {
            methodID = env->FromReflectedMethod(method);
        }
        env->ReleaseStringUTFChars(methodName, methodName_);
        env->DeleteLocalRef(methodName);
        env->DeleteLocalRef(method);
    }
    return methodID;
}
