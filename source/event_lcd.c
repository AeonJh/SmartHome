//
// Created by aeonjh on 23-1-5.
//

//获取坐标
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "../include/game2048.h"
#include "../include/event_lcd.h"
#include "../include/smoke.h"
#include "../include/lcd.h"
#include "../include/TempAndHumi.h"

#define MOVE_UP 8
#define MOVE_DOWN 2
#define MOVE_LEFT 4
#define MOVE_RIGHT 6


//0.主屏幕触摸
int event_main()
{
    int touch_fd = open("/dev/ubuntu_event",O_RDWR);

    //驱动文件打开失败
    if(touch_fd<0)
    {
        perror("open fail");
        return -1;
    }

    //触摸坐标
    int x_read = -1;
    int y_read = -1;
    //程序标识符
    int flag = 0;
    //int bug = 0;

    //定义输出事件结构体
    struct input_event ev;
    //输入事件ev初值
    //触摸一点后输入事件ev始终保持不变，因此采用手动赋初值
    char origin_ev[24] = {};
    //printf("%ld\n",sizeof(struct input_event));

    //一直读取触摸板信息
    while(1)
    {
        //读取屏幕
        read(touch_fd, &ev, sizeof(struct input_event));//第三个参数ev

        //读坐标X轴
        if(ev.type == EV_ABS && ev.code ==ABS_X)
        {
            x_read = ev.value ;
        }
        //读坐标Y轴
        if(ev.type == EV_ABS && ev.code ==ABS_Y)
        {
            y_read = ev.value ;
        }

        //功能选择
        //1.环境检测
        if(x_read>=80 && x_read<=205 && y_read>=160 && y_read<=285)
        {

            flag = 1;
            //执行完后写入初值等待下次触摸
            x_read = -1;
            y_read = -1;
            write(touch_fd,origin_ev,24);
            break;
        }
        //2.音乐播放器
        if(x_read>=260 && x_read<=378 && y_read>=160 && y_read<=285)
        {

            flag = 2;
            //执行完后写入初值等待下次触摸
            x_read = -1;
            y_read = -1;
            write(touch_fd,origin_ev,24);
            break;
        }
        //3.相册
        if(x_read>=430 && x_read<=547 && y_read>=160 && y_read<=285)
        {

            flag = 3;
            //执行完后写入初值等待下次触摸
            x_read = -1;
            y_read = -1;
            write(touch_fd,origin_ev,24);
            break;
        }
        //4.娱乐
        if(x_read>=600 && x_read<=717 && y_read>=160 && y_read<=285)
        {

            flag = 4;
            //执行完后写入初值等待下次触摸
            x_read = -1;
            y_read = -1;
            write(touch_fd,origin_ev,24);
            break;
        }
        //结束程序
        if(x_read>=720 && x_read<=800 && y_read>=0 && y_read<=80)
        {
            system("killall -9 madplay");
            //关机画面
            lcd_display_bmp(0,0,"/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/goodbye.bmp");
            sleep(3);
            lcd_clean();
            flag = 5;
            x_read = -1;
            y_read = -1;
            write(touch_fd,origin_ev,24);
            break;
        }

    }
    //关闭文件
    close(touch_fd);

    return flag;
}

//1.环境监测-触摸
int event_environ()
{
    lcd_clean();
    lcd_display_bmp(0,0,"/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/envir.bmp");
    lcd_display_bmp(0,0,"/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/return.bmp");


    int touch_fd = open("/dev/ubuntu_event",O_RDWR);

    //驱动文件打开失败
    if(touch_fd<0)
    {
        perror("open fail");
        return -1;
    }

    //触摸坐标
    int x_read = -1;
    int y_read = -1;
    //程序标识符
    int flag = 0;
    //int bug = 0;

    //定义输出事件结构体
    struct input_event ev;
    //输入事件ev初值
    //触摸一点后输入事件ev始终保持不变，因此采用手动赋初值
    char origin_ev[24] = {};
    //printf("%ld\n",sizeof(struct input_event));

    //创建线程
    pthread_t id1;
    int res_id1 = pthread_create(&id1, NULL, (void *(*)(void *))&TempAndHumi, NULL);
    if (res_id1 != 0)
    {
        printf("线程创建失败\n");
        perror("");
        return -1;
    }
    pthread_t id2;
    int res_id2 = pthread_create(&id2, NULL, (void *(*)(void *))&smoke_check, NULL);
    if (res_id2 != 0)
    {
        printf("线程创建失败\n");
        perror("");
        return -1;
    }

    //一直读取触摸板信息
    while(1)
    {
        //读取屏幕
        read(touch_fd, &ev, sizeof(struct input_event));//第三个参数ev

        //读坐标X轴
        if(ev.type == EV_ABS && ev.code ==ABS_X)
        {
            x_read = ev.value ;
        }
        //读坐标Y轴
        if(ev.type == EV_ABS && ev.code ==ABS_Y)
        {
            y_read = ev.value ;
        }

        //返回主程序
        if(x_read>=0 && x_read<=120 && y_read>=0 && y_read<=80)
        {
            pthread_cancel(id1);
            pthread_cancel(id2);
            pthread_join(id1,NULL);
            pthread_join(id2,NULL);
            break;
        }
    }
    //关闭文件
    close(touch_fd);
    return flag;
}

/*
 ***** 注意： &为后台运行，如果不加&则为前台播放，将无法对其进行暂停、恢复播放操作 *****
 *
system("madplay music/*.mp3 &");	//利用system函数调用madplay播放器播放music目录下所有的.mp3音乐

system("madplay music/*.mp3 -r &");	//列表歌曲循环播放

system("killall -STOP madplay &");	//利用system函数调用killall命令暂停播放音乐

system("killall -CONT madplay &");	//利用system函数调用killall命令恢复播放音乐

system("killall -9 madplay");		//利用system函数调用killall命令将madplay终止关闭
 */
//2.音乐播放器-触摸
int event_music()
{
    lcd_clean();
    lcd_display_bmp(0,0,"/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/player.bmp");
    lcd_display_bmp(0,0,"/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/return.bmp");
    lcd_display_bmp(0,264,"/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/music_player.bmp");

    int touch_fd = open("/dev/ubuntu_event", O_RDWR);

    //驱动文件打开失败
    if (touch_fd < 0) {
        perror("open fail");
        return -1;
    }

    //触摸坐标
    int x_read = -1;
    int y_read = -1;
    //程序标识符
    int flag = 0;
    //int bug = 0;

    //歌曲存储
    char mp3_name[5][80]={"/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/music/1.mp3",
                         "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/music/2.mp3",
                         "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/music/3.mp3",
                         "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/music/4.mp3",
                         "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/music/5.mp3"};

    char mp3[5][30]={"夏天的风.mp3"," 虞兮叹.mp3"," 红昭愿.mp3","天   下.mp3","爱丫爱丫.mp3"};

    //存放拼接后的字符串
    char buf[100];

    //进入待播放状态
    system("madplay -q /mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/music/*.mp3 -r &");
    system("killall -STOP madplay &");
    LCD_PutString(120,350,mp3[0],WHITE,BLACK);

    //定义输出事件结构体
    struct input_event ev;
    //输入事件ev初值
    //触摸一点后输入事件ev始终保持不变，因此采用手动赋初值
    char origin_ev[24] = {};
    //printf("%ld\n",sizeof(struct input_event));


    //一直读取触摸板信息
    while (1)
    {
        //读取屏幕
        read(touch_fd, &ev, sizeof(struct input_event));//第三个参数ev

        //读坐标X轴
        if (ev.type == EV_ABS && ev.code == ABS_X) {
            x_read = ev.value;
        }
        //读坐标Y轴
        if (ev.type == EV_ABS && ev.code == ABS_Y) {
            y_read = ev.value;
        }

        //功能选择
        //1.暂停
        if(x_read>=200 && x_read<=299 && y_read>=245 && y_read<=340)
        {
            system("killall -STOP madplay &");
            x_read = -1;
            y_read = -1;
            write(touch_fd,origin_ev,24);
            //printf("暂停\n");
        }

        //2.上一曲
        if(x_read>=300 && x_read<=399 && y_read>=245 && y_read<=340)
        {
            x_read = -1;
            y_read = -1;
            write(touch_fd,origin_ev,24);
            //printf("上一曲\n");

            system("killall -9 madplay");
            flag--;
            if(flag == -1) flag = 4;
            sprintf(buf,"madplay -q %s &",mp3_name[flag]);
            lcd_display_bmp(0,264,"/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/music_player.bmp");
            LCD_PutString(120,350,mp3[flag],WHITE,BLACK);
            system(buf);
        }

        //3.播放
        if(x_read>=400 && x_read<=499 && y_read>=245 && y_read<=340)
        {
            system("killall -CONT madplay &");
            x_read = -1;
            y_read = -1;
            write(touch_fd,origin_ev,24);
            //printf("播放n");
        }

        //4.下一曲
        if(x_read>=500 && x_read<=600 && y_read>=245 && y_read<=340)
        {
            x_read = -1;
            y_read = -1;
            write(touch_fd,origin_ev,24);

            system("killall -9 madplay");
            flag++;
            if(flag == 5) flag = 0;
            sprintf(buf,"madplay -q %s &",mp3_name[flag]);
            lcd_display_bmp(0,264,"/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/music_player.bmp");
            LCD_PutString(120,350,mp3[flag],WHITE,BLACK);
            system(buf);
            //printf("下一曲\n");
        }

        //返回主程序
        if(x_read>=0 && x_read<=120 && y_read>=0 && y_read<=80)
        {
            break;
        }

    }
    //关闭文件
    close(touch_fd);

    return flag;
}


//3.相册-触摸
int event_picture()
{
    lcd_clean();
    lcd_display_bmp(0,0,"/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/pictures.bmp");
    lcd_display_bmp(25,125,"/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/Photo_Frame.bmp");
    //(x:20-107,y:200-280)
    lcd_display_bmp(200,20,"/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/left.bmp");
    //(x:693-780,y:200-280)
    lcd_display_bmp(200,693,"/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/right.bmp");
    lcd_display_bmp(0,0,"/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/return.bmp");

    lcd_display_bmp(105,205,"/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/picture0.bmp");

    int touch_fd = open("/dev/ubuntu_event", O_RDWR);

    //驱动文件打开失败
    if (touch_fd < 0) {
        perror("open fail");
        return -1;
    }

    //触摸坐标
    int x_read = -1;
    int y_read = -1;
    //程序标识符
    int flag = 0;
    //int bug = 0;

    //定义输出事件结构体
    struct input_event ev;
    //输入事件ev初值
    //触摸一点后输入事件ev始终保持不变，因此采用手动赋初值
    char origin_ev[24] = {};
    //printf("%ld\n",sizeof(struct input_event));

    //图片数组
    char picture[10][100]={"/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/picture0.bmp",
                           "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/picture1.bmp",
                           "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/picture2.bmp",
                           "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/picture3.bmp",
                           "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/picture4.bmp",
                           "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/picture5.bmp",
                           "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/picture6.bmp",
                           "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/picture7.bmp",
                           "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/picture8.bmp",
                           "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/picture9.bmp"};

    //一直读取触摸板信息
    while (1)
    {
        //读取屏幕
        read(touch_fd, &ev, sizeof(struct input_event));//第三个参数ev

        //读坐标X轴
        if (ev.type == EV_ABS && ev.code == ABS_X) {
            x_read = ev.value;
        }
        //读坐标Y轴
        if (ev.type == EV_ABS && ev.code == ABS_Y) {
            y_read = ev.value;
        }


        //功能选择
        //1.上一张图片
        if(x_read>=20 && x_read<=107 && y_read>=200 && y_read<=280)
        {
            flag--;
            if(flag == -1)  flag=9;
            x_read = -1;
            y_read = -1;
            write(touch_fd,origin_ev,24);
            lcd_display_bmp(105,205,picture[flag]);
        }
        //2.下一张图片
        if(x_read>=693 && x_read<=780 && y_read>=200 && y_read<=280)
        {
            flag++;
            if(flag == 10)  flag=0;
            x_read = -1;
            y_read = -1;
            write(touch_fd,origin_ev,24);
            lcd_display_bmp(105,205,picture[flag]);
        }


        //返回主程序
        if(x_read>=0 && x_read<=120 && y_read>=0 && y_read<=80)
        {
            break;
        }

    }
    //关闭文件
    close(touch_fd);

    return flag;
}

//4.娱乐-触摸
int event_game()
{
game_label:

    lcd_clean();
    lcd_display_bmp(0,0,"/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/2048.bmp");
    lcd_display_bmp(100,341,"/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/game.bmp");
    lcd_display_bmp(0,0,"/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/return.bmp");

    int touch_fd = open("/dev/ubuntu_event", O_RDWR);

    //驱动文件打开失败
    if (touch_fd < 0) {
        perror("open fail");
        return -1;
    }

    //触摸坐标
    int x_read = -1;
    int y_read = -1;
    //程序标识符
    int flag = 0;
    //int bug = 0;

    //定义输出事件结构体
    struct input_event ev;
    //输入事件ev初值
    //触摸一点后输入事件ev始终保持不变，因此采用手动赋初值
    char origin_ev[24] = {};
    //printf("%ld\n",sizeof(struct input_event));

    //一直读取触摸板信息
    while (1)
    {
        //读取屏幕
        read(touch_fd, &ev, sizeof(struct input_event));//第三个参数ev

        //读坐标X轴
        if (ev.type == EV_ABS && ev.code == ABS_X) {
            x_read = ev.value;
        }
        //读坐标Y轴
        if (ev.type == EV_ABS && ev.code == ABS_Y) {
            y_read = ev.value;
        }

        //功能选择
        //0.开始游戏
        if(flag==0 && x_read>=300 && x_read<=500 && y_read>=300 && y_read<=360)
        {
            flag=1;
            x_read = -1;
            y_read = -1;
            write(touch_fd,origin_ev,24);
            game2048();
            sleep(2);
            goto game_label;
        }

        //返回主程序
        if(x_read>=0 && x_read<=120 && y_read>=0 && y_read<=80)
        {
            x_read = -1;
            y_read = -1;
            write(touch_fd,origin_ev,24);
            break;
        }
    }
    //关闭文件
    close(touch_fd);

    return flag;
}