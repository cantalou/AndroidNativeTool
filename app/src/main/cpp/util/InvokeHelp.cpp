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
    if (!env->IsInstanceOf(obj, clazz)) {
        obj = *(jobject *) invokeMethod(env, obj, "getClass", "()Ljava/lang/Class;", false, false);
    }
    result = *(jstring *) invokeMethod(env, obj, "toString", "()Ljava/lang/String;", false, false);
    env->DeleteGlobalRef(clazz);
    return result;
}

jclass getClass(JNIEnv *env, jobject obj, jboolean isStatic) {
    if (isStatic) {
        return (jclass) obj;
    }
    return env->GetObjectClass(obj);
}

char getMethodType(JNIEnv *env, jobject obj, jmethodID methodID, const char *sign, jboolean isStatic) {

    if (sign != NULL) {
        return sign[strlen(sign) - 1];
    }

    char type = NULL;
    jclass objClass = getClass(env, obj, isStatic);
    jobject method = env->ToReflectedMethod(objClass, methodID, isStatic);
    jobject returnType = *(jobject *) invokeMethod(env, method, "getReturnType", "()Ljava/lang/Class;", false, false);
    jstring returnTypeName = *(jstring *) invokeMethod(env, returnType, "toString", "()Ljava/lang/String;", false, false);

    if (stringEquals(env, returnTypeName, "boolean")) {
        type = 'Z';
    } else if (stringEquals(env, returnTypeName, "byte")) {
        type = 'B';
    } else if (stringEquals(env, returnTypeName, "char")) {
        type = 'C';
    } else if (stringEquals(env, returnTypeName, "short")) {
        type = 'S';
    } else if (stringEquals(env, returnTypeName, "int")) {
        type = 'I';
    } else if (stringEquals(env, returnTypeName, "long")) {
        type = 'J';
    } else if (stringEquals(env, returnTypeName, "float")) {
        type = 'F';
    } else if (stringEquals(env, returnTypeName, "double")) {
        type = 'D';
    } else if (stringEquals(env, returnTypeName, "void")) {
        type = 'V';
    } else {
        type = NULL;
    }

    env->DeleteLocalRef(returnTypeName);
    env->DeleteLocalRef(returnType);
    env->DeleteLocalRef(method);
    env->DeleteLocalRef(objClass);

    return type;
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
            jclass objClazz = NULL;
            jclass clazz = env->FindClass("java/lang/Class");
            if (env->IsInstanceOf(obj, clazz)) {
                objClazz = (jclass) obj;
            } else {
                objClazz = env->GetObjectClass(obj);
            }
            result = env->GetMethodID(objClazz, name, sign);
            env->DeleteLocalRef(clazz);
            env->DeleteLocalRef(objClazz);
        }
    } else {

        //Class.getMethods()方法
        jclass objClazz = NULL;
        if (isStatic) {
            objClazz = (jclass) obj;
        } else {
            jclass clazz = env->FindClass("java/lang/Class");
            if (env->IsInstanceOf(obj, clazz)) {
                objClazz = (jclass) obj;
            } else {
                objClazz = env->GetObjectClass(obj);
            }
            env->DeleteLocalRef(clazz);
        }

        jobjectArray publicMethods = (jobjectArray) invokeMethod(env, objClazz, "getMethods", "()[Ljava/lang/reflect/Method;", false, false);
        result = findMethod(env, publicMethods, name);
        env->DeleteLocalRef(publicMethods);

        //Class.getDeclaredMethods()方法
        if (result == NULL) {
            jobjectArray declareMethods = (jobjectArray) invokeMethod(env, objClazz, "getDeclaredMethods", "()[Ljava/lang/reflect/Method;", false, false);
            result = findMethod(env, declareMethods, name);
            env->DeleteLocalRef(declareMethods);
        }

        env->DeleteLocalRef(objClazz);
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
    void *result = NULL;
    va_list args;
    va_start(args, isVoid);
    jmethodID methodID = findMethod(env, obj, name, sign, isStatic);
    if (methodID != NULL) {
        char type = 'V';
        if (!isVoid) {
            type = getMethodType(env, obj, methodID, sign, isStatic);
        }
        result = invokeMethodV(env, obj, methodID, type, isStatic, args);
    }
    va_end(args);
    return result;
}

/**
 * 调用静态方法
 */
void *invokeMethod(JNIEnv *env, const char *className, const char *name, const char *sign, jboolean isVoid, ...) {
    jclass clazz = env->FindClass(className);
    void *result = NULL;
    va_list args;
    va_start(args, isVoid);
    jmethodID methodID = findMethod(env, clazz, name, sign, true);
    if (methodID != NULL) {
        char type = 'V';
        if (!isVoid) {
            type = getMethodType(env, clazz, methodID, sign, true);
        }
        result = invokeMethodV(env, clazz, methodID, type, true, args);
    }
    va_end(args);
    env->DeleteLocalRef(clazz);
    return result;
}

/**
 * 调用方法
 */
void *invokeMethod(JNIEnv *env, jobject obj, jmethodID methodID, char type, jboolean isStatic, ...) {
    void *result = NULL;
    va_list args;
    va_start(args, isStatic);
    result = invokeMethodV(env, obj, methodID, type, isStatic, args);
    va_end(args);
    return result;
}

/**
 * 调用方法
 */
void *invokeMethodV(JNIEnv *env, jobject obj, jmethodID methodID, char type, jboolean isStatic, va_list args) {
    void *result = NULL;
    if (type == NULL) {
        type = getMethodType(env, obj, methodID, NULL, isStatic);
    }
    switch (type) {
        case 'Z': {
            if (isStatic) {
                jboolean value = env->CallStaticBooleanMethodV((jclass) obj, methodID, args);
                result = &value;
            } else {
                jboolean value = env->CallBooleanMethodV(obj, methodID, args);
                result = &value;
            }
            break;
        }
        case 'B': {
            if (isStatic) {
                jbyte value = env->CallStaticByteMethodV((jclass) obj, methodID, args);
                result = &value;
            } else {
                jbyte value = env->CallByteMethodV(obj, methodID, args);
                result = &value;
            }
            break;
        }
        case 'C': {
            if (isStatic) {
                jchar value = env->CallStaticCharMethodV((jclass) obj, methodID, args);
                result = &value;
            } else {
                jchar value = env->CallCharMethodV(obj, methodID, args);
                result = &value;
            }
            break;
        }
        case 'S': {
            if (isStatic) {
                jshort value = env->CallStaticShortMethodV((jclass) obj, methodID, args);
                result = &value;
            } else {
                jshort value = env->CallShortMethodV(obj, methodID, args);
                result = &value;
            }
            break;
        }
        case 'I': {
            if (isStatic) {
                jint value = env->CallStaticIntMethodV((jclass) obj, methodID, args);
                result = &value;
            } else {
                jint value = env->CallIntMethodV(obj, methodID, args);
                result = &value;
            }
            break;
        }
        case 'J': {
            if (isStatic) {
                jlong value = env->CallStaticLongMethodV((jclass) obj, methodID, args);
                result = &value;
            } else {
                jlong value = env->CallLongMethodV(obj, methodID, args);
                result = &value;
            }
            break;
        }
        case 'F': {
            if (isStatic) {
                jfloat value = env->CallStaticFloatMethodV((jclass) obj, methodID, args);
                result = &value;
            } else {
                jfloat value = env->CallFloatMethodV(obj, methodID, args);
                result = &value;
            }
            break;
        }
        case 'D': {
            if (isStatic) {
                jdouble value = env->CallStaticDoubleMethodV((jclass) obj, methodID, args);
                result = &value;
            } else {
                jdouble value = env->CallDoubleMethodV(obj, methodID, args);
                result = &value;
            }
            break;
        }
        case 'V': {
            if (isStatic) {
                env->CallStaticVoidMethodV((jclass) obj, methodID, args);
            } else {
                env->CallVoidMethodV(obj, methodID, args);
            }
            break;
        }
        default: {
            if (isStatic) {
                jobject value = env->CallStaticObjectMethodV((jclass) obj, methodID, args);
                result = &value;
            } else {
                jobject value = env->CallObjectMethodV(obj, methodID, args);
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
            LOGI("findField sign param is empty, try from getFields or getDeclaredFields");
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
        LOGI("Not found field %s in object %s", name, toString);
        env->ReleaseStringUTFChars(toString_, toString);
        env->DeleteGlobalRef(toString_);
    } else {
        if (LOG) {
            LOGI("Found field %s from getFields or getDeclaredFields , %d", name, result);
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
