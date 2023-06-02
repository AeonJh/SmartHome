//
// Created by aeonjh on 23-1-8.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <string.h>
#include "../include/event_lcd.h"
#include "../include/lcd.h"
#include "../include/game2048.h"


// 游戏主界面是一个`4*4`的`16`宫格，使用二维数组进行表示，用`0`表示空格
int matrix_2048[4][4] = {0};

// `16`宫格中空格的个数
int empty = 16;


void game2048()
{
    //初始化游戏背景
    lcd_clean();
    lcd_display_bmp(0,0,"./source/images/2048b.bmp");
    lcd_display_bmp(0,0,"./source/images/return.bmp");
    lcd_display_bmp(15,110,"./source/images/gameboard.bmp");
    lcd_display_bmp(120,640,"./source/images/up.bmp");
    lcd_display_bmp(280,640,"./source/images/down.bmp");
    lcd_display_bmp(200,560,"./source/images/left0.bmp");
    lcd_display_bmp(200,720,"./source/images/right0.bmp");

    game_init();

    int touch_fd = open("/dev/ubuntu_event", O_RDWR);

    //驱动文件打开失败
    if (touch_fd < 0) {
        perror("open fail");
        return;
    }

    //触摸坐标
    int x_read = -1;
    int y_read = -1;
    //程序标识符
    int flag = 0;
    int over = 0;

    //定义输出事件结构体
    struct input_event ev;
    //输入事件ev初值
    //触摸一点后输入事件ev始终保持不变，因此采用手动赋初值
    char origin_ev[24] = {};
    //printf("%ld\n",sizeof(struct input_event));


    //一直读取触摸板信息
    while (1) {
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

        //1.上
        if(x_read>=640 && x_read<=720 && y_read>=120 && y_read<=200)
        {
            flag = 8;
            x_read = -1;
            y_read = -1;
            write(touch_fd,origin_ev,24);
            //调用game_move函数
            over = game_move(flag);
            if(over)
                continue;
            else
            {
                //关闭文件
                close(touch_fd);
                return;
            }
        }
        //2.下
        if(x_read>=640 && x_read<=720 && y_read>=280 && y_read<=360)
        {
            flag = 2;
            x_read = -1;
            y_read = -1;
            write(touch_fd,origin_ev,24);
            //调用game_move函数
            over = game_move(flag);
            if(over)
                continue;
            else
            {
                //关闭文件
                close(touch_fd);
                return;
            }
        }
        //3.左
        if(x_read>=560 && x_read<=640 && y_read>=200 && y_read<=280)
        {
            flag = 4;
            x_read = -1;
            y_read = -1;
            write(touch_fd,origin_ev,24);
            //调用game_move函数
            over = game_move(flag);
            if(over)
                continue;
            else
            {
                //关闭文件
                close(touch_fd);
                return;
            }
        }
        //4.右
        if(x_read>=720 && x_read<=800 && y_read>=200 && y_read<=280)
        {
            flag = 6;
            x_read = -1;
            y_read = -1;
            write(touch_fd,origin_ev,24);
            //调用game_move函数
            over = game_move(flag);
            if(over)
                continue;
            else
            {
                //关闭文件
                close(touch_fd);
                return;
            }
        }

        //结束 or 退出
        if(x_read>=0 && x_read<=120 && y_read>=0 && y_read<=80)
        {
            lcd_display_bmp(0,0,"./source/images/2048bmp/game_over.bmp");
            break;
        }

    }

    //关闭文件
    close(touch_fd);

    return;
}

//游戏初始化
void game_init()
{
    for(int i=0;i<4;i++)
        for (int j = 0; j < 4; ++j)
            matrix_2048[i][j] = 0;
    empty = 16;

    game_generate();
    game_generate();
    game_draw(matrix_2048);

    return;
}

//方块移动及合成
int game_move(int flag)
{
    int label = game_over();
    if(label)
    {
        switch (flag)
        {
            case 8:
                for (int j = 0; j < 4; ++j)//列
                {
                    for (int i = 0; i < 3; ++i)//第一行开始寻找，最后一行无需判断
                    {
                        for (int k = i+1; k < 4; ++k) //比较下一行
                        {
                            if(matrix_2048[k][j]!=0)
                            {
                                if(matrix_2048[i][j] == 0)//位移
                                {
                                    matrix_2048[i][j] = matrix_2048[k][j];
                                    matrix_2048[k][j] = 0;
                                }
                                else
                                {
                                    if (matrix_2048[k][j] == matrix_2048[i][j])//合并
                                    {
                                        matrix_2048[i][j] *= 2;
                                        matrix_2048[k][j] = 0;
                                        empty++;
                                        break;
                                    }
                                    else
                                        break;
                                }
                            }
                        }
                    }
                }
                if(empty>1)
                {
                    game_generate();
                    game_generate();
                }
                else if(empty)
                    game_generate();
                break;
            case 2:
                for (int j = 0; j < 4; ++j)//列
                {
                    for (int i = 3; i > 0; --i)//最后一行开始寻找，第一行无需判断
                    {
                        for (int k = i-1; k >= 0; --k) //比较上一行
                        {
                            if (matrix_2048[k][j] != 0) {
                                if (matrix_2048[i][j] == 0)//位移
                                {
                                    matrix_2048[i][j] = matrix_2048[k][j];
                                    matrix_2048[k][j] = 0;
                                } else {
                                    if (matrix_2048[k][j] == matrix_2048[i][j])//合并
                                    {
                                        matrix_2048[i][j] *= 2;
                                        matrix_2048[k][j] = 0;
                                        empty++;
                                        break;
                                    } else {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                if(empty>1)
                {
                    game_generate();
                    game_generate();
                }
                else if(empty)
                    game_generate();
                break;
            case 4:
                for (int i = 0; i < 4; ++i)//行
                {
                    for (int j = 0; j < 3; ++j)//最左一列开始寻找，最右一列无需判断
                    {
                        for (int k = j+1; k < 4; ++k) //比较右一列
                        {
                            if(matrix_2048[i][k]!=0)
                            {
                                if(matrix_2048[i][j] == 0)//位移
                                {
                                    matrix_2048[i][j] = matrix_2048[i][k];
                                    matrix_2048[i][k] = 0;
                                }
                                else {
                                    if (matrix_2048[i][k] == matrix_2048[i][j])//合并
                                    {
                                        matrix_2048[i][j] *= 2;
                                        matrix_2048[i][k] = 0;
                                        empty++;
                                        break;
                                    } else {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                if(empty>1)
                {
                    game_generate();
                    game_generate();
                }
                else if(empty)
                    game_generate();
                break;
            case 6:
                for (int i = 0; i < 4; ++i)//行
                {
                    for (int j = 3; j > 0; --j)//最右一列开始寻找，最左一列无需判断
                    {
                        for (int k = j-1; k >= 0; --k) //比较左一行
                        {
                            if(matrix_2048[i][k]!=0)
                            {
                                if(matrix_2048[i][j] == 0)//位移
                                {
                                    matrix_2048[i][j] = matrix_2048[i][k];
                                    matrix_2048[i][k] = 0;
                                }
                                else
                                {
                                    if(matrix_2048[i][k] ==matrix_2048[i][j])//合并
                                    {
                                        matrix_2048[i][j] *=2;
                                        matrix_2048[i][k] = 0;
                                        empty++;
                                        break;
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                if(empty>1)
                {
                    game_generate();
                    game_generate();
                }
                else if(empty)
                    game_generate();
                break;
        }
        game_draw();
        return 1;
    }
    else
    {
        return 0;
    }
}

//判断游戏是否结束
//通过判断相邻元素是否相等(仅当空格为0时进行分析)
int game_over()
{
    //判定是否胜利
    for(int i=0;i<4;i++)
    {
        for (int j = 0; j < 4; ++j)
        {
            if(matrix_2048[i][j] == 1024)
            {
                lcd_clean();
                lcd_display_bmp(0,0,"./source/images/2048bmp/game_win.bmp");
                //system("madplay -q /mnt/hgfs/VMware_Share/my_project/virtual_lcd./source/music/yeaoh.mp3 &");
                return 0;
            }
        }
    }
    if(empty>0)
        return 1;
    else
    {
        //逐行逐列比较
        for (int i = 0; i < 4; ++i)
        {
            int t1 = 0,t2 = 1;
            while(t2<=3)
            {
                if(matrix_2048[i][t1]==matrix_2048[i][t2] || matrix_2048[t1][i]==matrix_2048[t2][i]) //横 || 纵
                    return 1;
                else
                {
                    t1++;
                    t2++;
                }
            }
        }
        lcd_clean();
        lcd_display_bmp(0,0,"./source/images/2048bmp/game_over.bmp");
        //system("madplay -q /mnt/hgfs/VMware_Share/my_project/virtual_lcd./source/music/boooh.mp3 &");
        return 0;
    }
}

//随机生成新方块
void game_generate()
{
    srand(time(NULL));
    int n =( rand() % 5 )? 2 : 4; //80%概率生成2，20%概率生成4

    int a[16];//16个元素依次保存第n个空白位置的行坐标
    int b[16];//16个元素依次保存第n个空白位置的列坐标
    int count = 0;

    for(int i=0;i<4;i++)
    {
        for(int j=0;j<4;j++)
        {
            if(matrix_2048[i][j] == 0)
            {
                a[count] = i;
                b[count] = j;
                count++;
            }
        }
    }

    int m = rand() % empty;
    matrix_2048[a[m]][b[m]] = n;

    empty--;

}

//绘制图形
void game_draw()
{
    for (int i = 0; i < 4; ++i){
        for (int j = 0; j < 4; ++j){
            switch(matrix_2048[i][j]){
                case 0:
                    lcd_display_bmp(15+i*100+(i+1)*10,110+j*100+(j+1)*10,"./source/images/2048bmp/0.bmp");
                    break;
                case 2:
                    lcd_display_bmp(15+i*100+(i+1)*10,110+j*100+(j+1)*10,"./source/images/2048bmp/2.bmp");
                    break;
                case 4:
                    lcd_display_bmp(15+i*100+(i+1)*10,110+j*100+(j+1)*10,"./source/images/2048bmp/4.bmp");
                    break;
                case 8:
                    lcd_display_bmp(15+i*100+(i+1)*10,110+j*100+(j+1)*10,"./source/images/2048bmp/8.bmp");
                    break;
                case 16:
                    lcd_display_bmp(15+i*100+(i+1)*10,110+j*100+(j+1)*10,"./source/images/2048bmp/16.bmp");
                    break;
                case 32:
                    lcd_display_bmp(15+i*100+(i+1)*10,110+j*100+(j+1)*10,"./source/images/2048bmp/32.bmp");
                    break;
                case 64:
                    lcd_display_bmp(15+i*100+(i+1)*10,110+j*100+(j+1)*10,"./source/images/2048bmp/64.bmp");
                    break;
                case 128:
                    lcd_display_bmp(15+i*100+(i+1)*10,110+j*100+(j+1)*10,"./source/images/2048bmp/128.bmp");
                    break;
                case 256:
                    lcd_display_bmp(15+i*100+(i+1)*10,110+j*100+(j+1)*10,"./source/images/2048bmp/256.bmp");
                    break;
                case 512:
                    lcd_display_bmp(15+i*100+(i+1)*10,110+j*100+(j+1)*10,"./source/images/2048bmp/512.bmp");
                    break;
                case 1024:
                    lcd_display_bmp(15+i*100+(i+1)*10,110+j*100+(j+1)*10,"./source/images/2048bmp/1024.bmp");
                    break;
                case 2048:
                    lcd_display_bmp(15+i*100+(i+1)*10,110+j*100+(j+1)*10,"./source/images/2048bmp/2048.bmp");
                    break;
            }
        }
    }
}