//Copyright (c) 2017 Finjin
//
//This file is part of Finjin Engine (finjin-engine).
//
//Finjin Engine is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//This Source Code Form is subject to the terms of the Mozilla Public
//License, v. 2.0. If a copy of the MPL was not distributed with this
//file, You can obtain one at http://mozilla.org/MPL/2.0/.


package com.finjin.engine;

import android.app.NativeActivity;
import android.app.Activity;
import android.app.UiModeManager;
import android.content.Intent;
import android.content.Context;
import android.content.res.Configuration;
import android.content.res.AssetManager;
import android.content.res.AssetFileDescriptor;
import android.os.Bundle;
import android.os.Build;
import android.os.Environment;
import android.net.Uri;
import android.view.InputDevice;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.widget.TextView;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;

public class FinjinNativeActivity extends NativeActivity 
{
    @Override
    public void onCreate(Bundle savedInstanceState) 
    {
        //Initialize large static variables
        //This MUST go here. Putting it in the class itself will cause:
        //  java.lang.VerifyError: Rejecting class com.finjin.engine.FinjinNativeActivity because it failed compile-time verification (declaration of 'com.finjin.engine.FinjinNativeActivity' appears in /data/internal/app/com.finjin.app-2/base.apk)
        //Oddly, this will not happen if the array only contains around 10 elements
        if (gameControllerButtonMapping == null)
        {
            gameControllerButtonMapping = new int[]
            {
                KeyEvent.KEYCODE_BUTTON_1,
                KeyEvent.KEYCODE_BUTTON_2,
                KeyEvent.KEYCODE_BUTTON_3,
                KeyEvent.KEYCODE_BUTTON_4,
                KeyEvent.KEYCODE_BUTTON_5,
                KeyEvent.KEYCODE_BUTTON_6,
                KeyEvent.KEYCODE_BUTTON_7,
                KeyEvent.KEYCODE_BUTTON_8,
                KeyEvent.KEYCODE_BUTTON_9,
                KeyEvent.KEYCODE_BUTTON_10,
                KeyEvent.KEYCODE_BUTTON_11,
                KeyEvent.KEYCODE_BUTTON_12,
                KeyEvent.KEYCODE_BUTTON_13,
                KeyEvent.KEYCODE_BUTTON_14,
                KeyEvent.KEYCODE_BUTTON_15,
                KeyEvent.KEYCODE_BUTTON_16,
                KeyEvent.KEYCODE_BUTTON_A,
                KeyEvent.KEYCODE_BUTTON_B,
                KeyEvent.KEYCODE_BUTTON_C,
                KeyEvent.KEYCODE_BUTTON_X,
                KeyEvent.KEYCODE_BUTTON_Y,
                KeyEvent.KEYCODE_BUTTON_Z,
                KeyEvent.KEYCODE_BUTTON_L1,
                KeyEvent.KEYCODE_BUTTON_L2,
                KeyEvent.KEYCODE_BUTTON_R1,
                KeyEvent.KEYCODE_BUTTON_R2,
                KeyEvent.KEYCODE_BUTTON_THUMBL,
                KeyEvent.KEYCODE_BUTTON_THUMBR,
                KeyEvent.KEYCODE_BUTTON_START,
                KeyEvent.KEYCODE_BUTTON_SELECT,
                KeyEvent.KEYCODE_BUTTON_MODE,
                KeyEvent.KEYCODE_HOME,
                KeyEvent.KEYCODE_DPAD_UP,
                KeyEvent.KEYCODE_DPAD_DOWN,
                KeyEvent.KEYCODE_DPAD_LEFT,
                KeyEvent.KEYCODE_DPAD_RIGHT,
                KeyEvent.KEYCODE_DPAD_CENTER,
                KeyEvent.KEYCODE_MENU,
                KeyEvent.KEYCODE_BACK
            };
        }
        
        Intent launchIntent = getIntent();

        this.assetManager = getAssets();

        this.packageName = getApplicationContext().getPackageName();

        this.userDocumentsDirectory = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOCUMENTS).getAbsolutePath();
        this.userMusicDirectory = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_MUSIC).getAbsolutePath();
        this.userVideosDirectory = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_MOVIES).getAbsolutePath();
        this.userPicturesDirectory = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES).getAbsolutePath();
        this.userCameraRollDirectory = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM).getAbsolutePath();
        this.userDownloadsDirectory = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS).getAbsolutePath();                
        this.userApplicationTemporaryDirectory = getCacheDir().getAbsolutePath();

        this.systemDataDirectory = Environment.getDataDirectory().getAbsolutePath();
        this.systemExternalStorageDirectory = Environment.getExternalStorageDirectory().getAbsolutePath();
        
        updateDisplaySize();

        refreshKeycodes();
        refreshGameControllers();
        refreshMice();
        refreshKeyboards();
        
        refreshApkPaths();

        super.onCreate(savedInstanceState);
    }
    
    @Override
    protected void onResume() 
    {
        hideSystemUI();

        super.onResume();
    }
     
    private AssetManager assetManager;

    public String packageName;

    public int displayWidthPixels;
    public int displayHeightPixels;

    public String manufacturer = Build.MANUFACTURER;
    public String model = Build.MODEL;

    public String[] keyCodeStrings;
    public void refreshKeycodes()
    {
        this.keyCodeStrings = new String[300]; //This matches KeyboardConstants::MAX_BUTTON_COUNT in InputConstants.hpp. It doesn't have to match but it should
        for (int i = 0; i < this.keyCodeStrings.length; i++)
        {
            try
            {
                this.keyCodeStrings[i] = KeyEvent.keyCodeToString(i);
            }
            catch (Exception ex)
            {
                this.keyCodeStrings[i] = "";
            }
        }
    }

    //Keyboard stuff-----------------------------------------------------------------------------------
    public static final int MAX_KEYBOARDS = 16;

    public int keyboardId;
    public String keyboardName;
    public String keyboardInstanceDescriptor;
    public String keyboardProductDescriptor;
    public boolean keyboardIsVirtual;
    
    private InputDevice[] keyboardDevices = new InputDevice[MAX_KEYBOARDS];    

    private int[] keyboardIds = new int[MAX_KEYBOARDS];
    private InputDeviceInfo[] keyboardsInfo = new InputDeviceInfo[MAX_KEYBOARDS];
    public int keyboardCount = 0;
    
    private int[] nextKeyboardIds = new int[MAX_KEYBOARDS];
    private int nextKeyboardCount = 0;

    public int connectedKeyboardCount = 0;
    public int[] connectedKeyboardIds = new int[MAX_KEYBOARDS];

    public int disconnectedKeyboardCount = 0;
    public int[] disconnectedKeyboardIds = new int[MAX_KEYBOARDS];

    public boolean checkKeyboardConfigurationChanged()
    {
        this.nextKeyboardCount = findAllKeyboards(this.nextKeyboardIds, null);

        //Find newly disconnected devices
        this.disconnectedKeyboardCount = 0;
        for (int i = 0; i < this.keyboardCount; i++)
        {
            if (getIndexOfDeviceId(this.keyboardIds[i], this.nextKeyboardIds, this.nextKeyboardCount) < 0)
                this.disconnectedKeyboardIds[this.disconnectedKeyboardCount++] = this.keyboardIds[i];
        }

        //Find newly connected devices
        this.connectedKeyboardCount = 0;
        for (int i = 0; i < this.nextKeyboardCount; i++)
        {
            if (getIndexOfDeviceId(this.nextKeyboardIds[i], this.keyboardIds, this.keyboardCount) < 0)
                this.connectedKeyboardIds[this.connectedKeyboardCount++] = this.nextKeyboardIds[i];
        }

        return this.disconnectedKeyboardCount > 0 || this.connectedKeyboardCount > 0;
    }

    public static int findAllKeyboards(int ids[], InputDevice devices[])
    {
        //android.util.Log.i("finjin-engine", "Java searching for keyboards");
        return findAllInputSources(ids, devices, InputDevice.SOURCE_KEYBOARD);
    }

    public void refreshKeyboards()
    {
        this.nextKeyboardCount = 0;
        this.connectedKeyboardCount = 0;
        this.disconnectedKeyboardCount = 0;

        this.keyboardCount = findAllKeyboards(this.keyboardIds, this.keyboardDevices);

        for (int i = 0; i < this.keyboardCount; i++)
        {
            InputDevice keyboardDevice = this.keyboardDevices[i];

            this.keyboardsInfo[i] = new InputDeviceInfo(keyboardDevice);            
        }

        for (int i = this.keyboardCount; i < MAX_KEYBOARDS; i++)
            this.keyboardDevices[i] = null;
            
        selectKeyboard(0);
    }

    public void selectKeyboard(int index)
    {        
        if (index >= 0 && index < this.keyboardCount)
        {
            this.keyboardId = this.keyboardIds[index];
            this.keyboardName = this.keyboardsInfo[index].deviceName;
            this.keyboardInstanceDescriptor = this.keyboardsInfo[index].instanceDescriptor;
            this.keyboardProductDescriptor = this.keyboardsInfo[index].productDescriptor;
            this.keyboardIsVirtual = this.keyboardsInfo[index].isVirtual;
        }
        else
        {
            this.keyboardId = 0;
            this.keyboardName = null;
            this.keyboardInstanceDescriptor = null;
            this.keyboardProductDescriptor = null;
            this.keyboardIsVirtual = false;
        }
    }

    //Mouse stuff-----------------------------------------------------------------------------------
    
    public static final int MAX_MICE = 16;

    public int mouseId;
    public String mouseName;
    public String mouseInstanceDescriptor;
    public String mouseProductDescriptor;
    public boolean mouseIsVirtual;
    public String[] mouseAxisNames;
    public int[] mouseAxisIndices;
    public float[] mouseAxisMinVals;
    public float[] mouseAxisMaxVals; 
    public float[] mouseAxisFlats; 
    public float[] mouseAxisFuzzes; 
    public int[] mouseButtonIndices = null;
    public String[] mouseButtonNames = null;

    private InputDevice[] mouseDevices = new InputDevice[MAX_MICE];    

    private int[] mouseIds = new int[MAX_MICE];
    private InputDeviceInfo[] mousesInfo = new InputDeviceInfo[MAX_MICE];
    public int mouseCount = 0;
    
    private int[] nextMouseIds = new int[MAX_MICE];
    private int nextMouseCount = 0;

    public int connectedMouseCount = 0;
    public int[] connectedMouseIds = new int[MAX_MICE];

    public int disconnectedMouseCount = 0;
    public int[] disconnectedMouseIds = new int[MAX_MICE];

    private static int[] mouseButtonMapping =
    {
        KeyEvent.KEYCODE_BUTTON_1,
        KeyEvent.KEYCODE_BUTTON_2,
        KeyEvent.KEYCODE_BUTTON_3,
        KeyEvent.KEYCODE_BUTTON_4,
        KeyEvent.KEYCODE_BUTTON_5,
        KeyEvent.KEYCODE_BUTTON_6,
        KeyEvent.KEYCODE_BUTTON_7,
        KeyEvent.KEYCODE_BUTTON_8
    };

    public boolean checkMouseConfigurationChanged()
    {
        this.nextMouseCount = findAllMouses(this.nextMouseIds, null);

        //Find newly disconnected devices
        this.disconnectedMouseCount = 0;
        for (int i = 0; i < this.mouseCount; i++)
        {
            if (getIndexOfDeviceId(this.mouseIds[i], this.nextMouseIds, this.nextMouseCount) < 0)
                this.disconnectedMouseIds[this.disconnectedMouseCount++] = this.mouseIds[i];
        }

        //Find newly connected devices
        this.connectedMouseCount = 0;
        for (int i = 0; i < this.nextMouseCount; i++)
        {
            if (getIndexOfDeviceId(this.nextMouseIds[i], this.mouseIds, this.mouseCount) < 0)
                this.connectedMouseIds[this.connectedMouseCount++] = this.nextMouseIds[i];
        }

        return this.disconnectedMouseCount > 0 || this.connectedMouseCount > 0;
    }

    public static int findAllMouses(int ids[], InputDevice devices[])
    {
        //android.util.Log.i("finjin-engine", "Java searching for mice");
        return findAllInputSources(ids, devices, InputDevice.SOURCE_MOUSE);
    }
        
    public void refreshMice()
    {
        this.nextMouseCount = 0;
        this.connectedMouseCount = 0;
        this.disconnectedMouseCount = 0;

        this.mouseCount = findAllMouses(this.mouseIds, this.mouseDevices);

        for (int i = 0; i < this.mouseCount; i++)
        {
            InputDevice mouseDevice = this.mouseDevices[i];

            this.mousesInfo[i] = new InputDeviceInfo(mouseDevice, mouseButtonMapping);            
        }

        for (int i = this.mouseCount; i < MAX_MICE; i++)
            this.mouseDevices[i] = null;
            
        selectMouse(0);
    }

    public void selectMouse(int index)
    {        
        if (index >= 0 && index < this.mouseCount)
        {
            this.mouseId = this.mouseIds[index];
            this.mouseName = this.mousesInfo[index].deviceName;
            this.mouseInstanceDescriptor = this.mousesInfo[index].instanceDescriptor;
            this.mouseProductDescriptor = this.mousesInfo[index].productDescriptor;
            this.mouseIsVirtual = this.mousesInfo[index].isVirtual;
            this.mouseAxisNames = this.mousesInfo[index].axisNames;
            this.mouseAxisIndices = this.mousesInfo[index].axisIndices;
            this.mouseAxisMinVals = this.mousesInfo[index].axisMinVals;
            this.mouseAxisMaxVals = this.mousesInfo[index].axisMaxVals;                        
            this.mouseAxisFlats = this.mousesInfo[index].axisFlats;
            this.mouseAxisFuzzes = this.mousesInfo[index].axisFuzzes;
            this.mouseButtonIndices = this.mousesInfo[index].buttonIndices;

            if (this.mouseButtonIndices != null)
            {   
                this.mouseButtonNames = new String[this.mouseButtonIndices.length];
                for (int i = 0; i < this.mouseButtonNames.length; i++)
                    this.mouseButtonNames[i] = this.keyCodeStrings[this.mouseButtonIndices[i]];
            }
            else
                this.mouseButtonNames = null;    
        }
        else
        {
            this.mouseId = 0;
            this.mouseName = null;
            this.mouseInstanceDescriptor = null;
            this.mouseProductDescriptor = null;
            this.mouseIsVirtual = false;
            this.mouseAxisNames = null;
            this.mouseAxisIndices = null;
            this.mouseAxisMinVals = null;
            this.mouseAxisMaxVals = null;
            this.mouseAxisFlats = null;
            this.mouseAxisFuzzes = null;
            this.mouseButtonIndices = null;
            this.mouseButtonNames = null;
        }
    }

    //Game controller stuff---------------------------------------------------------------------------------

    public static final int MAX_GAME_CONTROLLERS = 16; //See MAX_NATIVE_GAME_CONTROLLERS in AndroidGameController.hpp

    public int gameControllerId;
    public String gameControllerName;
    public int gameControllerNumber;
    public String gameControllerInstanceDescriptor;
    public String gameControllerProductDescriptor;
    public boolean gameControllerIsVirtual;
    public String[] gameControllerAxisNames;
    public int[] gameControllerAxisIndices;
    public float[] gameControllerAxisMinVals;
    public float[] gameControllerAxisMaxVals; 
    public float[] gameControllerAxisFlats; 
    public float[] gameControllerAxisFuzzes; 
    public int[] gameControllerButtonIndices;
    public String[] gameControllerButtonNames;
    
    private InputDevice[] gameControllerDevices = new InputDevice[MAX_GAME_CONTROLLERS];    

    private int[] gameControllerIds = new int[MAX_GAME_CONTROLLERS];
    private InputDeviceInfo[] gameControllersInfo = new InputDeviceInfo[MAX_GAME_CONTROLLERS];
    public int gameControllerCount = 0;
    
    private int[] nextGameControllerIds = new int[MAX_GAME_CONTROLLERS];
    private int nextGameControllerCount = 0;

    public int connectedGameControllerCount = 0;
    public int[] connectedGameControllerIds = new int[MAX_GAME_CONTROLLERS];

    public int disconnectedGameControllerCount = 0;
    public int[] disconnectedGameControllerIds = new int[MAX_GAME_CONTROLLERS];

    private static int[] gameControllerButtonMapping;

    public boolean checkGameControllerConfigurationChanged()
    {
        this.nextGameControllerCount = findAllGameControllers(this.nextGameControllerIds, null);

        //Find newly disconnected devices
        this.disconnectedGameControllerCount = 0;
        for (int i = 0; i < this.gameControllerCount; i++)
        {
            if (getIndexOfDeviceId(this.gameControllerIds[i], this.nextGameControllerIds, this.nextGameControllerCount) < 0)
                this.disconnectedGameControllerIds[this.disconnectedGameControllerCount++] = this.gameControllerIds[i];
        }

        //Find newly connected devices
        this.connectedGameControllerCount = 0;
        for (int i = 0; i < this.nextGameControllerCount; i++)
        {
            if (getIndexOfDeviceId(this.nextGameControllerIds[i], this.gameControllerIds, this.gameControllerCount) < 0)
                this.connectedGameControllerIds[this.connectedGameControllerCount++] = this.nextGameControllerIds[i];
        }

        return this.disconnectedGameControllerCount > 0 || this.connectedGameControllerCount > 0;
    }

    public static int findAllGameControllers(int ids[], InputDevice devices[])
    {
        //android.util.Log.i("finjin-engine", "Java searching for game controllers");
        return findAllInputSources(ids, devices, InputDevice.SOURCE_JOYSTICK | InputDevice.SOURCE_GAMEPAD);        
    }

    public void refreshGameControllers()
    {
        this.nextGameControllerCount = 0;
        this.connectedGameControllerCount = 0;
        this.disconnectedGameControllerCount = 0;

        this.gameControllerCount = findAllGameControllers(this.gameControllerIds, this.gameControllerDevices);

        for (int i = 0; i < this.gameControllerCount; i++)
        {
            InputDevice gameControllerDevice = this.gameControllerDevices[i];

            this.gameControllersInfo[i] = new InputDeviceInfo(gameControllerDevice, gameControllerButtonMapping);            
        }

        for (int i = this.gameControllerCount; i < MAX_GAME_CONTROLLERS; i++)
            this.gameControllerDevices[i] = null;
            
        selectGameController(0);
    }

    public void selectGameController(int index)
    {        
        if (index >= 0 && index < this.gameControllerCount)
        {
            this.gameControllerId = this.gameControllerIds[index];
            this.gameControllerName = this.gameControllersInfo[index].deviceName;
            this.gameControllerNumber = this.gameControllersInfo[index].controllerNumber;
            this.gameControllerInstanceDescriptor = this.gameControllersInfo[index].instanceDescriptor;
            this.gameControllerProductDescriptor = this.gameControllersInfo[index].productDescriptor;
            this.gameControllerIsVirtual = this.gameControllersInfo[index].isVirtual;
            this.gameControllerAxisNames = this.gameControllersInfo[index].axisNames;
            this.gameControllerAxisIndices = this.gameControllersInfo[index].axisIndices;
            this.gameControllerAxisMinVals = this.gameControllersInfo[index].axisMinVals;
            this.gameControllerAxisMaxVals = this.gameControllersInfo[index].axisMaxVals;
            this.gameControllerAxisFlats = this.gameControllersInfo[index].axisFlats;
            this.gameControllerAxisFuzzes = this.gameControllersInfo[index].axisFuzzes;
            this.gameControllerButtonIndices = this.gameControllersInfo[index].buttonIndices;

            if (this.gameControllerButtonIndices != null)
            {   
                this.gameControllerButtonNames = new String[this.gameControllerButtonIndices.length];
                for (int i = 0; i < this.gameControllerButtonNames.length; i++)
                    this.gameControllerButtonNames[i] = this.keyCodeStrings[this.gameControllerButtonIndices[i]];
            }
            else
                this.gameControllerButtonNames = null;    
        }
        else
        {
            this.gameControllerId = 0;
            this.gameControllerName = null;
            this.gameControllerNumber = 0;
            this.gameControllerInstanceDescriptor = null;
            this.gameControllerProductDescriptor = null;
            this.gameControllerIsVirtual = false;
            this.gameControllerAxisNames = null;
            this.gameControllerAxisIndices = null;
            this.gameControllerAxisMinVals = null;
            this.gameControllerAxisMaxVals = null;
            this.gameControllerAxisFlats = null;
            this.gameControllerAxisFuzzes = null;
            this.gameControllerButtonIndices = null;
            this.gameControllerButtonNames = null;
        }
    }


    //Storage stuff------------------------------------------------------------------------------
    public String userDocumentsDirectory;
    public String userMusicDirectory;
    public String userVideosDirectory;
    public String userPicturesDirectory;
    public String userCameraRollDirectory;
    public String userDownloadsDirectory;
    public String userApplicationTemporaryDirectory;

    public String systemDataDirectory;
    public String systemExternalStorageDirectory;
        
    public boolean isExternalStorageReadyForRead()
    {
        String state = Environment.getExternalStorageState();
        return Environment.MEDIA_MOUNTED.equals(state) || Environment.MEDIA_MOUNTED_READ_ONLY.equals(state);
    }

    public boolean isExternalStorageReadyForWrite()
    {
        String state = Environment.getExternalStorageState();
        return Environment.MEDIA_MOUNTED.equals(state);
    }
    
    
    //These methods are called by native code------------------------------------------------------

    public void openUrl(String url)
    {
        Uri uriUrl = Uri.parse(url); 
        Intent intent = new Intent(Intent.ACTION_VIEW, uriUrl); 
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK); 
        startActivity(intent); 
    } 

    public native void handleTextInputReturnValue(int callerId, String text);
    
    private float dipsToPixels(float dipValue) 
    {
        return android.util.TypedValue.applyDimension(android.util.TypedValue.COMPLEX_UNIT_DIP, dipValue, getResources().getDisplayMetrics());
    }

    private static final int VALUE_STRING = 0;
    private static final int VALUE_INT = 1;
    private static final int VALUE_FLOAT = 2;

    private static final int INPUT_FLAG_CAP_SENTENCES = 0x1;
    private static final int INPUT_FLAG_SUGGESTIONS = 0x2;
    private static final int INPUT_FLAG_MULTILINE = 0x4;
    private static final int INPUT_FLAG_LONG_MESSAGE = 0x8;

    public void showTextInputDialog
        (
        final int callerId, 
        final String title, 
        final String message, 
        final String hint, 
        final String initialValue, 
        final int valueType, 
        final int maxLength, 
        final float minValue, 
        final float maxValue, 
        final int inputFlags
        )
    {
        final android.content.Context context = this;

        runOnUiThread(new Runnable() 
        {
            public void run()
            {
                int padding = (int)dipsToPixels(10.0f);                

                android.widget.LinearLayout layout = new android.widget.LinearLayout(context);
                layout.setOrientation(android.widget.LinearLayout.VERTICAL);
                layout.setGravity(android.view.Gravity.CENTER_HORIZONTAL);
                layout.setPadding(padding, 0, padding, padding);

                final android.widget.EditText editText = new android.widget.EditText(context);
                int editTextFlags = 0;
                if (valueType == VALUE_INT || valueType == VALUE_FLOAT)
                {
                    editTextFlags |= android.text.InputType.TYPE_CLASS_NUMBER;
                    if (valueType == VALUE_FLOAT)
                        editTextFlags |= android.text.InputType.TYPE_NUMBER_FLAG_DECIMAL;
                    if (minValue < 0)
                        editTextFlags |= android.text.InputType.TYPE_NUMBER_FLAG_SIGNED;                    
                }
                else
                {
                    editTextFlags |= android.text.InputType.TYPE_CLASS_TEXT;
                    if ((inputFlags & INPUT_FLAG_CAP_SENTENCES) != 0)
                        editTextFlags |= android.text.InputType.TYPE_TEXT_FLAG_CAP_SENTENCES;
                    if ((inputFlags & INPUT_FLAG_SUGGESTIONS) == 0)
                        editTextFlags |= android.text.InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS;
                    if ((inputFlags & INPUT_FLAG_MULTILINE) != 0)
                        editTextFlags |= android.text.InputType.TYPE_TEXT_FLAG_MULTI_LINE;
                    if ((inputFlags & INPUT_FLAG_LONG_MESSAGE) != 0)
                        editTextFlags |= android.text.InputType.TYPE_TEXT_VARIATION_LONG_MESSAGE;                        
                    if (maxLength > 0)
                        editText.setFilters( new android.text.InputFilter[] { new android.text.InputFilter.LengthFilter(maxLength)} );
                }
                editText.setInputType(editTextFlags);

                editText.setHint(hint);
                editText.setText(initialValue);

                layout.addView(editText);                

                new android.app.AlertDialog.Builder(context)
                    .setTitle(title)
                    .setMessage(message)
                    .setView(layout)
                    .setPositiveButton(getString(android.R.string.ok), new android.content.DialogInterface.OnClickListener() 
                    {
                        public void onClick(android.content.DialogInterface dialog, int whichButton) 
                        {
                            String text = editText.getText().toString();
                            handleTextInputReturnValue(callerId, text);
                        }
                    })
                    .setNegativeButton(getString(android.R.string.cancel), new android.content.DialogInterface.OnClickListener() 
                    {
                        public void onClick(android.content.DialogInterface dialog, int whichButton) 
                        {
                            handleTextInputReturnValue(callerId, null);
                        }
                    })
                    .show();
            }
        });        
    }

    public void copyTextToClipboard(final String title, final String text)
    {
        runOnUiThread(new Runnable() 
        {
            public void run()
            {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB)
                {
                    //Newer interface
                    android.content.ClipboardManager clipboardManager = (android.content.ClipboardManager)getSystemService(Context.CLIPBOARD_SERVICE);
                    android.content.ClipData clipData = android.content.ClipData.newPlainText(title, text);
                    clipboardManager.setPrimaryClip(clipData);
                }
                else
                {
                    //Old interface
                    android.text.ClipboardManager clipboardManager = (android.text.ClipboardManager)getSystemService(Context.CLIPBOARD_SERVICE);
                    clipboardManager.setText(text);
                }
            }
        });        
    }


    //APK assets--------------------------------------------------------------------------
    private static final int ASSET_TYPE_FILE = 0;
    private static final int ASSET_TYPE_DIRECTORY = 1;

    private static final boolean GET_ASSET_FILE_SIZE = false;

    public int apkAssetPathCount;
    public String[] apkAssetPaths;    
    public int[] apkAssetPathTypes; //ASSET_TYPE_FILE or ASSET_TYPE_DIRECTORY
    public long[] apkAssetPathSizes;

    private void refreshApkPaths()
    {
        ArrayList<String> apkAssetPathsList = new ArrayList<String>(100);
        ArrayList<Integer> apkAssetPathTypesList = new ArrayList<Integer>(100);
        ArrayList<Long> apkAssetPathSizesList = new ArrayList<Long>(100);

        this.apkAssetPathCount = 0;

        enumerateApkPaths(apkAssetPathsList, apkAssetPathTypesList, apkAssetPathSizesList, ""); //Start at the root

        this.apkAssetPathCount = apkAssetPathsList.size();

        //Convert enumerated values into something easier for JNI to read
        if (this.apkAssetPaths == null || this.apkAssetPathCount > this.apkAssetPaths.length)
            this.apkAssetPaths = new String[this.apkAssetPathCount];    
        if (this.apkAssetPathTypes == null || this.apkAssetPathCount > this.apkAssetPathTypes.length)
            this.apkAssetPathTypes = new int[this.apkAssetPathCount];
        if (this.apkAssetPathSizes == null || this.apkAssetPathCount > this.apkAssetPathSizes.length)
            this.apkAssetPathSizes = new long[this.apkAssetPathCount];
        for (int i = 0; i < this.apkAssetPathCount; i++)
        {
            this.apkAssetPaths[i] = apkAssetPathsList.get(i);
            this.apkAssetPathTypes[i] = apkAssetPathTypesList.get(i);
            this.apkAssetPathSizes[i] = apkAssetPathSizesList.get(i);
        }
    }

    private void enumerateApkPaths(ArrayList<String> apkAssetPathsList, ArrayList<Integer> apkAssetPathTypesList, ArrayList<Long> apkAssetPathSizesList, String directoryRelativePath)
    {
        String[] items = null;
        try
        {
            items = this.assetManager.list(directoryRelativePath);
        }
        catch (IOException ex)
        {
            //It's not a directory. Fix the previously added entry
            if (!apkAssetPathsList.isEmpty())
            {
                int parentDirectoryIndex = apkAssetPathsList.size();
                apkAssetPathTypesList.set(parentDirectoryIndex, ASSET_TYPE_FILE);
                apkAssetPathTypesList.set(parentDirectoryIndex, -1);
            }

            return;
        }

        //It's really a directory
        for (int i = 0; i < items.length; i++)
        {
            String fullPath;
            if (directoryRelativePath.length() == 0)
                fullPath = items[i];
            else
                fullPath = directoryRelativePath + "/" + items[i];

            int assetType = ASSET_TYPE_DIRECTORY;
            long length = 0; //0 = directory, -1 = unknown
            try 
            {
                if (items[i].contains(".")) //This is a pretty crappy way of determining whether the entry is a file
                {
                    if (GET_ASSET_FILE_SIZE)
                    {
                        AssetFileDescriptor descriptor = this.assetManager.openFd(fullPath);
                        length = descriptor.getDeclaredLength();
                        descriptor.close();

                        assetType = ASSET_TYPE_FILE;    
                    }
                    else
                    {
                        length = -1;                    
                        assetType = ASSET_TYPE_FILE;    
                    }
                }
            }
            catch (IOException ex)
            {
                //This shouldn't happen. Do nothing
            }

            apkAssetPathsList.add(fullPath);
            apkAssetPathSizesList.add(length);            
            apkAssetPathTypesList.add(assetType);

            if (assetType == ASSET_TYPE_DIRECTORY)
                enumerateApkPaths(apkAssetPathsList, apkAssetPathTypesList, apkAssetPathSizesList, fullPath);
        }
    }


    //Other stuff--------------------------------------------------------------------------
    private static int findAllInputSources(int ids[], InputDevice devices[], int sourceType)
    {
        int maxDeviceCount = ids.length;

        int foundCount = 0;

        int[] deviceIds = InputDevice.getDeviceIds(); 
        for (int i = 0; i < deviceIds.length && foundCount < maxDeviceCount; i++) 
        {
            InputDevice device = InputDevice.getDevice(deviceIds[i]);
            
            int deviceSources = device.getSources() & ~InputDevice.SOURCE_CLASS_MASK; //Mask out the device classes
            if ((deviceSources & sourceType) != 0) 
            {
                if (devices != null)
                    devices[foundCount] = device;
                ids[foundCount] = deviceIds[i];
                
                //android.util.Log.i("finjin-engine", "Java found device id: " + deviceIds[i]);

                foundCount++;
            }
        }

        return foundCount;
    }

    private static int getIndexOfDeviceId(int id, int[] deviceIds, int deviceCount)
    {
        for (int i = 0; i < deviceCount; i++)
        {
            if (deviceIds[i] == id)
                return i;
        }
        return -1;
    }

    public void updateDisplaySize() 
    {
        android.util.DisplayMetrics metrics = new android.util.DisplayMetrics();
        android.view.Display display = getWindowManager().getDefaultDisplay();
        display.getMetrics(metrics);
        this.displayWidthPixels = metrics.widthPixels;
        this.displayHeightPixels = metrics.heightPixels;
    }

    public void hideSystemUI() 
    {
        getWindow().getDecorView().setSystemUiVisibility(
            android.view.View.SYSTEM_UI_FLAG_LAYOUT_STABLE |  
            android.view.View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION | 
            android.view.View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |  
            android.view.View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
            android.view.View.SYSTEM_UI_FLAG_FULLSCREEN |
            android.view.View.SYSTEM_UI_FLAG_IMMERSIVE);
    }
    
    public void showSystemUI() 
    {
        getWindow().getDecorView().setSystemUiVisibility(
            android.view.View.SYSTEM_UI_FLAG_LAYOUT_STABLE | 
            android.view.View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION | 
            android.view.View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN);
    }

    static class InputDeviceInfo
    {
        public String deviceName;
        public int controllerNumber;
        public String instanceDescriptor;
        public String productDescriptor;
        public boolean isVirtual;
        public String[] axisNames;
        public int[] axisIndices;
        public float[] axisMinVals;
        public float[] axisMaxVals; 
        public float[] axisFlats;
        public float[] axisFuzzes;
        public int[] buttonIndices;
        
        public InputDeviceInfo(InputDevice dev)
        {
            this(dev, null);
        }

        public InputDeviceInfo(InputDevice dev, int[] buttonsToCheck)
        {
            this.deviceName = dev.getName();

            this.controllerNumber = dev.getControllerNumber();

            this.instanceDescriptor = dev.getDescriptor();
            
            this.productDescriptor = "vid" + dev.getVendorId() + "-pid" + dev.getProductId();

            this.isVirtual = dev.isVirtual();

            List<InputDevice.MotionRange> ranges = dev.getMotionRanges();
            int axisCount = ranges.size();
            this.axisNames = new String[axisCount];
            this.axisIndices = new int[axisCount];
            this.axisMinVals = new float[axisCount];
            this.axisMaxVals = new float[axisCount];
            this.axisFlats = new float[axisCount];
            this.axisFuzzes = new float[axisCount];
            
            int axisIndex = 0;        
            Iterator<InputDevice.MotionRange> iterator = ranges.iterator();
            while (iterator.hasNext())
            {
                InputDevice.MotionRange range = iterator.next();

                this.axisNames[axisIndex] = MotionEvent.axisToString(range.getAxis());
                this.axisIndices[axisIndex] = range.getAxis();
                this.axisMinVals[axisIndex] = range.getMin();
                this.axisMaxVals[axisIndex] = range.getMax();
                this.axisFlats[axisIndex] = range.getFlat();
                this.axisFuzzes[axisIndex] = range.getFuzz();

                axisIndex++;
            }

            if (buttonsToCheck != null)
            {
                boolean[] hasButtons = dev.hasKeys(buttonsToCheck);

                int hasButtonCount = 0;
                for (int i = 0; i < hasButtons.length; i++)
                {
                    if (hasButtons[i])
                        hasButtonCount++;
                }

                if (hasButtonCount > 0)
                {
                    this.buttonIndices = new int[hasButtonCount];
                    
                    hasButtonCount = 0;                    
                    for (int i = 0; i < hasButtons.length; i++)
                    {
                        if (hasButtons[i])
                            this.buttonIndices[hasButtonCount++] = buttonsToCheck[i];   
                    }
                }
            }
        }
    }

}
