#include <iostream>
#include <windows.h>
#include <stdio.h>
#include "head.cpp"

#define SIZE 8
#define max(a,b) a>b?a:b
#define min(a,b) a<b?a:b
using namespace std;
/*
//显示棋盘上棋子的状态
void Output(char chessboard[][SIZE]);

//更新可行动矩阵，同时返回行动力数目(可下子数目)
int Check(char chessboard[][SIZE], int isDown[][SIZE], char player);

//在指定位置下子,并将中间棋子处理成我方棋子
void PlayStep(char chessboard[][SIZE], int row, int col, char player);

//获取当前棋局分数
int GetMaxScore(char chessboard[][SIZE], char player);

//复制棋盘
void copyChessBoard(char newChessBoard[][SIZE], char oldChessBoard[][SIZE]);

//alpha-beita剪枝算法，进行深度优先搜索
int alphaBeita(char chessboard[][SIZE], int isDown[][SIZE], char player, int depth, int alpha, int beta);

//AI开始自动下子，让其进行第一层遍历
void AIPlayStep(char chessboard[][SIZE], int isDown[][SIZE], int AI, int turnID);


棋盘数据结构:
    //保存棋盘中各单元格下子的状态, -1为白(游戏者)， 1为黑(AI)
	char chessboard[8][8];

保存棋盘对某一方可下子位置：
    //保存棋盘中各位置是否可以下子，可下子的位置为1，其余位置为0
	int isDown[8][8] = { 0 };

当前回合下棋方：
    //0表示游戏者-白,1表示AI-黑
    int player = -1;	//下棋方，-1为AI后手，1为AI先手
    经过第一层循环后会变成
    player = -1 表示游戏者正在下棋
    player = 1  表示AI正在下棋

注：
1.遍历棋盘时为每一步建立一个棋盘矩阵
2.游戏规则实现在main中
3.采用的是矩阵位置评估函数
4.游戏永远是黑棋先下
*/
int main()
{
	//保存棋盘中各单元格下子的状态
	char chessboard[SIZE][SIZE];
	//保存棋盘中各位置是否可以下子，可下子的位置为1，其余位置为0
	int isDown[SIZE][SIZE] = { 0 };
	int row, col, x, y;
	//已下棋子数量
	int iCount = 0;
	int player, AI;	//当前下棋方,AI方
	//跳过下子的次数，若为2，表示双方都不能下子
	int SkipPlay = 0;

	//保存AI和游戏者的得分
	int Score[2];
	char select;
	printf("黑白棋\n\n");
	printf("游戏者执白先下，AI执黑，按回车键开始:\n");
	getchar();

    cout<<"请选择AI先手or后手，先手1(黑)，后手-1(白): ";
    cin>>AI;

    //设置当前下棋方，黑棋先下故设置为1
    player = 1;
    iCount = 4;	//累计下子数
    //棋盘各位置清空
    for (row = 0; row < SIZE; row++)
        for (col = 0; col < SIZE; col++)
            chessboard[row][col] = 0;

    //在棋盘中间位置放置黑棋
    chessboard[SIZE/2 - 1][SIZE/2 - 1] = chessboard[SIZE/2][SIZE/2] = 1;
    //在棋盘中间位置放置白棋
    chessboard[SIZE/2 - 1][SIZE/2] = chessboard[SIZE/2][SIZE/2 - 1] = -1;

    printf("\n棋盘初始状态:\n");
    //显示初始棋盘下子的状况
    Output(chessboard, -1*AI);

    int turnID = 0;//回合数
    do
    {
        //若是AI下棋
        if (player != AI)
        {
            //判断是否可下白子
            if (Check(chessboard, isDown, player))
            {
                SkipPlay = 0;	//清除无效下子次数
                //死循环，直到用户输入正确的坐标为止
                while(1)
                {
                    printf("输入下子的位置(行 列):");
                    cin>>x>>y;
                    x--;	//计算行坐标位置
                    y--;	//计算列坐标位置
                    //若行列坐标输入有效

                    if (x >= 0 && y >= 0 && x < SIZE && y < SIZE && isDown[x][y])
                    {
                        //在指定坐标位置下子
                        PlayStep(chessboard, x, y, player);
                        iCount++;	//累加下子数
                        break;
                    }
                    else
                    {
                        fflush(stdin);	//清空输入缓冲区
                        printf("坐标输入错误，请重新输入。\n\n");
                    }
                }

                printf("\n你下子后的状态:\n");
                Output(chessboard, -1*AI);	//显示棋子状态
                printf("按任意键AI下子。\n");
                getchar();getchar();
            }
            else if (++SkipPlay < 2)//若无效下子的次数小于2
            {
                printf("你没位置可下，按回车键让对方下子。");
                getchar();
            }else
                printf("双方都没地方下子，游戏结束!\n");
        }
        //若是AI下棋
        else
        {

            if (Check(chessboard, isDown, player))
            {
                SkipPlay = 0;	//清除无效下子次数
                AIPlayStep(chessboard, isDown, player, turnID);
                iCount++;	//累加下子数
                printf("\nAI下子后的状态:\n");
                Output(chessboard, -1*AI);	//显示棋子状态
            }
            else
            {
                //若无效下子次数小于2
                if (++SkipPlay < 2)
                    printf("我没位置可走，请你走。\n");
                else
                    printf("双方都没地方下子，游戏结束!");
            }
        }
        player = -1*player;//交换下棋方
        turnID++;//回合数++
    }while (iCount < SIZE*SIZE && SkipPlay < 2);//下子数量小于64 且无效下子的次数小于2

    //显示各双方棋子的状况
    Output(chessboard, -1*AI);
    Score[0] = Score[1] = 0;//清空计分变量
    //循环统计各单元格黑白棋子的数量
    for (row = 0; row < SIZE; row++)
    {
        for (col = 0; col < SIZE; col++)
        {
            //统计黑子数
            Score[0] += chessboard[row][col] == 1;
            //统计白子数
            Score[1] += chessboard[row][col] == -1;
        }
    }
    printf("最终成绩:\n");
    printf("游戏者:%d\n  AI:%d\n", Score[1], Score[0]);
    fflush(stdin);	//清空输入缓冲区

	printf("Game Over!\n");
	getchar();
	return 0;
}

