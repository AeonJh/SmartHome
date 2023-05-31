//
// Created by aeonjh on 23-1-6.
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <string.h>
#include "../include/event_lcd.h"
#include "../include/lcd.h"
#include "../include/TempAndHumi.h"


void TempAndHumi()
{
    int environ_fd = open("/dev/ttySAC2",O_RDWR);
    if(environ_fd == -1)
    {
        perror("");
        return ;
    }

    //发送命令给传感器
    unsigned char cmd[3] = {0xA5,0x83,0x28};
    int w = write(environ_fd,cmd,3);
    if(w == -1)
    {
        perror("");
        close(environ_fd);
        return ;
    }

    //数据接收数组
    unsigned char data[24]={};
    //光强
    unsigned char L_Lux[4]={};
    //温度
    unsigned char T_Temperaturea[2]={};
    //气压
    unsigned char P_Pressure[2]={};
    //湿度
    unsigned char H_Hum[2]={};
    //海拔
    unsigned char H_Altitude[2]={};

    while(1)
    {

        //读取传感器数据
        int r = read(environ_fd, data, 24);
        if(r == -1)
        {
            perror("");
            close(environ_fd);
            return;
        }
        //读取光照强度
        if (r == 24 && data[2] == 0x15)
        {
            //光照：前高8位<<24 | 前低8位<<16 | 后高8位<<8 | 后低8位
            int Lux=(data[4]<<24)|(data[5]<<16)|(data[6]<<8)|data[7];
            //Lux = Lux/100;
            sprintf(L_Lux, "%d", Lux);
            LCD_PutString(202, 389, L_Lux, BLUE, 0xadd8b8);

            //温度：高8位<<8|低8位
            short int Temperature = data[13] << 8 | data[14];
            //Temperature = Temperature/100;
            sprintf(T_Temperaturea, "%d", Temperature);
            LCD_PutString(202, 641, T_Temperaturea, BLUE, 0xadd8b8);

            //气压：前高8位<<24 | 前低8位<<16 | 后高8位<<8 | 后低8位
            int Pressure = (data[15]<<24)|(data[16]<<16)|(data[17]<<8)|data[18];
            //Pressure = Pressure/100;
            sprintf(P_Pressure, "%d", Pressure);
            LCD_PutString(325, 140, P_Pressure, BLUE, 0xadd8b8);

            //湿度：高8位<<8|低8位
            short int Hum = data[19] << 8 | data[20];
            //Hum = Hum/100;
            sprintf(H_Hum, "%d", Hum);
            LCD_PutString(325, 394, H_Hum, BLUE, 0xadd8b8);

            //海拔：高8位<<8|低8位
            short int Altitude = data[21] << 8 | data[22];
            sprintf(H_Altitude, "%d", Altitude);
            LCD_PutString(325, 634, H_Altitude, BLUE, 0xadd8b8);

        }
        sleep(1);
    }
    close(environ_fd);
}