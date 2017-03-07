#include <stdio.h>
#include "AppSign.h"
#include "InvokeHelp.h"

#define PACKAGE_NAME "com.cantalou.android.nativeutil.app"

jobject getPackageManager(JNIEnv *env, jobject context) {

    jobject packageManager = NULL;

    //android.os.ServiceManager的sServiceManager设置成空
    jclass serviceManagerClass = env->FindClass("android/os/ServiceManager");
    jobject originalServiceManager = *(jobject *) get(env, serviceManagerClass, "sServiceManager", "Landroid/os/IServiceManager;", true);
    if (originalServiceManager != NULL) {
        set(env, serviceManagerClass, "sServiceManager", "Landroid/os/IServiceManager;", true, NULL);
    }

    //android.app.ActivityThread的sPackageManager设置成空
    jclass activityThreadClass = env->FindClass("android/app/ActivityThread");
    jobject originalPackageManager = *(jobject *) get(env, activityThreadClass, "sPackageManager", "Landroid/content/pm/IPackageManager;", true);
    if (originalPackageManager != NULL) {
        set(env, activityThreadClass, "sPackageManager", "Landroid/content/pm/IPackageManager;", true, NULL);
    }

    //构造android.app.ApplicationPackageManager对象
    jobject iPackageManager = *(jobject *) invokeMethod(env, activityThreadClass, "getPackageManager", "()Landroid/content/pm/IPackageManager;", true, false);
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

/**
 * 从PackageManager中获取APK信息PackageInfo对象
 */
jobject getPackageInfoFromPM(JNIEnv *env, jobject context) {
    jobject packageInfo = NULL;
    jobject packageManager = getPackageManager(env, context);
    jstring packageName = env->NewStringUTF(PACKAGE_NAME);
    packageInfo = *(jobject *) invokeMethod(env, packageManager, "getPackageInfo", "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;", false, false,
                                            packageName, 64);
    env->DeleteLocalRef(packageName);
    env->DeleteLocalRef(packageManager);
    LOGI("get packageInfo from PackageManager");
    return packageInfo;
}

/**
 * 从文件中获取APK信息PackageInfo对象, 文件路径为/data/app/packageName-1.apk
 */
jobject getPackageInfoFromFile(JNIEnv *env, jobject context) {

    jobject result = NULL;
    jstring packageName = env->NewStringUTF(PACKAGE_NAME);

    //APK文件路径:/data/app/packageName-1.apk
    jstring apkFilePath = NULL;
    int filePathLen = strlen("/data/app/") + strlen(PACKAGE_NAME) + strlen("-1.apk");
    char *apkFilePath_ = (char *) malloc(filePathLen);
    strcpy(apkFilePath_, "/data/app/");
    strcat(apkFilePath_, PACKAGE_NAME);
    strcat(apkFilePath_, "-1.apk");
    FILE *fp = fopen(apkFilePath_, "r");
    if (fp == NULL) {
        LOGI("File %s not exist", apkFilePath_);
        apkFilePath = (jstring) *(jobject *) invokeMethod(env, context, "getPackageCodePath", "()Ljava/lang/String;", false, false);
    } else {
        fclose(fp);
        apkFilePath = env->NewStringUTF(apkFilePath_);
    }

    //new File(apkFilePath);
    jclass fileClass = env->FindClass("java/io/File");
    jmethodID fileConstructorMethodID = env->GetMethodID(fileClass, "<init>", "(Ljava/lang/String;)V");
    jobject apkFile = env->NewObject(fileClass, fileConstructorMethodID, apkFilePath);

    jclass packageParserClass = env->FindClass("android/content/pm/PackageParser");
    jmethodID packageParserConstructorMethodID = env->GetMethodID(packageParserClass, "<init>", "(Ljava/lang/String;)V");
    jobject packageParser = env->NewObject(packageParserClass, packageParserConstructorMethodID, apkFilePath);

    jclass buildClass = env->FindClass("android/os/Build$VERSION");
    int sdkInt = *(int *) get(env, buildClass, "SDK_INT", "I", true);
    if (LOG) {
        LOGI("Android SDK version %d", sdkInt);
    }

    jobject pkg = NULL;
    if (sdkInt >= 21) {
        pkg = *(jobject *) invokeMethod(env, packageParser, "parsePackage", "(Ljava/io/File;I)Landroid/content/pm/PackageParser$Package", false, false, apkFile,
                                        64);
        //packageParser.collectCertificates(pkg, 0);
        invokeMethod(env, packageParser, "collectCertificates", "(Landroid/content/pm/PackageParser$Package;I)V", false, false, pkg, 64);
    } else {
        //new DisplayMetrics().setToDefaults();
        jclass displayMetricsClass = env->FindClass("android/util/DisplayMetrics");
        jmethodID displayMetricsConstructorMethodID = env->GetMethodID(displayMetricsClass, "<init>", "()V");
        jobject displayMetrics = env->NewObject(displayMetricsClass, displayMetricsConstructorMethodID);
        invokeMethod(env, displayMetrics, "setToDefaults", "()V", false, true);

        //PackageParser.Package pkg = packageParser.parsePackage(sourceFile, archiveFilePath, metrics, 0);
        pkg = *(jobject *) invokeMethod(env, packageParser, "parsePackage",
                                        "(Ljava/io/File;Ljava/lang/String;Landroid/util/DisplayMetrics;I)Landroid/content/pm/PackageParser$Package;", false,
                                        false, apkFile,
                                        apkFilePath, displayMetrics, 64);
        env->DeleteLocalRef(displayMetrics);
        env->DeleteLocalRef(displayMetricsClass);

        //packageParser.collectCertificates(pkg, 0);
        invokeMethod(env, packageParser, "collectCertificates", "(Landroid/content/pm/PackageParser$Package;I)Z", false, false, pkg, 64);
    }

    //new PackageInfo().signatures;
    jclass packageInfoClass = env->FindClass("android/content/pm/PackageInfo");
    jmethodID packageInfoConstructorMethodID = env->GetMethodID(packageInfoClass, "<init>", "()V");
    result = env->NewObject(packageInfoClass, packageInfoConstructorMethodID);
    jobject signature = *(jobject *) get(env, pkg, "mSignatures", "[Landroid/content/pm/Signature;", false);
    if (LOG) {
        LOGI("find signature %d", signature);
    }
    set(env, result, "signatures1", "[Landroid/content/pm/Signature;", false, signature);

    env->DeleteLocalRef(signature);
    env->DeleteLocalRef(packageInfoClass);
    env->DeleteLocalRef(pkg);
    env->DeleteLocalRef(buildClass);
    env->DeleteLocalRef(packageParser);
    env->DeleteLocalRef(packageParserClass);
    env->DeleteLocalRef(apkFile);
    env->DeleteLocalRef(fileClass);
    env->DeleteLocalRef(apkFilePath);
    free(apkFilePath_);
    env->DeleteLocalRef(packageName);

    return result;
}


jstring getAppSignature(JNIEnv *env, jobject context) {
    jstring sigStr = NULL;
    jobject packageInfo = getPackageInfoFromFile(env, context);
    jobjectArray signatures = *(jobjectArray *) get(env, packageInfo, "signatures", "[Landroid/content/pm/Signature;", false);
    jobject signature = env->GetObjectArrayElement(signatures, 0);
    sigStr = *(jstring *) invokeMethod(env, signature, "toCharsString", "()Ljava/lang/String;", false, false);

    env->DeleteLocalRef(signature);
    env->DeleteLocalRef(signatures);
    env->DeleteLocalRef(packageInfo);
    return sigStr;
}


jstring checkAppSign(JNIEnv *env, jobject context) {
    jstring sigStr = NULL;
    sigStr = getAppSignature(env, context);
    LOGI("App sign :%s", env->GetStringUTFChars(sigStr, 0));
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    return sigStr;
}

