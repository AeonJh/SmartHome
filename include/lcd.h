#ifndef __LCD_H__
#define __LCD_H__

//颜色定义
#define RED                 0xff0000              //红色
#define GREEN               0x00ff00              //绿色
#define BLUE                0x0000ff              //蓝色
#define YELLOW              0xffff00              //黄色
#define BLACK               0x000000              //黑色
#define WHITE               0xffffff              //白色
#define SEAGREEN            0x43CD80              //海绿


/*
	功能：初始化显示屏
	在显示内容之前应该调用该函数一次（只能调用一次）
	返回值：
		初始化成功返回1
		初始化失败返回0
		
*/
int lcd_init();



/*
	功能：关闭屏幕及接触映射，和 lcd_init函数成对出现
			显示完所有内容之后调用函数
*/
void lcd_uninit();


/*
	功能：在屏幕上显示一个像素点
	参数：x :横坐标(列)
		 y :纵坐标(行)
		 color:颜色值
*/
void lcd_display_point(int x,int y,int color);


/*
	功能：在屏幕上画矩形
	参数：origin_x :起始横坐标(列)
		 origin_y :起始纵坐标(行)
		 length :矩形长度
		 width :矩形宽度
		 color:颜色值
*/
void lcd_draw_solid_rectangle(int origin_y,int origin_x,int length,int width,int color);//实心

void lcd_draw_hollow_rectangle(int origin_y,int origin_x,int length,int width,int color);//空心


/*
	功能：在屏幕上画空心圆
	参数：center_x :圆心横坐标(列)
		 center_y :圆心纵坐标(行)
		 r :半径
		 color:颜色值
*/
void lcd_draw_solid_circle(int center_y,int center_x,int r,int color);


/*
	功能：在屏幕上画实心圆
	参数：center_x :圆心横坐标(列)
		 center_y :圆心纵坐标(行)
		 r :半径
		 color:颜色值
*/
void lcd_draw_hollow_circle(int center_y,int center_x,int r,int color);


//================================================================================================
//	实现功能：显示字符
//  输入参数：x 横坐标
//         	y 纵坐标
//		   	c 待显示的字符
//          fColor 字符颜色
//		    bColor 字符背景颜色
//================================================================================================
void LCD_PutChar(short x, short y, char c, int fColor, int bColor);


//================================================================================================
//	实现功能：显示中文字符
//  输入参数：x 横坐标
//         	y 纵坐标
//		   	g[] 待显示的字符
//          fColor 字符颜色
//		    bColor 字符背景颜色
//================================================================================================
void Put16x16(short x, short  y, unsigned char g[3], int fColor,int bColor);


//================================================================================================
//	实现功能：显示中英文字符串
//  输入参数：x 横坐标
//         	y 纵坐标
//		   	*s 待显示的字符串,
//		    bColor 字符背景颜色
//================================================================================================
void LCD_PutString(short x, short y, char *s, int fColor, int bColor);


//  功能：显示bmp格式图片
//  参数：pathname 图片路径
void lcd_display_bmp(int x0,int y0,char *pathname);


//  功能：恢复屏幕到默认状态(全黑)
void lcd_clean();

//  功能：主屏幕显示
void origin_view();

//  功能：加载
void loading();

#endif
