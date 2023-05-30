//
// Created by aeonjh on 23-1-5.
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <string.h>
#include "../include/smoke.h"
#include "../include/event_lcd.h"
#include "../include/lcd.h"

//测试烟雾传感器
void smoke_check()
{
    int smoke_fd = open("/dev/ttySAC1",O_RDWR);
    if(smoke_fd == -1)
    {
        perror("");
        return ;
    }

    //发送命令给传感器
    unsigned char cmd[9] = {0xFF, 0x01, 0x86 ,0x00 ,0x00, 0x00 ,0x00 ,0x00, 0x79};
    write(smoke_fd,cmd,9);

    //数据接收数组
    unsigned char data[9];

    unsigned char s_smoke[4]={};

    while(1)
    {
        //读取烟雾浓度数据
        int r = read(smoke_fd, data, 9);
        if (r == 9)
        {
            //高位<<8 | 低位
            short int smoke = data[2] << 8 | data[3];
            sprintf(s_smoke, "%d", smoke);
            LCD_PutString(202, 141, s_smoke, BLUE, 0xadd8b8);
        }
        sleep(1);
    }

    close(smoke_fd);

    return;
}

