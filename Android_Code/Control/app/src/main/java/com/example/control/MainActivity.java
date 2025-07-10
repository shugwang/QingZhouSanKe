package com.example.control;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Handler;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.*;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import org.json.JSONObject;

import java.text.SimpleDateFormat;
import java.util.*;

// 省略 import 保持清爽，上面你已有

public class MainActivity extends AppCompatActivity {
    // MQTT相关
    private String ip = "tcp://47.111.93.229:1883";
    private String userName = "1234", passWord = "1234", Id = "app" + System.currentTimeMillis();
    private String mqtt_sub_topic = "pubTopic", mqtt_pub_topic = "subTopic";
    private MqttClient mqtt_client;
    private MqttConnectOptions options;

    // UI控件
    private TextView tvPHValue, tvOxygen, tvTurbidity, tvTemperature, tvConductivity, tvLastUpdate;
    private TextView tvLeftMotorValue, tvRightMotorValue;
    private SeekBar leftMotorSeekBar, rightMotorSeekBar;
    private Button btnEmergencyLeft, btnEmergencyRight, btnAutoCruise, btnAutoSample;
    private EditText etCValue, etDValue;

    // 状态标志
    private boolean emergencyLeftPressed = false;
    private boolean emergencyRightPressed = false;
    private boolean isWaterOperationInProgress = false;
    private long lastMotorCommandTime = 0;
    private static final long MOTOR_COMMAND_INTERVAL_MS = 200;
    private long lastWaterOperationTime = 0;
    private int lastWaterOperationDuration = 0;

    private int leftMotorSpeed = 0, rightMotorSpeed = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initViews();
        setupMotorControls();
        setupButtonListeners();
        mqtt_init_Connect();
        checkAndRequestPermissions();
    }

    private void initViews() {
        tvPHValue = findViewById(R.id.tvPHValue);
        tvOxygen = findViewById(R.id.tvOxygen);
        tvTurbidity = findViewById(R.id.tvTurbidity);
        tvTemperature = findViewById(R.id.tvTemperature);
        tvConductivity = findViewById(R.id.tvConductivity);
        tvLastUpdate = findViewById(R.id.tvLastUpdate);
        tvLeftMotorValue = findViewById(R.id.tvLeftMotorValue);
        tvRightMotorValue = findViewById(R.id.tvRightMotorValue);

        leftMotorSeekBar = findViewById(R.id.leftMotorSeekBar);
        rightMotorSeekBar = findViewById(R.id.rightMotorSeekBar);
        btnEmergencyLeft = findViewById(R.id.btnEmergencyLeft);
        btnEmergencyRight = findViewById(R.id.btnEmergencyRight);
        btnAutoCruise = findViewById(R.id.btnAutoCruise);
        btnAutoSample = findViewById(R.id.btnAutoSample);
        etCValue = findViewById(R.id.etCValue);
        etDValue = findViewById(R.id.etDValue);
    }

    private void setupMotorControls() {
        leftMotorSeekBar.setMax(254);
        leftMotorSeekBar.setProgress(127);
        rightMotorSeekBar.setMax(254);
        rightMotorSeekBar.setProgress(127);

        leftMotorSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                leftMotorSpeed = progress - 127;
                updateLeftMotorDisplay();
                if (!isWaterOperationInProgress) sendMotorCommand();
            }
            public void onStartTrackingTouch(SeekBar seekBar) {}
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        rightMotorSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                rightMotorSpeed = progress - 127;
                updateRightMotorDisplay();
                if (!isWaterOperationInProgress) sendMotorCommand();
            }
            public void onStartTrackingTouch(SeekBar seekBar) {}
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });
    }

    private void updateLeftMotorDisplay() {
        String direction = (leftMotorSpeed > 0) ? "正转" : (leftMotorSpeed < 0) ? "反转" : "停止";
        tvLeftMotorValue.setText(String.format(Locale.getDefault(), "%+d (%s)", leftMotorSpeed, direction));
    }

    private void updateRightMotorDisplay() {
        String direction = (rightMotorSpeed > 0) ? "正转" : (rightMotorSpeed < 0) ? "反转" : "停止";
        tvRightMotorValue.setText(String.format(Locale.getDefault(), "%+d (%s)", rightMotorSpeed, direction));
    }

    private void setupButtonListeners() {
        btnAutoSample.setOnClickListener(v -> {
            InputMethodManager imm = (InputMethodManager) getSystemService(INPUT_METHOD_SERVICE);
            if (imm != null) {
                imm.hideSoftInputFromWindow(etCValue.getWindowToken(), 0);
                imm.hideSoftInputFromWindow(etDValue.getWindowToken(), 0);
            }

            String cStr = etCValue.getText().toString().trim();
            String dStr = etDValue.getText().toString().trim();
            if (cStr.isEmpty() || dStr.isEmpty()) {
                Toast.makeText(this, "吸水时间和排水时间不能为空", Toast.LENGTH_SHORT).show();
                return;
            }

            try {
                int c = Integer.parseInt(cStr);
                int d = Integer.parseInt(dStr);
                if (c <= 0 || d <= 0) {
                    Toast.makeText(this, "时间值必须大于0", Toast.LENGTH_SHORT).show();
                    return;
                }

                if (!checkWaterOperationCooldown()) return;

                String cmd = "1 6 " + c + " " + d;
                sendControlCommand(cmd);
                Toast.makeText(this, "命令已发送: " + cmd, Toast.LENGTH_SHORT).show();

                isWaterOperationInProgress = true;
                lastWaterOperationTime = System.currentTimeMillis();
                lastWaterOperationDuration = (c + d) * 1000;
                new Handler().postDelayed(() -> {
                    isWaterOperationInProgress = false;
                    Toast.makeText(MainActivity.this, "水样采集/排放完成", Toast.LENGTH_SHORT).show();
                }, lastWaterOperationDuration);

            } catch (NumberFormatException e) {
                Toast.makeText(this, "请输入有效数字", Toast.LENGTH_SHORT).show();
            }
        });

        btnEmergencyLeft.setOnTouchListener((v, event) -> {
            emergencyLeftPressed = event.getAction() == MotionEvent.ACTION_DOWN;
            checkEmergencyStop();
            return false;
        });

        btnEmergencyRight.setOnTouchListener((v, event) -> {
            emergencyRightPressed = event.getAction() == MotionEvent.ACTION_DOWN;
            checkEmergencyStop();
            return false;
        });

        btnAutoCruise.setOnClickListener(v -> {
            Intent intent = new Intent(MainActivity.this, MapActivity.class);
            startActivityForResult(intent, 1);
        });
    }

    private boolean checkWaterOperationCooldown() {
        long now = System.currentTimeMillis();
        long elapsed = now - lastWaterOperationTime;
        if (elapsed < lastWaterOperationDuration) {
            long remain = (lastWaterOperationDuration - elapsed) / 1000;
            Toast.makeText(this, "请等待 " + remain + " 秒后再试", Toast.LENGTH_SHORT).show();
            return false;
        }
        if (isWaterOperationInProgress) {
            Toast.makeText(this, "采样正在进行中", Toast.LENGTH_SHORT).show();
            return false;
        }
        return true;
    }

    private void checkEmergencyStop() {
        if (emergencyLeftPressed && emergencyRightPressed) {
            leftMotorSeekBar.setProgress(127);
            rightMotorSeekBar.setProgress(127);
            sendControlCommand("0 2 0 0");
            Toast.makeText(this, "紧急停止激活！", Toast.LENGTH_SHORT).show();
        }
    }

    private void sendMotorCommand() {
        long now = System.currentTimeMillis();
        if (now - lastMotorCommandTime >= MOTOR_COMMAND_INTERVAL_MS) {
            lastMotorCommandTime = now;
            String cmd = "0 1 " + leftMotorSpeed + " " + rightMotorSpeed;
            sendControlCommand(cmd);
        }
    }

    private void sendControlCommand(String command) {
        try {
            if (mqtt_client == null || !mqtt_client.isConnected()) {
                Connect();
                Toast.makeText(this, "MQTT未连接，尝试重连", Toast.LENGTH_SHORT).show();
                return;
            }
            MqttMessage msg = new MqttMessage(command.getBytes());
            msg.setQos(1);
            mqtt_client.publish(mqtt_pub_topic, msg);
            Log.d("MQTT_DEBUG", "已发送: " + command);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void processReceivedMessage(String message) {
        runOnUiThread(() -> {
            try {
                JSONObject json = new JSONObject(message);
                StringBuilder alarm = new StringBuilder();
                if (json.has("pH")) {
                    double v = json.getDouble("pH");
                    tvPHValue.setText("pH: " + v);
                    if (v < 6.5 || v > 8.5) alarm.append("pH异常\n");
                }
                if (json.has("oxygen")) {
                    double v = json.getDouble("oxygen");
                    tvOxygen.setText("溶解氧: " + v + " mg/L");
                    if (v < 3) alarm.append("溶解氧过低\n");
                }
                if (json.has("turbidity")) {
                    double v = json.getDouble("turbidity");
                    tvTurbidity.setText("浊度: " + v + " NTU");
                    if (v > 20) alarm.append("浊度过高\n");
                }
                if (json.has("temperature")) {
                    double v = json.getDouble("temperature");
                    tvTemperature.setText("水温: " + v + " ℃");
                    if (v < 0 || v > 35) alarm.append("水温异常\n");
                }
                if (json.has("conductivity")) {
                    double v = json.getDouble("conductivity");
                    tvConductivity.setText("电导率: " + v + " μS/cm");
                    if (v > 2000) alarm.append("电导率过高\n");
                }
                tvLastUpdate.setText("最后更新: " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.getDefault()).format(new Date()));
                if (alarm.length() > 0) {
                    new androidx.appcompat.app.AlertDialog.Builder(this)
                            .setTitle("⚠ 系统报警")
                            .setMessage(alarm.toString())
                            .setCancelable(false)
                            .setPositiveButton("确认", null)
                            .show();
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        });
    }

    public void mqtt_init_Connect() {
        try {
            mqtt_client = new MqttClient(ip, Id, new MemoryPersistence());
            options = new MqttConnectOptions();
            options.setUserName(userName);
            options.setPassword(passWord.toCharArray());
            options.setConnectionTimeout(30);
            options.setKeepAliveInterval(30);
            options.setAutomaticReconnect(true);
            options.setCleanSession(false);
            Connect();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void Connect() {
        try {
            mqtt_client.connect(options);
            mqtt_client.subscribe(mqtt_sub_topic);
            mqtt_client.setCallback(new MqttCallback() {
                public void connectionLost(Throwable cause) {
                    Log.e("MQTT", "连接丢失", cause);
                }
                public void messageArrived(String topic, MqttMessage message) {
                    processReceivedMessage(new String(message.getPayload()));
                }
                public void deliveryComplete(IMqttDeliveryToken token) {}
            });
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override protected void onDestroy() {
        super.onDestroy();
        try {
            if (mqtt_client != null && mqtt_client.isConnected())
                mqtt_client.disconnect();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void checkAndRequestPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            List<String> permissions = new ArrayList<>();
            if (checkSelfPermission(Manifest.permission.INTERNET) != PackageManager.PERMISSION_GRANTED)
                permissions.add(Manifest.permission.INTERNET);
            if (checkSelfPermission(Manifest.permission.ACCESS_NETWORK_STATE) != PackageManager.PERMISSION_GRANTED)
                permissions.add(Manifest.permission.ACCESS_NETWORK_STATE);
            if (!permissions.isEmpty())
                requestPermissions(permissions.toArray(new String[0]), 100);
        }
    }

    @Override public void onRequestPermissionsResult(int requestCode, @NonNull String[] perms, @NonNull int[] results) {
        super.onRequestPermissionsResult(requestCode, perms, results);
    }

    @Override protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == 1 && resultCode == RESULT_OK) {
            double lat = data.getDoubleExtra("latitude", 0);
            double lng = data.getDoubleExtra("longitude", 0);
            try {
                JSONObject json = new JSONObject();
                json.put("targetLat", lat);
                json.put("targetLng", lng);
                sendTargetPosition(json.toString());
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    private void sendTargetPosition(String msg) {
        try {
            if (mqtt_client != null && mqtt_client.isConnected())
                mqtt_client.publish("boat/target", new MqttMessage(msg.getBytes()));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
