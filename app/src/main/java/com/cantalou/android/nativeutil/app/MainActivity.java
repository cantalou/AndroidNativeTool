package com.cantalou.android.nativeutil.app;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.TextView;

import com.cantalou.android.nativeutil.NativeHelper;

import java.lang.reflect.Method;


public class MainActivity extends AppCompatActivity {

    private TextView textView;

    static class Test {

        private static String test() {
            return "static method";
        }

        private String test1() {
            return "instance method";
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        textView = (TextView) findViewById(R.id.sample_text);
        String msg = NativeHelper.test(Test.class, "test", "()Ljava/lang/String;") + ", " + NativeHelper.test(new Test(), "test1", "()Ljava/lang/String;");
        textView.setText(msg);
    }

    // Used to load the 'NativeHelper' library on application startup.
    static {
        System.loadLibrary("NativeHelper");
    }

    public void text(View view) throws NoSuchMethodException {
    }


}
