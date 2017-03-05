package com.cantalou.android.nativeutil.app;

import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.TextView;

import com.cantalou.android.nativeutil.NativeHelper;


public class MainActivity extends AppCompatActivity {


    // Used to load the 'NativeHelper' library on application startup.
    static {
        System.loadLibrary("NativeHelper");
    }


    private TextView textView;

    static String voidMethod = "";


    static class Test {

        static String staticField = "get staticField ok";

        String instanceField = "get instanceField ok";

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

        public boolean Z() {
            return false;
        }

        public byte B() {
            return 0;
        }

        public char C() {
            return 0;
        }

        public short S() {
            return 0;
        }

        public int I() {
            return 0;
        }

        public long J() {
            return 0;
        }

        public float F() {
            return 0;
        }

        public double D() {
            return 0;
        }

        public boolean[] ZA() {
            return new boolean[]{false};
        }

        public void V() {

        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        try {
            setContentView(R.layout.activity_main);
            textView = (TextView) findViewById(R.id.sample_text);
            StringBuilder sb = new StringBuilder();
            String sign = getPackageManager().getPackageInfo(getPackageName(), PackageManager.GET_SIGNATURES).signatures[0].toCharsString();
            sb.append("\n 返回值类型 :" + NativeHelper.checkSign(getBaseContext()).equals(sign));
            textView.setText(sb.toString());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void text(View view) throws NoSuchMethodException {

    }


}
