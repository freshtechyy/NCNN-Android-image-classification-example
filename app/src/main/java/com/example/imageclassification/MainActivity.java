//
// Created by Huili Yu on 3/30/23.
//

package com.example.imageclassification;

import androidx.appcompat.app.AppCompatActivity;

import android.content.res.AssetManager;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.example.imageclassification.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'imageclassification' library on application startup.
    static {
        System.loadLibrary("imageclassification");
    }

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        Button btn_classify = (Button) findViewById(R.id.btn_classify);
        btn_classify.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Resources res = getResources();
                Bitmap bitmapIn = BitmapFactory.decodeResource(res, R.drawable.car);
                TextView pred = (TextView) findViewById(R.id.textView);
                pred.setText(ImageClassification(bitmapIn, getAssets()));
            }
        });
    }

    /**
     * A native method that is implemented by the 'imageclassification' native library,
     * which is packaged with this application.
     */
    public native String ImageClassification(Bitmap bitmapIn, AssetManager assetManager);
}