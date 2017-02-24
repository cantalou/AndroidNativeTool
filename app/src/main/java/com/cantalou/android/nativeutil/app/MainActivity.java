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
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        try {
            setContentView(R.layout.activity_main);
            textView = (TextView) findViewById(R.id.sample_text);
            StringBuilder sb = new StringBuilder();
            sb.append("\n" + this.getPackageCodePath());
            String sign = getPackageManager().getPackageInfo(getPackageName(), PackageManager.GET_SIGNATURES).signatures[0].toCharsString();
            String nativeSign = NativeHelper.checkSign(this);
            sb.append("\n 签名 :" + sign.equals(nativeSign));
            textView.setText(sb.toString());
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
    }

    public void text(View view) throws NoSuchMethodException {

    }


}
