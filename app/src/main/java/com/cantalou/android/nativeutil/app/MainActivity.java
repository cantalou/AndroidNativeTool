package com.cantalou.android.nativeutil.app;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.TextView;

import com.cantalou.android.nativeutil.NativeHelper;


public class MainActivity extends AppCompatActivity {

    private TextView textView;

    static String voidMethod = "";

    static class Test {

        private static String staticMethod() {
            return "staticMethod ok";
        }

        private static void staticVoidMethod() {
            voidMethod = "staticVoidMethod ok";
            return;
        }

        private String instanceMethod() {
            return "instanceMethod ok";
        }

        private void instanceVoidMethod() {
            voidMethod = "instanceVoidMethod ok";
            return;
        }

        private static String setStaticMethod(String value) {
            return "staticMethod ok " + value;
        }

        private String setInstanceMethod(String value) {
            return "instanceMethod ok " + value;
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        textView = (TextView) findViewById(R.id.sample_text);
        StringBuilder sb = new StringBuilder();
//        sb.append("\n" + NativeHelper.test(Test.class, "staticMethod", "()Ljava/lang/String;", true, false, ""));
//        sb.append("\n" + NativeHelper.test(Test.class, "staticVoidMethod", "()V", true, true, "") + voidMethod);
//        sb.append("\n" + NativeHelper.test(new Test(), "instanceMethod", "()Ljava/lang/String;", false, false, ""));
//        sb.append("\n" + NativeHelper.test(new Test(), "instanceVoidMethod", "()V", false, true, "") + voidMethod);
//        sb.append("\n" + NativeHelper.test(Test.class, "setStaticMethod", "(Ljava/lang/String;)Ljava/lang/String;", true, false, "1"));
//        sb.append("\n" + NativeHelper.test(new Test(), "setInstanceMethod", "(Ljava/lang/String;)Ljava/lang/String;", false, false, "2"));

        sb.append("\n" + NativeHelper.test(Test.class, "staticMethod", null, true, false, ""));
//        sb.append("\n" + NativeHelper.test(Test.class, "staticVoidMethod", "()V", true, true, "") + voidMethod);
//        sb.append("\n" + NativeHelper.test(new Test(), "instanceMethod", "()Ljava/lang/String;", false, false, ""));
//        sb.append("\n" + NativeHelper.test(new Test(), "instanceVoidMethod", "()V", false, true, "") + voidMethod);
//        sb.append("\n" + NativeHelper.test(Test.class, "setStaticMethod", "(Ljava/lang/String;)Ljava/lang/String;", true, false, "1"));
//        sb.append("\n" + NativeHelper.test(new Test(), "setInstanceMethod", "(Ljava/lang/String;)Ljava/lang/String;", false, false, "2"));
        textView.setText(sb.toString());
    }

    // Used to load the 'NativeHelper' library on application startup.
    static {
        System.loadLibrary("NativeHelper");
    }

    public void text(View view) throws NoSuchMethodException {
    }


}
