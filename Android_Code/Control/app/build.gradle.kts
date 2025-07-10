plugins {
    alias(libs.plugins.android.application)
}

android {
    namespace = "com.example.control"
    compileSdk = 35

    defaultConfig {
        applicationId = "com.example.control"
        minSdk = 29
        targetSdk = 35
        versionCode = 1
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }
    packaging {
        resources {
            excludes.add("assets/location_map_gps_locked.png")
            // 或者同时排除多个：
            // excludes.addAll(listOf("assets/location_map_gps_locked.png", "another/file"))
        }
    }
    packagingOptions {
        pickFirst ("assets/location_map_gps_3d.png")
    }
}

dependencies {
    // 基础库
    implementation(libs.appcompat)
    implementation(libs.material)
    implementation(libs.activity)
    implementation(libs.constraintlayout)

    // MQTT 通信
    implementation("org.eclipse.paho:org.eclipse.paho.client.mqttv3:1.2.5")
    implementation("org.eclipse.paho:org.eclipse.paho.android.service:1.1.1")

    // 高德地图 SDK（版本统一且稳定）
    implementation("com.amap.api:3dmap:5.0.0")
    implementation("com.amap.api:location:6.4.0")
    implementation("com.amap.api:search:5.0.0")
    implementation ("com.amap.api:map2d:6.0.0")
    testImplementation(libs.junit)
    androidTestImplementation(libs.ext.junit)
    androidTestImplementation(libs.espresso.core)
}
