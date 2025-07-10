package com.example.control;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import com.amap.api.location.AMapLocation;
import com.amap.api.location.AMapLocationClient;
import com.amap.api.location.AMapLocationClientOption;
import com.amap.api.location.AMapLocationListener;
import com.amap.api.maps.AMap;
import com.amap.api.maps.CameraUpdateFactory;
import com.amap.api.maps.MapView;
import com.amap.api.maps.model.LatLng;
import com.amap.api.maps.model.Marker;
import com.amap.api.maps.model.MarkerOptions;
import com.amap.api.maps.model.MyLocationStyle;

public class MapActivity extends AppCompatActivity implements AMap.OnMapClickListener {

    private MapView mapView;
    private AMap aMap;
    private Marker selectedMarker;

    private static final int REQUEST_LOCATION_PERMISSION = 101;

    private AMapLocationClient mLocationClient;
    private AMapLocationClientOption mLocationOption;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // 设置高德隐私合规（必须最先调用）
        AMapLocationClient.updatePrivacyShow(this, true, true);
        AMapLocationClient.updatePrivacyAgree(this, true);

        setContentView(R.layout.activity_map);

        mapView = findViewById(R.id.map);
        mapView.onCreate(savedInstanceState);

        aMap = mapView.getMap();

        // 设置定位图层样式
        MyLocationStyle locationStyle = new MyLocationStyle();
        locationStyle.myLocationType(MyLocationStyle.LOCATION_TYPE_LOCATE); // 定位一次并移动视角
        aMap.setMyLocationStyle(locationStyle);
        aMap.getUiSettings().setMyLocationButtonEnabled(true);
        aMap.setOnMapClickListener(this);

        checkLocationPermission(); // 检查定位权限
    }

    private void checkLocationPermission() {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION)
                != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.ACCESS_FINE_LOCATION},
                    REQUEST_LOCATION_PERMISSION);
        } else {
            enableLocation();
            initLocation(); // 初始化并开始定位
        }
    }

    private void enableLocation() {
        if (aMap != null) {
            aMap.setMyLocationEnabled(true); // 启用定位图层
        }
    }

    private void initLocation() {
        try {
            if (mLocationClient == null) {
                mLocationClient = new AMapLocationClient(getApplicationContext());
            }
            if (mLocationOption == null) {
                mLocationOption = new AMapLocationClientOption();
                mLocationOption.setLocationMode(AMapLocationClientOption.AMapLocationMode.Hight_Accuracy);
                mLocationOption.setOnceLocation(true); // 只定位一次
            }
            mLocationClient.setLocationOption(mLocationOption);

            mLocationClient.setLocationListener(new AMapLocationListener() {
                @Override
                public void onLocationChanged(AMapLocation aMapLocation) {
                    if (aMapLocation != null && aMapLocation.getErrorCode() == 0) {
                        double lat = aMapLocation.getLatitude();
                        double lon = aMapLocation.getLongitude();
                        LatLng currentLatLng = new LatLng(lat, lon);

                        aMap.moveCamera(CameraUpdateFactory.newLatLngZoom(currentLatLng, 17));
                        aMap.addMarker(new MarkerOptions().position(currentLatLng).title("当前位置"));

                        Toast.makeText(MapActivity.this, "定位成功: " + lat + ", " + lon, Toast.LENGTH_SHORT).show();
                    } else {
                        Toast.makeText(MapActivity.this, "定位失败: " +
                                (aMapLocation != null ? aMapLocation.getErrorInfo() : "null"), Toast.LENGTH_LONG).show();
                    }
                }
            });

            mLocationClient.startLocation();
        } catch (Exception e) {
            e.printStackTrace();
            Toast.makeText(this, "定位初始化失败：" + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == REQUEST_LOCATION_PERMISSION) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                enableLocation();
                initLocation(); // 重新初始化定位
            } else {
                Toast.makeText(this, "定位权限被拒绝，无法使用定位功能", Toast.LENGTH_SHORT).show();
            }
        }
    }

    @Override
    public void onMapClick(LatLng latLng) {
        // 移除旧标记
        if (selectedMarker != null) {
            selectedMarker.remove();
        }

        // 添加新标记
        selectedMarker = aMap.addMarker(new MarkerOptions()
                .position(latLng)
                .title("巡航点")
                .snippet("纬度: " + latLng.latitude + ", 经度: " + latLng.longitude));

        // 返回选中的点
        Intent resultIntent = new Intent();
        resultIntent.putExtra("latitude", latLng.latitude);
        resultIntent.putExtra("longitude", latLng.longitude);
        setResult(RESULT_OK, resultIntent);

        // 可选：自动关闭页面
        // finish();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mapView.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        mapView.onPause();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mapView.onSaveInstanceState(outState);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mapView.onDestroy();
        if (mLocationClient != null) {
            mLocationClient.stopLocation();
            mLocationClient.onDestroy();
        }
    }
}
