package com.cantalou.android.nativeutil.app;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.TextView;

import com.cantalou.android.nativeutil.NativeHelper;

import java.lang.reflect.Method;


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
        setContentView(R.layout.activity_main);
        textView = (TextView) findViewById(R.id.sample_text);
        StringBuilder sb = new StringBuilder();

        //NativeHelper.setValue(Test.class, "staticField", "Ljava/lang/String;", true, "set staticField ok");
        //sb.append("\n" + NativeHelper.getValue(Test.class, "staticField", "Ljava/lang/String;", true));

        Test test = new Test();
        NativeHelper.setValue(test, "instanceField", "Ljava/lang/String;", false,"set instanceField ok");
        //sb.append("\n" + NativeHelper.getValue(test, "instanceField", "Ljava/lang/String;", false));

        textView.setText(sb.toString());
    }

    public void text(View view) throws NoSuchMethodException {

    }


}
