#include "AppSign.h"
#include "InvokeHelp.h"

jobject getPackageManager(JNIEnv *env, jobject context) {

    jobject packageManager = NULL;

    //android.os.ServiceManager的sServiceManager设置成空
    jclass serviceManagerClass = env->FindClass("android/os/ServiceManager");
    jobject originalServiceManager = get(env, serviceManagerClass, "sServiceManager", "Landroid/os/IServiceManager;", true);
    if (originalServiceManager != NULL) {
        set(env, serviceManagerClass, "sServiceManager", "Landroid/os/IServiceManager;", true, NULL);
    }

    //android.app.ActivityThread的sPackageManager设置成空
    jclass activityThreadClass = env->FindClass("android/app/ActivityThread");
    jobject originalPackageManager = get(env, activityThreadClass, "sPackageManager", "Landroid/content/pm/IPackageManager;", true);
    if (originalPackageManager != NULL) {
        set(env, activityThreadClass, "sPackageManager", "Landroid/content/pm/IPackageManager;", true, NULL);
    }

    //构造android.app.ApplicationPackageManager对象
    jobject iPackageManager = invokeMethod(env, activityThreadClass, "getPackageManager", "()Landroid/content/pm/IPackageManager;", true, false);
    jclass packageManagerClass = env->FindClass("android/app/ApplicationPackageManager");
    jmethodID constructorMethodID = env->GetMethodID(packageManagerClass, "<init>", "(Landroid/app/ContextImpl;Landroid/content/pm/IPackageManager;)V");
    packageManager = env->NewObject(packageManagerClass, constructorMethodID, context, iPackageManager);

    //还原android.os.ServiceManager
    if (originalServiceManager != NULL) {
        set(env, serviceManagerClass, "sServiceManager", "Landroid/os/IServiceManager;", true, originalServiceManager);
    }
    env->DeleteLocalRef(serviceManagerClass);
    env->DeleteLocalRef(originalServiceManager);

    //还原android.app.ActivityThread
    if (originalPackageManager != NULL) {
        set(env, activityThreadClass, "sPackageManager", "Landroid/content/pm/IPackageManager;", true, originalPackageManager);
    }
    env->DeleteLocalRef(activityThreadClass);
    env->DeleteLocalRef(originalPackageManager);

    env->DeleteLocalRef(iPackageManager);
    env->DeleteLocalRef(packageManagerClass);

    return packageManager;
}


jstring checkAppSign(JNIEnv *env, jobject context) {
    jstring sigStr  = NULL;
    jobject packageManager = getPackageManager(env, context);
    jstring packageName = (jstring) invokeMethod(env, context, "getPackageName", "()Ljava/lang/String;", false, false);
    jobject packageInfo = invokeMethod(env, packageManager, "getPackageInfo", "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;", false, false,
                                       packageName, 64);
    jobjectArray signatures = (jobjectArray) get(env, packageInfo, "signatures", "[Landroid/content/pm/Signature;", false);
    jobject signature = env->GetObjectArrayElement(signatures, 0);
    sigStr = (jstring)invokeMethod(env, signature, "toCharsString", "()Ljava/lang/String;", false, false);
    LOGI("App sign :%s", env->GetStringUTFChars(sigStr,0));
    env->DeleteLocalRef(signature);
    env->DeleteLocalRef(signatures);
    env->DeleteLocalRef(packageName);
    env->DeleteLocalRef(packageInfo);
    env->DeleteLocalRef(packageManager);
    return sigStr;
}

