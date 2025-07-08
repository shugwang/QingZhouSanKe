#include "navigation.h"

void GPS_init(void)
{
    uart_init(GPS_UART_IDX, 9600);
}

//将GPS $GNGGA ddmm.mmmm（度分）转换成常用坐标dd.ddddd （WGS-84国际标准下）
double dm_to_dd(double dm) {
    // 获取度部分
    double degree = (int)(dm / 100);
    // 获取分部分
    double minute = dm - degree * 100;
    // 将度分转换为小数形式
    double decimal = degree + minute / 60;
    
    return decimal;
}

void GPS_get_data(unsigned char* uartReadBuff)
{
    int len = 0;
    
    len = uart_read_data( GPS_UART_IDX ,uartReadBuff, UART_BUFF_SIZE);
    if (len > 0) {
        printf("\r\n--------------------------------------------------------------\r\n");
        printf("The serial port information output by the GPS module is : ,len: %u \r\n",len);
        printf("%s \r\n",uartReadBuff);

        /***********************************解析GPS模块发送的 NMEA 0183协议的数据帧**************************************/

        /********************获取$GNGGA的经纬度信息（WGS-84国际标准下）***************************************/
        // 查找$GNGGA所在的行
        char *nmea_gngga = strstr(uartReadBuff, "$GNGGA");
        if (nmea_gngga == NULL) {
                printf("[$GNGGA,WGS-84]The information on the $GNGGA line not found\n");
        }else{
                printf("[$GNGGA,WGS-84]The information on the $GNGGA line is found\n");
        }

        // 使用strtok函数分割 $GNGGA所在的行
        char *token;
        token = strtok(nmea_gngga, ",");
        char *nmea_fields[15]; //$GNGGA所在的行有15个数
        int i = 0;
        while (token != NULL) {
                nmea_fields[i++] = token;
                token = strtok(NULL, ",");
                if (i >= 15) break;
        }
    
        // 获取第3,4,5,6个字段的值(即GPS模块接收的经纬度坐标)
        char ddmm_output[128];

        if (i >= 6) {  

                //将第3,4,5,6个字段的值合并在一起，即ddmm_output的值为GPS模块接收的经纬度坐标(ddmm.mmmmm格式),例如3209.22695 N,11841.95567 E
                //当GPS没有连接信号时，这个数值打印出来是错误的。
                //nmea_fields[2]表示纬度
                //nmea_fields[3]来判断是北纬还是南纬，
                //nmea_fields[4]表示经度，
                //nmea_fields[5]来判断是东经还是西经
                sprintf(ddmm_output, "%s %s,%s %s", nmea_fields[2], nmea_fields[3], nmea_fields[4], nmea_fields[5]);
                printf("[$GNGGA,WGS-84]Latitude and longitude data (ddmm.mmmmm) is : %s \n", ddmm_output);//ddmm_output的值为GPS模块接收的经纬度坐标(ddmm.mmmmm格式)

                /******将GPS $GNGGA ddmm.mmmm（度分）转换成常用坐标dd.ddddd******/
                //将字符数组nmea_fields[2]转换为double类型
                double num0= 0.0;
                num0 = atof(nmea_fields[2]);
                //将字符数组nmea_fields[2]由ddmm.mmmm（度分）转换成常用坐标dd.ddddd （WGS-84国际标准下）
                double dd0 = dm_to_dd(num0);

                //将字符数组nmea_fields[4]转换为double类型
                double num1 = 0.0;
                num1 = atof(nmea_fields[4]);
                //将字符数组nmea_fields[4]由ddmm.mmmm（度分）转换成常用坐标dd.ddddd （WGS-84国际标准下）
                double dd1 = dm_to_dd(num1);
                printf("[$GNGGA,WGS-84]Latitude and longitude data (dd.ddddd) is : %0.5lf\n", dd0);//%0.5lf是格式化输出实数（double类型）时的占位符，其中0表示数值不足指定位数时用0来填充，5表示小数点后保留5位有效数字，l表示该数值为double类型，f表示浮点数。因此，%0.5lf用于输出一个占位符宽度为5（浮点数的整数部分位数+小数点后5位）、小数点后保留5位的实数值。
                printf("[$GNGGA,WGS-84]Latitude and longitude data (dd.ddddd) is : %0.5lf\n", dd1);
                /**************************************************************/

        } else { printf("[$GNGGA]Latitude and longitude data no found\n"); }
        /********************************************************************************/

        /****************************************************************************************************************/
    }

}