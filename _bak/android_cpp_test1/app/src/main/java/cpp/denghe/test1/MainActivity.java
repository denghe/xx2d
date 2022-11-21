package cpp.denghe.test1;

import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import cpp.denghe.test1.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    private GLSurfaceView gLView;

    private static String LOGTAG = "***************** test1 java ******************";

    // Used to load the 'test1' library on application startup.
    static {
        System.loadLibrary("test1");
    }

    //private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        gLView = new MyGLSurfaceView(this);
        setContentView(gLView);

        onCreateJNI();
        //Log.d(LOGTAG, "onCreate");
        Log.d(LOGTAG, "stringFromJNI() = " + stringFromJNI());

//        binding = ActivityMainBinding.inflate(getLayoutInflater());
//        setContentView(binding.getRoot());
//
//        // Example of a call to a native method
//        TextView tv = binding.sampleText;
//        tv.setText(stringFromJNI());
    }

    @Override
    protected void onResume() {
        super.onResume();
        gLView.onResume();
        //Log.d(LOGTAG, "onResume");
        onResumeJNI();
    }

    @Override
    protected void onPause() {
        super.onPause();
        gLView.onPause();
        //Log.d(LOGTAG, "onPause");
        onPauseJNI();
    }

    public native String stringFromJNI();
    public native void onCreateJNI();
    public native void onResumeJNI();
    public native void onPauseJNI();
}