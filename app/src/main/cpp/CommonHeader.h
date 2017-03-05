#include <jni.h>
#include <string.h>
#include <Android/log.h>

#ifdef __cplusplus
extern "C"
#endif
#define LOG    true
#define TAG    "NativeTool"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
