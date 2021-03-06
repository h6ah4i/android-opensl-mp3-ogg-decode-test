/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.h6ah4i.android.example.decodemp3ogg;

import android.app.Activity;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Toast;

public class MainActivity extends Activity
        implements ActivityCompat.OnRequestPermissionsResultCallback {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        findViewById(R.id.button_decode_mp3).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                decodeAssetAudio(getAssets(), "background.mp3");
                Toast.makeText(MainActivity.this, "MP3 Decode finished", Toast.LENGTH_SHORT).show();
            }
        });

        findViewById(R.id.button_decode_ogg).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                decodeAssetAudio(getAssets(), "background.ogg");
                Toast.makeText(MainActivity.this, "OGG Decode finished", Toast.LENGTH_SHORT).show();
            }
        });
    }

    public static native boolean decodeAssetAudio(AssetManager assetManager, String filename);

    static {
        System.loadLibrary("decode-mp3-ogg-jni");
    }
}
