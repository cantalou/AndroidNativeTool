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
void *invokeMethod(JNIEnv *env, jobject obj, const char *name, const char *sign, jboolean isStatic, jboolean isVoid, ...) {
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
void *invokeMethod(JNIEnv *env, const char *className, const char *name, const char *sign, jboolean isVoid, ...) {
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
void *invokeMethod(JNIEnv *env, jobject obj, jmethodID methodID, const char type, jboolean isStatic, jboolean isVoid, ...) {
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
    void *result = NULL;

    if (isVoid) {
        if (isStatic) {
            env->CallStaticVoidMethodV((jclass) obj, methodID, args);
        } else {
            env->CallVoidMethodV(obj, methodID, args);
        }
    } else {
        if (isStatic) {
            result = env->CallStaticObjectMethodV((jclass) obj, methodID, args);
        } else {
            result = env->CallObjectMethodV(obj, methodID, args);
        }
    }


    switch (type) {
        case 'Z': {
            if (isStatic) {
                jboolean value = env->GetStaticBooleanField((jclass) obj, fieldID);
                result = &value;
            } else {
                jboolean value = env->GetBooleanField(obj, fieldID);
                result = &value;
            }
            break;
        }
        case 'B': {
            if (isStatic) {
                jbyte value = env->GetStaticByteField((jclass) obj, fieldID);
                result = &value;
            } else {
                jbyte value = env->GetByteField(obj, fieldID);
                result = &value;
            }
            break;
        }
        case 'C': {
            if (isStatic) {
                jchar value = env->GetStaticCharField((jclass) obj, fieldID);
                result = &value;
            } else {
                jchar value = env->GetCharField(obj, fieldID);
                result = &value;
            }
            break;
        }
        case 'S': {
            if (isStatic) {
                jshort value = env->GetStaticShortField((jclass) obj, fieldID);
                result = &value;
            } else {
                jshort value = env->GetShortField(obj, fieldID);
                result = &value;
            }
            break;
        }
        case 'I': {
            if (isStatic) {
                jint value = env->GetStaticIntField((jclass) obj, fieldID);
                result = &value;
                if (LOG) {
                    LOGI("get value %d, addr %d", value, result);
                }
            } else {
                jint value = env->GetIntField(obj, fieldID);
                result = &value;
            }
            break;
        }
        case 'J': {
            if (isStatic) {
                jlong value = env->GetStaticLongField((jclass) obj, fieldID);
                result = &value;
            } else {
                jlong value = env->GetLongField(obj, fieldID);
                result = &value;
            }
            break;
        }
        case 'F': {
            if (isStatic) {
                jfloat value = env->GetStaticFloatField((jclass) obj, fieldID);
                result = &value;
            } else {
                jfloat value = env->GetFloatField(obj, fieldID);
                result = &value;
            }
            break;
        }
        case 'D': {
            if (isStatic) {
                jdouble value = env->GetStaticDoubleField((jclass) obj, fieldID);
                result = &value;
            } else {
                jdouble value = env->GetDoubleField(obj, fieldID);
                result = &value;
            }
            break;
        }
        default: {
            if (isStatic) {
                jobject value = env->GetStaticObjectField((jclass) obj, fieldID);
                result = &value;
            } else {
                jobject value = env->GetObjectField(obj, fieldID);
                result = &value;
            }
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
        if (LOG) {
            LOGI("Field sign is empty, try from getFields or getDeclaredFields");
        }
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

        if (!isStatic) {
            env->DeleteLocalRef(clazz);
        }
    }
    if (LOG && result == NULL) {
        jstring toString_ = toString(env, obj);
        const char *toString = env->GetStringUTFChars(toString_, 0);
        LOGI("Not found method %s in object %s", name, toString);
        env->ReleaseStringUTFChars(toString_, toString);
        env->DeleteGlobalRef(toString_);
    } else {
        if (LOG) {
            LOGI("Found file %s from getFields or getDeclaredFields , %d", name, result);
        }
    }
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    return result;
}

void *get(JNIEnv *env, jobject obj, const char *name, const char *sign, jboolean isStatic) {
    jfieldID fieldID = findField(env, obj, name, sign, isStatic);
    if (fieldID == NULL) {
        return NULL;
    }
    char type = 0;
    if (sign != NULL && strlen(sign) == 1) {
        type = sign[0];
    }
    return get(env, obj, fieldID, type, isStatic);
}

void *get(JNIEnv *env, jobject obj, jfieldID fieldID, const char type, jboolean isStatic) {
    void *result = NULL;
    switch (type) {
        case 'Z': {
            if (isStatic) {
                jboolean value = env->GetStaticBooleanField((jclass) obj, fieldID);
                result = &value;
            } else {
                jboolean value = env->GetBooleanField(obj, fieldID);
                result = &value;
            }
            break;
        }
        case 'B': {
            if (isStatic) {
                jbyte value = env->GetStaticByteField((jclass) obj, fieldID);
                result = &value;
            } else {
                jbyte value = env->GetByteField(obj, fieldID);
                result = &value;
            }
            break;
        }
        case 'C': {
            if (isStatic) {
                jchar value = env->GetStaticCharField((jclass) obj, fieldID);
                result = &value;
            } else {
                jchar value = env->GetCharField(obj, fieldID);
                result = &value;
            }
            break;
        }
        case 'S': {
            if (isStatic) {
                jshort value = env->GetStaticShortField((jclass) obj, fieldID);
                result = &value;
            } else {
                jshort value = env->GetShortField(obj, fieldID);
                result = &value;
            }
            break;
        }
        case 'I': {
            if (isStatic) {
                jint value = env->GetStaticIntField((jclass) obj, fieldID);
                result = &value;
                if (LOG) {
                    LOGI("get value %d, addr %d", value, result);
                }
            } else {
                jint value = env->GetIntField(obj, fieldID);
                result = &value;
            }
            break;
        }
        case 'J': {
            if (isStatic) {
                jlong value = env->GetStaticLongField((jclass) obj, fieldID);
                result = &value;
            } else {
                jlong value = env->GetLongField(obj, fieldID);
                result = &value;
            }
            break;
        }
        case 'F': {
            if (isStatic) {
                jfloat value = env->GetStaticFloatField((jclass) obj, fieldID);
                result = &value;
            } else {
                jfloat value = env->GetFloatField(obj, fieldID);
                result = &value;
            }
            break;
        }
        case 'D': {
            if (isStatic) {
                jdouble value = env->GetStaticDoubleField((jclass) obj, fieldID);
                result = &value;
            } else {
                jdouble value = env->GetDoubleField(obj, fieldID);
                result = &value;
            }
            break;
        }
        default: {
            if (isStatic) {
                jobject value = env->GetStaticObjectField((jclass) obj, fieldID);
                result = &value;
            } else {
                jobject value = env->GetObjectField(obj, fieldID);
                result = &value;
            }
        }
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
