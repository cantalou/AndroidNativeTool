#include <string.h>
#include <jni.h>
#include <Android/log.h>
#include "InvokeHelp.h"

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

/**
 * 判断方法的返回类型是否为Void
 */
jboolean isVoidMethod(JNIEnv *env, jclass clazz, jmethodID methodID, const char *name) {

    if (stringEquals(name, "getReturnType") || stringEquals(name, "toString") || true) {
        return false;
    }

    jboolean isVoid = false;
    jobject method = env->ToReflectedMethod(clazz, methodID, true);
    jobject returnType = invoke(env, method, "getReturnType", "()Ljava/lang/Class;");
    jstring returnTypeName = (jstring) invoke(env, returnType, "toString", "()Ljava/lang/String;");
    const char *returnTypeName_ = env->GetStringUTFChars(returnTypeName, 0);
    if (stringEquals(returnTypeName_, "void")) {
        isVoid = true;
    }
    env->ReleaseStringUTFChars(returnTypeName, returnTypeName_);
    env->DeleteLocalRef(returnTypeName);
    env->DeleteLocalRef(returnType);
    env->DeleteLocalRef(method);

    return isVoid;
}

jobject findMethod(JNIEnv *env, jobjectArray methods, const char *name) {
    jobject method = NULL;
    for (int i = 0, len = env->GetArrayLength(methods); i < len; ++i) {
        method = env->GetObjectArrayElement(methods, i);
        jstring methodName = (jstring) invoke(env, method, "getName", "()Ljava/lang/String;");
        const char *methodName_ = env->GetStringUTFChars(methodName, 0);
        if (stringEquals(name, methodName_)) {
            break;
        } else {
            env->DeleteLocalRef(method);
        }
        env->ReleaseStringUTFChars(methodName, methodName_);
        env->DeleteLocalRef(methodName);
    }
    LOGI("find method %s", method != NULL ? name : "not found");
    return method;
}


/**
 * 判断方法是否为Static
 */
jint isStatic(JNIEnv *env, jobject method) {

}


/**
 * 调用静态函数
 */
jobject invoke(JNIEnv *env, jobject obj, const char *name, const char *sign, ...) {

    jobject result = NULL;
    jmethodID methodID = NULL;

    jboolean isStatic;
    jobject method;
    jclass classClazz = env->FindClass("java/lang/Class");
    if (env->IsInstanceOf(obj, classClazz)) {
        methodID = env->GetStaticMethodID((jclass) obj, name, sign);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            isStatic = false;
        } else {
            isStatic = true;
        }
    }
    env->DeleteLocalRef(classClazz);

    jclass objClass;
    if (isStatic) {
        objClass = (jclass) obj;
    } else {
        objClass = env->GetObjectClass(obj);
    }

    va_list args;
    va_start(args, sign);

    if (methodID == NULL) {
        if (sign == NULL || strlen(sign) == 0) {

            //Class.getMethods()方法
            jobjectArray publicMethods = (jobjectArray) invoke(env, objClass, "getMethods", "()[Ljava/lang/reflect/Method;");
            for (int i = 0, len = env->GetArrayLength(publicMethods); i < len; ++i) {
                method = env->GetObjectArrayElement(publicMethods, i);
                jstring methodName = (jstring) invoke(env, method, "getName", "()Ljava/lang/String;");
                const char *methodName_ = env->GetStringUTFChars(methodName, 0);
                if (stringEquals(name, methodName_)) {
                    break;
                } else {
                    env->DeleteLocalRef(method);
                }
                env->ReleaseStringUTFChars(methodName, methodName_);
                env->DeleteLocalRef(methodName);
            }
            env->DeleteLocalRef(publicMethods);
            LOGI("getMethods method name %s, %s", name, method != NULL ? "not null" : " null");

            //Class.getDeclaredMethods()方法
            if (method == NULL) {
                jobjectArray declareMethods = (jobjectArray) invoke(env, objClass, "getDeclaredMethods", "()[Ljava/lang/reflect/Method;");
                for (int i = 0, len = env->GetArrayLength(declareMethods); i < len; ++i) {
                    method = env->GetObjectArrayElement(declareMethods, i);
                    jstring methodName = (jstring) invoke(env, method, "getName", "()Ljava/lang/String;");
                    const char *methodName_ = env->GetStringUTFChars(methodName, 0);
                    if (stringEquals(name, methodName_)) {
                        break;
                    } else {
                        env->DeleteLocalRef(method);
                    }
                    env->ReleaseStringUTFChars(methodName, methodName_);
                    env->DeleteLocalRef(methodName);
                }
                env->DeleteLocalRef(declareMethods);
            }

            if (method != NULL) {
                methodID = env->FromReflectedMethod(method);
                env->DeleteLocalRef(method);
                LOGI("FromReflectedMethod %d", methodID);
            }


        } else {
            if (isStatic) {
                methodID = env->GetStaticMethodID(objClass, name, sign);
            } else {
                methodID = env->GetMethodID(objClass, name, sign);
            }

            if (env->ExceptionCheck()) {
                env->ExceptionDescribe();
                env->ExceptionClear();
            }
        }
    }

    if (methodID != NULL) {
        LOGI("Method %s , static %s", name, isStatic ? "true" : "false");
        jboolean isVoid = isVoidMethod(env, objClass, methodID, name);
        if (isStatic) {
            if (isVoid) {
                env->CallStaticVoidMethod(objClass, methodID, args);
            } else {
                result = env->CallStaticObjectMethod(objClass, methodID, args);
            }
        } else {
            if (isVoid) {
                env->CallVoidMethod(obj, methodID, args);
            } else {
                result = env->CallObjectMethod(obj, methodID, args);
            }
        }
    } else {
        LOGI("Method %s not found", name);
    }

    if (objClass != obj) {
        env->DeleteLocalRef(objClass);
    }

    va_end(args);
    if (stringEquals(name, "test")) {
        LOGI("invoke %s , return %s", name, result != NULL ? "not null" : " null");
    }
    return result;
}


/**
 * 获取属性值
 */
jobject get(JNIEnv *env, jobject obj, const char *name, const char *sign) {

    jobject result = NULL;
    jclass classClazz = env->FindClass("java/lang/Class");
    jboolean isStatic = env->IsInstanceOf(obj, classClazz);

    jclass objClass;
    if (isStatic) {
        objClass = (jclass) obj;
    } else {
        objClass = env->GetObjectClass(obj);
    }

    if (sign == NULL) {

        jobject field;

        //Class.getMethods()方法
        jobjectArray publicMethods = (jobjectArray) invoke(env, objClass, "getMethods", "()[Ljava/lang/reflect/Method;");
        for (int i = 0, len = env->GetArrayLength(publicMethods); i < len; ++i) {
            field = env->GetObjectArrayElement(publicMethods, i);
            jstring methodName = (jstring) invoke(env, field, "getName", "()Ljava/lang/String;");
            if (strncmp(name, env->GetStringUTFChars(methodName, 0), strlen(name)) == 0) {
                break;
            } else {
                field = NULL;
            }
        }

        //Class.getDeclaredMethods()方法
        if (field == NULL) {
            jobjectArray declareMethods = (jobjectArray) invoke(env, objClass, "getDeclaredMethods", "()[Ljava/lang/reflect/Method;");
            for (int i = 0, len = env->GetArrayLength(declareMethods); i < len; ++i) {
                field = env->GetObjectArrayElement(declareMethods, i);
                jstring methodName = (jstring) invoke(env, field, "getName", "()Ljava/lang/String;");
                if (strncmp(name, env->GetStringUTFChars(methodName, 0), strlen(name)) == 0) {
                    break;
                } else {
                    field = NULL;
                }
            }
        }

        if (field == NULL) {
            LOGI("Field %s not found", name);
            return NULL;
        }

        result = invoke(env, field, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");

    } else {
        jfieldID methodID = env->GetFieldID(objClass, name, sign);
        result = env->GetObjectField(obj, methodID);
    }

    return result;
}


/**
 * 调用静态函数
 */
jobject invokeStatic(JNIEnv *env, jclass jclazz, jstring methodName, jstring sign) {
    jmethodID method = env->GetStaticMethodID(jclazz, env->GetStringUTFChars(methodName, 0), env->GetStringUTFChars(sign, 0));
    return env->CallStaticObjectMethod(jclazz, method);
}


/**
 * 调用类的静态函数
 */
jobject invokeStaticMethods(JNIEnv *env, jstring className, jstring methodName, jobjectArray types, jobjectArray values) {
    jclass context = env->FindClass("java/lang/Class");
    jmethodID forName = env->GetStaticMethodID(context, "forName", "(Ljava/lang/String;)Ljava/lang/Class;");
    jclass clazz = (jclass) env->CallStaticObjectMethod(context, forName, className);
    return NULL;
}

/**
 * 调用类的静态函数
 */
jobject invokeStaticMethods(JNIEnv *env, jclass clazz, jstring methodName, jobjectArray types, jobjectArray values) {
    jmethodID getMethodID = env->GetMethodID(clazz, "getMethod", "(Ljava/lang/String;[Ljava/lang/Class;)Ljava/lang/reflect/Method;");
    jobject getMethodResult = env->CallObjectMethod(clazz, getMethodID, methodName, types);
    jclass getMethodClass = env->GetObjectClass(getMethodResult);
    jmethodID invokeID = env->GetMethodID(getMethodClass, "invoke", "(Ljava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;");
    jobject invoke_obj = env->CallObjectMethod(NULL, invokeID, NULL, values);
    return invoke_obj;
}

//获取类的成员变量的值
jobject getFieldObject(JNIEnv *env, jstring class_name, jobject obj, jstring fieldName) {
    jclass context = env->FindClass("java/lang/Class");
    jmethodID forName_func = env->GetStaticMethodID(context, "forName", "(Ljava/lang/String;)Ljava/lang/Class;");
    jobject class_obj = env->CallStaticObjectMethod(context, forName_func, class_name);
    jclass class_java = env->GetObjectClass(class_obj);

    jmethodID getField_func = env->GetMethodID(class_java, "getDeclaredField", "(Ljava/lang/String;)Ljava/lang/reflect/Field;");
    jobject method_obj = env->CallObjectMethod(class_obj, getField_func, fieldName);
    jclass class_method_obj = env->GetObjectClass(method_obj);

    jmethodID setaccess_func = env->GetMethodID(class_method_obj, "setAccessible", "(Z)V");
    env->CallVoidMethod(method_obj, setaccess_func, true);

    jmethodID get_func = env->GetMethodID(class_method_obj, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
    jobject get_obj = env->CallObjectMethod(method_obj, get_func, obj);

    env->DeleteLocalRef(class_java);
    env->DeleteLocalRef(method_obj);
    return get_obj;
}

//修改类的成员变量的值
void setStaticFieldObject(JNIEnv *env, jstring class_name, jstring fieldName, jobject obj, jobject filedVaule) {
    jclass context = env->FindClass("java/lang/Class");
    jmethodID forName_func = env->GetStaticMethodID(context, "forName", "(Ljava/lang/String;)Ljava/lang/Class;");
    jobject class_obj = env->CallStaticObjectMethod(context, forName_func, class_name);
    jclass class_java = env->GetObjectClass(class_obj);

    jmethodID getField_func = env->GetMethodID(class_java, "getDeclaredField", "(Ljava/lang/String;)Ljava/lang/reflect/Field;");
    jobject method_obj = env->CallObjectMethod(class_obj, getField_func, fieldName);
    jclass class_method_obj = env->GetObjectClass(method_obj);

    jmethodID setaccess_func = env->GetMethodID(class_method_obj, "setAccessible", "(Z)V");
    env->CallVoidMethod(method_obj, setaccess_func, true);

    jmethodID set_func = env->GetMethodID(class_method_obj, "set", "(Ljava/lang/Object;Ljava/lang/Object;)V");
    env->CallVoidMethod(method_obj, set_func, obj, filedVaule);

    env->DeleteLocalRef(class_java);
    env->DeleteLocalRef(method_obj);
}