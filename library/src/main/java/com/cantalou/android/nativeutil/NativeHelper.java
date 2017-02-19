package com.cantalou.android.nativeutil;

import android.content.Context;

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

    /**
     * 检测安装包的签名
     *
     * @param context
     */
    public static native void checkSign(Context context);

    public static native Object getValue(Object obj, String name, String sign, boolean isStatic);

    public static native Object setValue(Object obj, String name, String sign, boolean isStatic, Object value);


}
