#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "../include/UTF816x16.h"
#include "../include/char8x16.h"
#include "../include/lcd.h"


int lcd_fd;
int *p;
/*
	功能：初始化显示屏
	在显示内容之前应该调用该函数一次（只能调用一次）
	返回值：
		初始化成功返回1
		初始化失败返回0
		
*/
int lcd_init()
{
    //打开屏幕驱动文件
    lcd_fd = open("/dev/ubuntu_lcd",O_RDWR);
    if(lcd_fd == -1)
    {
        perror("");
        printf("open lcd fail\n");

        return 0;
    }

    //映射到内存
    p = mmap(NULL,800*480*4,PROT_READ | PROT_WRITE,MAP_SHARED,lcd_fd,0);
    if(p == NULL)
    {
        perror("");
        printf("mmap fail");
        close(lcd_fd);
        return 0;
    }
    return 1;
}

/*
	功能：关闭屏幕及接触映射，和 lcd_init函数成对出现
			显示完所有内容之后调用函数
*/
void lcd_uninit()
{
    //解除映射
    munmap(p,800*480*4);
    close(lcd_fd);
}

/*
	功能：在屏幕上显示一个像素点
	参数：
		x :横坐标(列)
		y :纵坐标(行)
		color:颜色值
*/

void lcd_display_point(int x,int y,int color)
{
    if(y>=0 && y<480 && x>=0 && x<800)
        *(p+800*y+x) = color;
    else	printf("请输入合理的坐标！");
}


//画空心矩形
void lcd_draw_hollow_rectangle(int origin_y,int origin_x,int length,int width,int color)
{
    //画长
    for(int i=origin_x;i<(origin_x+length);i++)
    {
        lcd_display_point(i,origin_y,color);
        lcd_display_point(i,origin_y+width,color);
    }
    //画宽
    for(int i=origin_y;i<(origin_y+width);i++)
    {
        lcd_display_point(origin_x,i,color);
        lcd_display_point(origin_x+length,i,color);
    }
}

//画实心矩形
void lcd_draw_solid_rectangle(int origin_y,int origin_x,int length,int width,int color)
{
    for(int i=origin_x;i<(origin_x+length);i++)
        for(int j=origin_y;j<(origin_y+width);j++)
            lcd_display_point(i,j,color);
}


//画实心圆
void lcd_draw_solid_circle(int center_y,int center_x,int r,int color)
{
    if(center_y>=0 && center_y<480 && center_x>=0 && center_x<800)
    {
        for (int y = 0; y < 480; ++y)
        {
            for (int x = 0; x < 800; ++x)
            {
                //(x-a)*(x-a)+(y-b)*(y-b)=r*r
                if((x-center_x)*(x-center_x)+(y-center_y)*(y-center_y) <= r*r)
                    lcd_display_point(x,y,color);
            }
        }
    }
    else	printf("请输入合理的坐标！");
}


//画空心圆
void lcd_draw_hollow_circle(int x,int y,int r,int color)
{
    int a, b, num;
    a = 0;
    b = r;
    while(22 * b * b >= r * r)// 1/8圆即可
    {
        *(p+800*(x+a)+(y-b)) = color;// 0~1
        *(p+800*(x-a)+(y-b)) = color;// 0~7
        *(p+800*(x-a)+(y+b)) = color;// 4~5
        *(p+800*(x+a)+(y+b)) = color;// 4~3

        *(p+800*(x+b)+(y+a)) = color;// 2~3
        *(p+800*(x+b)+(y-a)) = color;// 2~1
        *(p+800*(x-b)+(y-a)) = color;// 6~7
        *(p+800*(x-b)+(y+a)) = color;// 6~5

        a++;
        //Bresenham画圆法
        num = (a * a + b * b) - r*r;
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}

//清屏
void lcd_clean()
{
    for(int i = 0; i<480; ++i)
    {
        for(int j = 0; j<800; ++j)
            //全屏写入黑色像素
            lcd_display_point(j,i,0x000000);
    }
}



//显示字符
void LCD_PutChar(short x, short y, char c, int fColor, int bColor)
{
    int i,j;
    for(i=0; i<16;i++)    //循环写入16字节，一个字符为16字节
    {
        char m=Font8x16[(c-0x20)*16+i];  //提取c字符的第i个字节以,c减去0x20是由于Ascii码库中的0~1f被去掉
        for(j=0;j<8;j++)  //循环写入8位，一个字节为8位
        {
            if((m&0x80)==0x80)//判断最高位是否为1
            {
                lcd_display_point(x+j,y+i,fColor);
            }
            else
            {
                lcd_display_point(x+j,y+i,bColor);
            }
            m<<=1;       //左移1位，准备写下一位
        }
    }
}

//显示汉字(linux下gcc编译汉字为3字节，此处为g[3])
void Put16x16(short x, short  y, unsigned char g[3], int fColor,int bColor)
{
    int i,j,k,n=0;
    for (k=0;k<FONT_NUM;k++)                        //循环20次，查询汉字字模位置，次数可根据所存储字体数量自定义
    {
        if ((ch16[k].UTF8[0]==g[0])&&(ch16[k].UTF8[1]==g[1])&&(ch16[k].UTF8[2]==g[2]))   //判断第k个汉字的编码是否与输入汉字g[3]相等
        {
            for(i=0;i<32;i++)                 //如果相等，既已找到待显示字模位置，循环写入32字节
            {
                char m=ch16[k].hz16[i];       //读取32字节中的第i字节
                for(j=0;j<8;j++)              //循环写入8位数据
                {
                    if((m&0x80)==0x80)        //判断最高位是否为1,最高位为1，写入字符颜色
                    {
                        if(n==1)
                            lcd_display_point(x+j+8,y+i/2,fColor);
                        else
                            lcd_display_point(x+j,y+i/2,fColor);
                    }
                    else                      //最高位为0，写入背景颜色
                    {
                        if(n==1)
                            lcd_display_point(x+j+8,y+i/2,bColor);
                        else
                            lcd_display_point(x+j,y+i/2,bColor);
                    }
                    m<<=1;                    //左移1位，准备写下一位
                }
                //选用16x16规格的汉字，每一行显示2字节，此处作判断前两字节是否写入
                n++;
                if(n==2) n=0;
            }
        }
    }
}

//================================================================================================
//	实现功能：	显示中英文字符串
//  输入参数：     x 横坐标
//         	y 纵坐标
//		   	*s 待显示的字符串,
//		    bColor 字符背景颜色
//================================================================================================
void LCD_PutString(short y, short x, char *s, int fColor, int bColor)
{
    short xNext = 0;
    short yNext = 0;

    xNext = x;
    yNext = y;
    while (*s)
    {
        unsigned char zh_CN[10];
        zh_CN[0] = *s;                                        //转换为char
        if (zh_CN[0]<0xa1)                                    //显示字符,还是汉字(linux里gcc编译汉字为3字节，首字节>0xa1)
        {
            LCD_PutChar(xNext, yNext, *s, fColor, bColor);      //输出字符
            s++;
            xNext+= 8;                                          //移到下一位显示地址
            if (xNext> 800)
            {
                xNext= 0;
                yNext+= 16;                                     //下移一行
            }
        }
        else
        {
            Put16x16(xNext, yNext, s, fColor, bColor);     	//输出汉字
            s += 3;
            xNext += 16;                                        //移到下一位显示地址
            if (xNext>800)
            {
                xNext=0;
                yNext+=16;                                      //下移一行
            }
        }
    }
}

void lcd_display_bmp(int y0,int x0,char *pathname)
{
    //从文件中读取图片
    int fd = open(pathname,O_RDONLY);
    //是否读取成功
    if(fd == -1)
    {
        perror("");
        printf("open *.bmp fail");
        return ;
    }

    lseek(fd,0x12,SEEK_SET);//定位光标到 宽度位置
    int width;
    read(fd,&width,4);//读取宽度数据

    int height;
    read(fd,&height,4);

    //printf("width=%d,height=%d\n",width,height);

    int i,j;
    int color;

    lseek(fd,54,SEEK_SET);//定位光标到像素数组位置
    unsigned char buf[width*height*3];
    unsigned char lastbuf[width*height*3];

    //保存每一行的无效字节
    int invalid_byte = (4-(width*3)%4)%4;

    //按行读取bmp图片的信息，跳过无效字节
    for (int i= 0; i<height; ++i)
    {
        read(fd,&buf[i*width*3],3*width);
        lseek(fd,invalid_byte,SEEK_CUR);
    }

    //将读取到的三字节像素点按行中间翻转
    for (int i = 0; i < width*3; ++i)
        for (int j = 0; j < height; ++j)
        {
            lastbuf[(height-1-j)*width*3+i] =
                    buf[j*width*3+i];
        }


    unsigned char R,G,B;
    int n = 0;
    for(i=0;i<height;i++)
    {
        for(j=0;j<width;j++)
        {
            //color = 图片第i行第j列的那个像素点的颜色值
            B = lastbuf[n];
            n++;
            G = lastbuf[n];
            n++;
            R = lastbuf[n];
            n++;
            color = R<<16 | G<<8 | B;
            lcd_display_point(j+x0,i+y0,color);
        }
    }

    //关闭图片文件
    close(fd);
}


//  功能：主屏幕显示
void origin_view() {
    lcd_clean();
    lcd_display_bmp(0, 0, "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/background.bmp");

    //显示选项
    //1.环境监测(x:80-205,y:160-285)
    lcd_display_bmp(160, 80, "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/environ.bmp");
    LCD_PutString(285, 105, "1.环境监测", BLACK, 0x72cabf);
    //2.音乐播放器(x:260-378,y:160-285)
    lcd_display_bmp(160, 260, "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/music.bmp");
    LCD_PutString(285, 272, "2.音乐播放器", BLACK, 0x72cabf);
    //3.相册(x:430-547,y:160-285)
    lcd_display_bmp(160, 430, "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/picture.bmp");
    LCD_PutString(285, 465, "3.相册", BLACK, 0x72cabf);
    //4.娱乐(x:600-717,y:160-285)
    lcd_display_bmp(160, 600, "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/game.bmp");
    LCD_PutString(285, 636, "4.娱乐", BLACK, 0x72cabf);
    //5.退出(x:720-800,y:0-80)
    lcd_display_bmp(0, 720, "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/exit.bmp");
}

//  功能：加载
void loading()
{
    //lcd_display_bmp(0, 0, "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/loading1.bmp");
    //sleep(1);
    lcd_display_bmp(0, 0, "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/loading2.bmp");
    sleep(1);
    lcd_display_bmp(0, 0, "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/loading3.bmp");
    sleep(1);
    //lcd_display_bmp(0, 0, "/mnt/hgfs/VMware_Share/my_project/virtual_lcd/source/images/loading4.bmp");
    //sleep(1);
}