#include "../CommonHeader.h"

jobject getPackageManager(JNIEnv *env, jobject context);

jobject getPackageInfoFromPM(JNIEnv *env, jobject context);

jobject getPackageInfoFromFile(JNIEnv *env, jobject context);

jstring getAppSignature(JNIEnv *env, jobject context);

jstring checkAppSign(JNIEnv *env, jobject context);