#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include "../include/lcd.h"
#include "../include/event_lcd.h"
#include "../include/smoke.h"
#include "../include/TempAndHumi.h"


int main()
{
    int flag = 0;
    int r = lcd_init();
    if(r==0)
    {
        return 0;
    }

    //清屏
    lcd_clean();

    //启动画面
    lcd_display_bmp(0, 0, "images/start.bmp");
    sleep(2);

    //显示一个矩形(实心)
    //lcd_draw_solid_rectangle(120,40,100,50,0xff0000);

    //显示一个矩形(空心)
    //lcd_draw_hollow_rectangle(390,40,100,50,0x00ff00);

    //显示一个圆形(实心)
    //lcd_draw_solid_circle(245,90,20,RED);
    //lcd_draw_hollow_circle(315,90,20,GREEN);

    //显示一个圆形(空心)
    //lcd_draw_hollow_circle(350,540,80,0xffff00);

    origin_view();

    while(1)
    {
        flag = event_main();

        //功能选择
        switch (flag)
        {
            case 1:
                flag = 0;
                loading();
                event_environ();
                origin_view();
                break;

            case 2:
                flag = 0;
                loading();
                event_music();
                origin_view();
                break;

            case 3:
                flag = 0;
                loading();
                event_picture();
                origin_view();
                break;

            case 4:
                flag = 0;
                loading();
                event_game();
                origin_view();
                break;

            case 5:
                flag = 0;
                lcd_uninit();
                return 0;

            default:
                break;

        }
    }

    return 0;
}


