#include "InvokeHelp.h"
#include "Log.h"

void log(JNIEnv *env, char *format) {

    LOGI("log %s", "123");

    char *p = format;
    p += 400;
    char c = (*p);

//    va_list params;
//    va_start(params, format);
//    std::string msg = "";
//    char *p = format;
//    for (; *p; p++) {
//        if (*p != '%') {
//            msg += (*p);
//            continue;
//        }
//
//        char c = (*++p);
//        if (c == 'd') {
//            int intValue = va_arg(params, int);
//            msg += intValue;
//        } else if (c == 'f') {
//            double doubleValue = va_arg(params, double);
//            msg += doubleValue;
//        } else if (c == 's') {
//            char *str = va_arg(params, char *);
//            msg += str;
//        } else if (c == 'o') {
//            jobject obj = va_arg(params, jobject);
//            jstring toString = (jstring) invokeMethod(env, obj, "toString", "()Ljava/lang/String;", false, false);
//            const char *toString_ = env->GetStringUTFChars(toString, 0);
//            msg += toString_;
//            env->ReleaseStringUTFChars(toString, toString_);
//            env->DeleteLocalRef(toString);
//        } else {
//            msg += c;
//        }
//
//    }
//    if (LOG) {
//        LOGI("%s", msg.c_str());
//    }
//    va_end(params);
}

//void log(JNIEnv *env, char *format, ...) {
//    va_list params;
//    va_start(params, format);
//    va_end(params);
//}



