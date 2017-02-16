package com.cantalou.android.nativeutil;

/**
 * JNI接口
 *
 * @author LinZhiWei
 * @date 2017年02月14日 13:47
 */
public class NativeHelper {

    /**
     * 生成MD5摘要
     *
     * @return 大写字符的字符串摘要
     */
    public static native String MD5(String content);

    public static native Object test(Object obj, String name, String sign);
}
