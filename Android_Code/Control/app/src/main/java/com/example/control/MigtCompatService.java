package com.example.control;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.Process;
import android.util.Log;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import android.os.Build;  // 添加这行
//显示


public class MigtCompatService extends Service {
    private static final String TAG = "MigtCompatService";
    private static final String MIGT_NODE_PATH = "/sys/module/metis/parameters/minor_window_app";

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(TAG, "Service started for MIUI compatibility");

        // 在子线程中处理MIUI兼容性问题
        new Thread(() -> {
            checkAndHandleMigtNode();
            checkMiuiOptimizations();
        }).start();

        return START_STICKY;
    }

    private void checkAndHandleMigtNode() {
        try {
            File migtFile = new File(MIGT_NODE_PATH);
            if (migtFile.exists()) {
                Log.d(TAG, "MIGT node exists, trying to read");

                // 尝试读取节点内容
                try (BufferedReader reader = new BufferedReader(new FileReader(migtFile))) {
                    String line = reader.readLine();
                    Log.d(TAG, "MIGT node content: " + line);
                }
            } else {
                Log.w(TAG, "MIGT node does not exist at: " + MIGT_NODE_PATH);
            }
        } catch (IOException e) {
            Log.e(TAG, "Error accessing MIGT node", e);
        } catch (SecurityException e) {
            Log.e(TAG, "Security exception when accessing MIGT node", e);
            // 这里可以添加备用方案
        }
    }

    private void checkMiuiOptimizations() {
        if (isMiui()) {
            Log.d(TAG, "Running on MIUI, checking optimizations");

            // 检查并申请必要的MIUI特殊权限
            if (!isMiuiBackgroundPermissionGranted()) {
                requestMiuiBackgroundPermission();
            }
        }
    }

    private boolean isMiui() {
        return "Xiaomi".equalsIgnoreCase(Build.MANUFACTURER)
                || "xiaomi".equalsIgnoreCase(Build.BRAND);
    }

    private boolean isMiuiBackgroundPermissionGranted() {
        // 这里实现检查MIUI后台权限是否已授予的逻辑
        // 可能需要使用反射或其他方法检查
        return false; // 默认返回false以确保请求权限
    }

    private void requestMiuiBackgroundPermission() {
        try {
            // MIUI自启动权限
            Intent autostartIntent = new Intent("miui.intent.action.OP_AUTO_START");
            autostartIntent.addCategory(Intent.CATEGORY_DEFAULT);
            autostartIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            startActivity(autostartIntent);

            // MIUI电池优化
            Intent batteryIntent = new Intent("miui.intent.action.POWER_HIDE_MODE_APP_LIST");
            batteryIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            startActivity(batteryIntent);

            Log.d(TAG, "Requested MIUI special permissions");
        } catch (Exception e) {
            Log.e(TAG, "Failed to request MIUI permissions", e);
        }
    }

    @Override
    public void onDestroy() {
        Log.d(TAG, "Service destroyed");
        super.onDestroy();
    }
}