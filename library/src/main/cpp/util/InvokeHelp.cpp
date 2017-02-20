#include "InvokeHelp.h"
#include "Log.h"

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

jstring toString(JNIEnv *env, jobject obj) {
    jstring result = NULL;
    jclass clazz = env->FindClass("java/lang/Class");
    if (env->IsInstanceOf(obj, clazz)) {
        jmethodID methodID = env->GetMethodID(clazz, "toString", "()Ljava/lang/String;");
        result = (jstring) env->CallObjectMethod(obj, methodID);
    } else {
        result = (jstring) invokeMethod(env, obj, "toString", "()Ljava/lang/String;", false, false);
    }
    env->DeleteGlobalRef(clazz);
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

jmethodID findMethod(JNIEnv *env, jobject obj, const char *name, const char *sign, jboolean isStatic) {
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
        jclass clazz = NULL;
        if (isStatic) {
            clazz = (jclass) obj;
        } else {
            clazz = env->GetObjectClass(obj);
        }

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
    if (LOG && result == NULL) {
        jstring toString_ = toString(env, obj);
        const char *toString = env->GetStringUTFChars(toString_, 0);
        LOGI("Not found method %s in object %s", name, toString);
        env->ReleaseStringUTFChars(toString_, toString);
        env->DeleteGlobalRef(toString_);
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
    jmethodID methodID = findMethod(env, obj, name, sign, isStatic);
    if (methodID != NULL) {
        result = invokeMethodV(env, obj, methodID, isStatic, isVoid, args);
    }
    va_end(args);
    return result;
}

/**
 * 调用方法
 */
jobject invokeMethod(JNIEnv *env, const char *className, const char *name, const char *sign, jboolean isVoid, ...) {
    jclass clazz = env->FindClass(className);
    jobject result = NULL;
    va_list args;
    va_start(args, isVoid);
    jmethodID methodID = findMethod(env, clazz, name, sign, true);
    if (methodID != NULL) {
        result = invokeMethodV(env, clazz, methodID, true, isVoid, args);
    }
    va_end(args);
    env->DeleteLocalRef(clazz);
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
 * 从数组中查找相同名字的Field
 */
jfieldID findField(JNIEnv *env, jobjectArray fields, const char *name) {
    jfieldID fieldID = NULL;
    for (int i = 0, len = env->GetArrayLength(fields); i < len && fieldID == NULL; ++i) {
        jobject field = env->GetObjectArrayElement(fields, i);
        jstring fieldName = (jstring) invokeMethod(env, field, "getName", "()Ljava/lang/String;", false, false);
        const char *fieldName_ = env->GetStringUTFChars(fieldName, 0);
        if (stringEquals(name, fieldName_)) {
            fieldID = env->FromReflectedField(field);
        }
        env->ReleaseStringUTFChars(fieldName, fieldName_);
        env->DeleteLocalRef(fieldName);
        env->DeleteLocalRef(field);
    }
    return fieldID;
}


jfieldID findField(JNIEnv *env, jobject obj, const char *name, const char *sign, jboolean isStatic) {
    jfieldID result = NULL;
    if (sign != NULL && strlen(sign) > 0) {
        if (isStatic) {
            result = env->GetStaticFieldID((jclass) obj, name, sign);
        } else {
            jclass clazz = env->GetObjectClass(obj);
            result = env->GetFieldID(clazz, name, sign);
            env->DeleteLocalRef(clazz);
        }
    } else {
        //Class.getFields()方法
        jclass clazz = NULL;
        if (isStatic) {
            clazz = (jclass) obj;
        } else {
            clazz = env->GetObjectClass(obj);
        }

        jobjectArray publicFields = (jobjectArray) invokeMethod(env, clazz, "getFields", "()[Ljava/lang/reflect/Field;", false, false);
        result = findField(env, publicFields, name);
        env->DeleteLocalRef(publicFields);

        //Class.getDeclaredFields()方法
        if (result == NULL) {
            jobjectArray declareFields = (jobjectArray) invokeMethod(env, clazz, "getDeclaredFields", "()[Ljava/lang/reflect/Field;", false, false);
            result = findField(env, declareFields, name);
            env->DeleteLocalRef(declareFields);
        }

        env->DeleteLocalRef(clazz);
    }
    if (LOG && result == NULL) {
        jstring toString_ = toString(env, obj);
        const char *toString = env->GetStringUTFChars(toString_, 0);
        LOGI("Not found method %s in object %s", name, toString);
        env->ReleaseStringUTFChars(toString_, toString);
        env->DeleteGlobalRef(toString_);
    }
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    return result;
}

jobject get(JNIEnv *env, jobject obj, const char *name, const char *sign, jboolean isStatic) {
    jfieldID methodID = findField(env, obj, name, sign, isStatic);
    if (methodID == NULL) {
        return NULL;
    }
    return get(env, obj, methodID, isStatic);
}

jobject get(JNIEnv *env, jobject obj, jfieldID fieldID, jboolean isStatic) {
    jobject result = NULL;
    if (isStatic) {
        result = env->GetStaticObjectField((jclass) obj, fieldID);
    } else {
        result = env->GetObjectField(obj, fieldID);
    }
    return result;
}

void set(JNIEnv *env, jobject obj, const char *name, const char *sign, jboolean isStatic, jobject value) {
    jfieldID fieldID = findField(env, obj, name, sign, isStatic);
    if (fieldID != NULL) {
        set(env, obj, fieldID, isStatic, value);
    }
}

void set(JNIEnv *env, jobject obj, jfieldID fieldID, jboolean isStatic, jobject value) {
    if (isStatic) {
        env->SetStaticObjectField((jclass) obj, fieldID, value);
    } else {
        env->SetObjectField(obj, fieldID, value);
    }
}
