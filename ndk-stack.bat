adb logcat | ndk-stack -sym ./build/intermediates/cmake/release/obj/x86/libNativeHelper.so
adb logcat | ndk-stack.cmd -sym ./build/intermediates/cmake/release/obj/x86
adb logcat | ndk-stack.cmd -sym ./app/build/intermediates/cmake/debug/obj/x86