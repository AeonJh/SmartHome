//
// Created by aeonjh on 23-1-8.
//

#ifndef VIRTUAL_LCD_GAME2048_H
#define VIRTUAL_LCD_GAME2048_H

//游戏主进程
void game2048();

//游戏初始化
void game_init();

//方块移动及合成
int game_move(int flag);

//判断游戏是否结束
//通过判断相邻元素是否相等
int game_over();

//随机生成新方块
void game_generate();

//绘制图形
void game_draw();

#endif //VIRTUAL_LCD_GAME2048_H
