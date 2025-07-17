# QingZhouSanKe

## 1. ship_project
无人水质监测船的嵌入式软件代码

运行平台：海思Hi-3861

编译平台：DevEco Device Tool

编译套件：DevTools_Hi3861V100_v1.0

使用步骤：

1. 该工程文件放置目录：
D:\Users\hi3861_hdu_iot_application\src\vendor\pzkj\pz_hi3861\

2. 修改ninja的编译文件为：
```
lite_component("app") {
    features = [
        # "//vendor/hqyj/fs_hi3861/demo:demo",
		# "//vendor/pzkj/pz_hi3861/demo:demo",
        "//vendor/pzkj/pz_hi3861/ship_project:demo",
    ]
}
```
3. rebuild、upload即可

## 2. andriod_code
上位机软件源码

运行平台：andriod >= 6.0
