#define SIZE 8
#define maxDepth 6
#define max(a,b) a>b?a:b
#define min(a,b) a<b?a:b
#include <iostream>
using namespace std;

static const short sideVal[8] = {1, 1, 1, 2, 3, 4, 6, 7};
static const short dir[8][2] = {{0,1},{0,-1},{1,0},{-1,0},{1,1},{1,-1},{-1,1},{-1,-1}};
double BWFACTOR = 0.08;
double CNFACTOR = 9.50;
double DCFACTOR = 4.80;
double SDFACTOR = 4.00;
double STFACTOR = 1.00;
double FRFACTOR = 0.89;
double MBFACTOR = 0.59;
double CRFACTOR = 0.09;
//显示棋盘上棋子的状态
void Output(char chessboard[][SIZE], int player);
//更新可行动矩阵，同时返回行动力数目(可下子数目)
int Check(char chessboard[][SIZE], int isDown[][SIZE], char player);
//在指定位置下子,并将中间棋子处理成我方棋子
void PlayStep(char chessboard[][SIZE], int row, int col, char player);
//获取当前棋局分数
double GetMaxScore(char chessboard[][SIZE], char player, int myMov, int opMov);
//复制棋盘
void copyChessBoard(char newChessBoard[][SIZE], char oldChessBoard[][SIZE]);
//alpha-beita剪枝算法，进行深度优先搜索
int alphaBeita(char chessboard[][SIZE], int isDown[][SIZE], char player, int depth, int alpha, int beta);
//AI开始自动下子，让其进行第一层遍历
void AIPlayStep(char chessboard[][SIZE], int isDown[][SIZE], int AI, int turnID);


//判断坐标是否处于棋盘内部
bool inRange(int x, int y){
    return (x>=0 && x<8 && y>=0 && y<8);
}
//计算边界稳定子估值
double countSideEval(char chessboard[][SIZE], int myplayer);
//判断内部稳定子
bool isStable(char chessboard[][SIZE], int i, int j);
//设置Eval权重
void setFactor(int trunID);


//显示棋盘上棋子的状态
void Output(char chessboard[][SIZE], int player)
{
    int isDown[SIZE][SIZE];
    Check(chessboard, isDown, player);
	int row, col;
	printf("\n   ");
	//输出列标号
	for (col = 0; col < SIZE; col++)
		printf("  %c ", '1' + col);
	printf("\n");
	//输出项部横线
	printf("  ┌");
	//输出一行
	for (col = 0; col < SIZE-1; col++)
		printf("───┬");
	printf("───┐\n");
	for (row = 0; row < SIZE; row++)
	{
		//输出行号
		printf("%2d│", row + 1);
		//输出棋盘各单元格中棋子的状态
		for (col = 0; col < SIZE; col++)
		{
			if (chessboard[row][col] == -1)//白棋
				printf(" ●│");//表面上看这是个黑球，其实他妈的这是个白球，坑了老子半天
			else if (chessboard[row][col] == 1)//黑棋
				printf(" ○│");
			else//未下子处
			{
			    if(isDown[row][col] == 0)
                    printf("   │");
                else
                    printf(" x │");
			}
		}
		printf("\n");
		if (row < SIZE - 1)
		{
			printf("  ├─");	//输出交叉线
			//输出一行
			for (col = 0; col < SIZE - 1; col++)
			{
				printf("──┼─");
			}
			printf("──┤\n");
		}
	}
	printf("  └─");
	//最后一行的横线
	for (col = 0; col < SIZE - 1; col++)
		printf("──┴─");
	printf("──┘\n");
}

//更新可行动矩阵，同时返回行动力数目(可下子数目)
int Check(char chessboard[][SIZE], int isDown[][SIZE], char player)
{
	int rowdelta, coldelta, row, col, x, y = 0;
	int iStep = 0;
	char opponent = -1*player;	//对方棋子
	char myplayer = player;	//我方棋子
	//将isDown数组全部清0
	for (row = 0; row < SIZE; row++)
		for (col = 0; col < SIZE; col++)
			isDown[row][col] = 0;

	//循环判断棋盘中哪些单元格可以下子
	for (row = 0; row < SIZE; row++)
	{
		for (col = 0; col < SIZE; col++)
		{
			//若棋盘上对应位置不为空(表示已经有子)
			if (chessboard[row][col] != 0)
				continue;//继续处理下一个单元格

			//循环检查上下行
			for (rowdelta = -1; rowdelta <= 1; rowdelta++)
			{
				//循环检查左右列
				for (coldelta = -1; coldelta <= 1; coldelta++)
				{
					//检查若坐标超过棋盘 或为 当前单元格
					if (row + rowdelta < 0 || row + rowdelta >= SIZE
						|| col + coldelta < 0 || col + coldelta >= SIZE
						|| (rowdelta == 0 && coldelta == 0))
						continue;	//继续循环

					//若(row,col)四周有对手下的子
					if (chessboard[row + rowdelta][col + coldelta] == opponent)
					{
						//以对手下子位置为坐标
						x = row + rowdelta;
						y = col + coldelta;
						//对对手下子为起始点，向四周查找自己方的棋子，以攻击对方棋子
						while(1)
						{
							//对手下子的四周坐标
							x += rowdelta;
							y += coldelta;

							//超过棋盘
							if (x < 0 || x >= SIZE || y < 0 || y >= SIZE)
								break;	//退出循环

							//若对应位置为空
							if (chessboard[x][y] == 0)
								break;

							//若对应位置下的子是当前棋手的
							if (chessboard[x][y] == myplayer)
							{
								//设置移动数组中对应位置为1 (该位置可下子，形成向对手进攻的棋形)
								isDown[row][col] = 1;
								iStep++;	//累加可下子的位置数量
								break;
							}
						}
					}
				}
			}
		}
	}
	//返回可下的位置数量（若返回值为0，表示没地方可下）
	return iStep;
}

//在指定位置下子,并将中间棋子处理成我方棋子
void PlayStep(char chessboard[][SIZE], int row, int col, char player)
{
	int rowdelta = 0;
	int coldelta = 0;
	int x = 0;
	int y = 0;
	char opponent = -1*player;	//对方棋子
	char myplayer = player;	//我方棋子
	chessboard[row][col] = myplayer;	//保存所下的棋子
	//检查所下子四周的棋子
	for (rowdelta = -1; rowdelta <= 1; rowdelta++)
	{
		for (coldelta = -1; coldelta <= 1; coldelta++)
		{
			//若坐标超过棋盘界限or方向不移动
			if (row + rowdelta < 0 || row + rowdelta >= SIZE || col + coldelta < 0
				|| col + coldelta >= SIZE || (rowdelta == 0 && coldelta == 0))
			{
				continue;	//继续下一位置
			}
			//若该位置是对手的棋子
			if (chessboard[row + rowdelta][col + coldelta] == opponent)
			{
				//以对手棋为坐标
				x = row + rowdelta;
				y = col + coldelta;
				//在对手棋子四周寻找我方棋子
				while(1)
				{
					x += rowdelta;
					y += coldelta;
					//若坐标超过棋盘
					if (x < 0 || x >= SIZE || y < 0 || y >= SIZE)
						break;	//退出循环

					//若对应位置为空
					if (chessboard[x][y] == 0)
						break;	//退出循环

					//若对应位置是我方模子
					if (chessboard[x][y] == myplayer)
					{
						//循环处理
						while (chessboard[x -= rowdelta][y -= coldelta] == opponent)
							//将中间的棋子都变成我方棋子
							chessboard[x][y] = myplayer;

						break;	//退出循环
					}
				}
			}
		}
	}
}

//获取当前棋局分数
/*
int GetMaxScore(char chessboard[][SIZE], char player)
{
    int weight[SIZE][SIZE] =
    {
//        {20,-3,11, 8, 8,11,-3,20},
//        {-3,-7,-4, 1, 1,-4,-7,-3},
//        {11,-4, 2, 2, 2, 2,-4,11},
//        { 8, 1, 2,-3,-3, 2, 1, 8},
//        { 8, 1, 2,-3,-3, 2, 1, 8},
//        {11,-4, 2, 2, 2, 2,-4,11},
//        {-3,-7,-4, 1, 1,-4,-7,-3},
//        {20,-3,11, 8, 8,11,-3,20}
        { 100, -5,  10,   5,   5,  10,  -5, 100},
        { -5, -45,   1,   1,   1,   1, -45,  -5},
        { 10,   1,   3,   2,   2,   3,   1,  10},
        {  5,   1,   2,   1,   1,   2,   1,   5},
        {  5,   1,   2,   1,   1,   2,   1,   5},
        { 10,   1,   3,   2,   2,   3,   1,  10},
        { -5, -45,   1,   1,   1,   1, -45,  -5},
        { 100, -5,  10,   5,   5,  10,  -5, 100}
    };
    int Score = 0;
    char opponent = -1*player;	//对方棋子
	char myplayer = player;
    for(int row = 0; row < SIZE; row++){
        for(int col = 0; col < SIZE; col++){
            //若棋盘对应位置是对手下的棋子，从总分中减1
			Score -= weight[row][col]*(chessboard[row][col] == opponent);
			//若棋盘对应位置是我方的棋子，总分中加1分
            Score += weight[row][col]*(chessboard[row][col] == myplayer);
        }
    }
    return Score;
}
*/

double GetMaxScore(char chessboard[][SIZE], char player, int myMov, int opMov)
{
    double CharaEval=0, BWRateEval=0, CornerEval=0, DCornerEval=0, SideEval=0;
    double StableEval=0, MobEval=0, FrontlineEval=0, FrontlineRateEval=0;
    int weight[SIZE][SIZE] =
    {
        {20,-3,11, 8, 8,11,-3,20},
        {-3,-7,-4, 1, 1,-4,-7,-3},
        {11,-4, 2, 2, 2, 2,-4,11},
        { 8, 1, 2,-3,-3, 2, 1, 8},
        { 8, 1, 2,-3,-3, 2, 1, 8},
        {11,-4, 2, 2, 2, 2,-4,11},
        {-3,-7,-4, 1, 1,-4,-7,-3},
        {20,-3,11, 8, 8,11,-3,20}
    };
    int opNum=0, myNum=0;
    char opponent = -1*player;	//对方棋子
	char myplayer = player;

	//基于位置特征的估值------------------------------------------------------
    for(int row = 0; row < SIZE; row++){
        for(int col = 0; col < SIZE; col++){
            //若棋盘对应位置是对手下的棋子，从总分中减1
			CharaEval -= weight[row][col]*(chessboard[row][col] == opponent);
			opNum += (chessboard[row][col] == opponent);
			//若棋盘对应位置是我方的棋子，总分中加1分
            CharaEval += weight[row][col]*(chessboard[row][col] == myplayer);
            myNum += (chessboard[row][col] == myplayer);
        }
    }

    //基于黑白子数比例的估值--------------------------------------------------
    if(myNum > opNum)
        BWRateEval = 100.0 * myNum/ (myNum + opNum);
    else if(myNum < opNum)
        BWRateEval = -100.0* opNum/ (myNum + opNum);
    else
        BWRateEval = 0;

    //基于角位棋子的估值------------------------------------------------------
    int Corner[4][2] = {{0,0},{0,7},{7,0},{7,7}};
    for(int i = 0; i < 4; i++){
        CornerEval += (chessboard[Corner[i][0]][Corner[i][1]] == myplayer);
        CornerEval -= (chessboard[Corner[i][0]][Corner[i][1]] == opponent);
    }
    CornerEval = 25*CornerEval;

    //基于靠近角位的危险位置的估值（*位和+位）--------------------------------
    int Star[4][2] = {{1,1},{1,6},{6,1},{6,6}};
    int Add[8][2] = {{0,1},{1,0},{0,6},{1,7},{6,0},{7,1},{6,7},{7,6}};
    for(int i = 0; i < 8; i++){
        DCornerEval += (chessboard[Corner[i/2][0]][Corner[i/2][1]] == 0) && (chessboard[Add[i][0]][Add[i][1]] == myplayer);
        DCornerEval -= (chessboard[Corner[i/2][0]][Corner[i/2][1]] == 0) && (chessboard[Add[i][0]][Add[i][1]] == opponent);
    }
    for(int i = 0; i < 4; i++){
        DCornerEval += 2* ((chessboard[Corner[i][0]][Corner[i][1]] == 0) && (chessboard[Star[i][0]][Star[i][1]] == myplayer));
        DCornerEval -= 2* ((chessboard[Corner[i][0]][Corner[i][1]] == 0) && (chessboard[Star[i][0]][Star[i][1]] == opponent));
    }
    DCornerEval = -12.5*DCornerEval;

    //基于边界稳定子的估值----------------------------------------------------
    SideEval = 2.5*countSideEval(chessboard, myplayer);

    //基于内部稳定子的估值----------------------------------------------------
    for(int i = 1; i < 7; i++){
        for(int j = 1; j < 7; j++){
            if(isStable(chessboard, i, j)){
                StableEval += (chessboard[i][j] == myplayer);
                StableEval -= (chessboard[i][j] == opponent);
            }
        }
    }
    StableEval = 12.5*StableEval;

    //基于行动力的估值--------------------------------------------------------
    if(!opMov)//若对方无子可下
        MobEval = 150;
    else if(!myMov)
        MobEval = -450;
    else if(myMov > opMov)
        MobEval = (100.0 * myMov)/(myMov + opMov);
    else if(myMov < opMov)
        MobEval = (-100.0* opMov)/(myMov + opMov);
    else
        MobEval = 0;

    //基于黑白边界的估值------------------------------------------------------
    int myFront = 0;
    int opFront = 0;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(chessboard[i][j] != 0) //如果不是空位，则跳过
                continue;
            for(int k = 0; k < 8; k++){ //对每一个空位，对其周围遍历
                int x = i + dir[k][0];
                int y = j + dir[k][1];
                int isMine = 0;
                int isOpp = 0;
                if(inRange(x, y)){
                    if(chessboard[x][y] == myplayer)
                        isMine = 1;
                    else if(chessboard[x][y] == opponent)
                        isOpp = 1;
                }
                myFront += isMine;
                opFront += isOpp;
            }
        }
    }
    if(myFront > opFront)
        FrontlineRateEval = -100.0*myFront/(myFront + opFront);
    else if(myFront < opFront)
        FrontlineRateEval = 100.0*opFront/(myFront + opFront);
    else
        FrontlineRateEval = 0;
/*
    cout<<"______________________________________"<<endl;
    cout<<"BWRateEval: "<<BWRateEval<<endl;
    cout<<"CornerEval: "<<CornerEval<<endl;
    cout<<"DCornerEval:"<<DCornerEval<<endl;
    cout<<"SideEval:  "<<SideEval<<endl;
    cout<<"StableEval: "<<StableEval<<endl;
    cout<<"MobEval: "<<MobEval<<endl;
    cout<<"FrontlineRateEval: "<<FrontlineRateEval<<endl;
    cout<<"CharaEval: "<<CharaEval<<endl;
*/
    double Eval = 0;
    Eval = (BWFACTOR * BWRateEval) + \
    (CNFACTOR * CornerEval) + \
    (DCFACTOR * DCornerEval) + \
    (SDFACTOR * SideEval) + \
    (STFACTOR * StableEval) + \
    (MBFACTOR * MobEval) + \
    (FRFACTOR * FrontlineRateEval) + \
    (CRFACTOR * CharaEval);
//    cout<<"Eval: "<<Eval<<endl;
//    cout<<"______________________________________"<<endl;
    return Eval;
}


//复制棋盘
void copyChessBoard(char newChessBoard[][SIZE], char oldChessBoard[][SIZE]){
    for(int row = 0; row < SIZE; row++)
        for(int col = 0; col < SIZE; col++)
            newChessBoard[row][col] = oldChessBoard[row][col];
}

//alpha-beita剪枝算法，进行深度优先搜索
int alphaBeita(char chessboard[][SIZE], int isDown[][SIZE], char player, int depth, int alpha, int beta, int AI)
{
    //建立新的chessboard和isDown
    char newchessboard[SIZE][SIZE];
    int newisDown[SIZE][SIZE];
    int AIMov, opMov;

    AIMov = Check(chessboard, newisDown, AI);
    opMov = Check(chessboard, newisDown, -1*AI);

    //如果不可下子 or 到达最大深度，返回评估函数值，同时更新newisDown权值
    if(depth == maxDepth || !Check(chessboard, newisDown, player)){
        return GetMaxScore(chessboard, AI, AIMov, opMov);
    }

    if(player == AI){ //如果AI下棋,返回最大的值
        for(int row = 0; row < SIZE; row++){ // 遍历整个棋盘
            for(int col = 0; col < SIZE; col++){
                if(!newisDown[row][col]) // 如果此处不可下棋
                    continue;

                copyChessBoard(newchessboard, chessboard);//复制棋盘
                PlayStep(newchessboard, row, col, player);//在复制的棋盘上下棋
                alpha = max(alpha, alphaBeita(newchessboard, newisDown, -1*AI, depth+1, alpha, beta, AI));
                if(alpha >= beta) //alpha-beta剪枝
                    return alpha;
            }
        }
        return alpha;
    }
    else{//如果人类下棋
        for(int row = 0; row < SIZE; row++){//遍历整个棋盘
            for(int col = 0; col < SIZE; col++){
                if(!newisDown[row][col]) // 如果此处不可下棋
                    continue;

                copyChessBoard(newchessboard, chessboard);//复制棋盘
                PlayStep(newchessboard, row, col, player);//在复制的棋盘上下棋
                beta = min(beta, alphaBeita(newchessboard, newisDown, AI, depth+1, alpha, beta, AI));
                if(alpha >= beta)//alpha-beta剪枝
                    return beta;
            }
        }
        return beta;
    }
}

//AI开始自动下子，让其进行第一层遍历, player代表AI, turnID代表回合数
void AIPlayStep(char chessboard[][SIZE], int isDown[][SIZE], int AI, int turnID){
    setFactor(turnID);
    char newchessboard[SIZE][SIZE];
    char bestchessboard[SIZE][SIZE];
    int newisDown[SIZE][SIZE];

    copyChessBoard(bestchessboard, chessboard);
    int opMov = Check(chessboard, newisDown, -1*AI);
    int myMov = Check(chessboard, newisDown, AI);

    int alpha = -9999;
    int beta = 9999;
    int nodealpha;

    int bestrow, bestcol;
    for(int row = 0; row < SIZE; row++){
        for(int col = 0; col < SIZE; col++){
            if(!newisDown[row][col]) continue;
            copyChessBoard(newchessboard, chessboard);//复制棋盘
            PlayStep(newchessboard, row, col, AI);//在复制的棋盘上下棋
            //得到子节点的alpha值
            nodealpha = alphaBeita(newchessboard, newisDown, -1*AI, 1, alpha, beta, AI);
            if(alpha < nodealpha){ //alpha-beta剪枝算法
                copyChessBoard(bestchessboard, newchessboard);
                alpha = nodealpha;
                bestrow = row;
                bestcol = col;
            }
        }
    }
    copyChessBoard(chessboard, bestchessboard); //将棋盘更新为最好的落子点
    cout<<"Depth:"<<maxDepth<<"  预估alpha:"<<alpha<<endl;
    cout<<"AI find: "<<bestrow+1<<" "<<bestcol+1<<endl;
    cout<<"当前AI方估值："<<GetMaxScore(chessboard, AI, myMov, opMov);
}
//计算边界稳定子估值
double countSideEval(char chessboard[][SIZE], int myplayer){
    double SideEval = 0;
    int opponent = -1*myplayer;
    //int corner[4][2] = {{0,0},{0,7},{7,0},{7,7}};
    if(chessboard[0][0] == myplayer){
        for(int i = 0; i < 8; i++){
            if(chessboard[0][i] == myplayer)
                SideEval += sideVal[i];
            else
                break;
        }
        for(int i = 0; i < 8; i++){
            if(chessboard[i][0] == myplayer)
                SideEval += sideVal[i];
            else
                break;
        }
    }
    else if(chessboard[0][0] == opponent){
        for(int i = 0; i < 8; i++){
            if(chessboard[0][i] == opponent)
                SideEval -= sideVal[i];
            else
                break;
        }
        for(int i = 0; i < 8; i++){
            if(chessboard[i][0] == opponent)
                SideEval -= sideVal[i];
            else
                break;
        }
    }

    if(chessboard[0][7] == myplayer){
        for(int i = 0; i < 8; i++){
            if(chessboard[0][7-i] == myplayer)
                SideEval += sideVal[i];
            else
                break;
        }
        for(int i = 0; i < 8; i++){
            if(chessboard[i][7] == myplayer)
                SideEval += sideVal[i];
            else
                break;
        }
    }
    else if(chessboard[0][7] == opponent){
        for(int i = 0; i < 8; i++){
            if(chessboard[0][7-i] == opponent)
                SideEval -= sideVal[i];
            else
                break;
        }
        for(int i = 0; i < 8; i++){
            if(chessboard[i][7] == opponent)
                SideEval -= sideVal[i];
            else
                break;
        }
    }

    if(chessboard[7][0] == myplayer){
        for(int i = 0; i < 8; i++){
            if(chessboard[7][i] == myplayer)
                SideEval += sideVal[i];
            else
                break;
        }
        for(int i = 0; i < 8; i++){
            if(chessboard[7-i][0] == myplayer)
                SideEval += sideVal[i];
            else
                break;
        }
    }
    else if(chessboard[7][0] == opponent){
        for(int i = 0; i < 8; i++){
            if(chessboard[7][i] == opponent)
                SideEval -= sideVal[i];
            else
                break;
        }
        for(int i = 0; i < 8; i++){
            if(chessboard[7-i][0] == opponent)
                SideEval -= sideVal[i];
            else
                break;
        }
    }

    if(chessboard[7][7] == myplayer){
        for(int i = 0; i < 8; i++){
            if(chessboard[7][7-i] == myplayer)
                SideEval += sideVal[i];
            else
                break;
        }
        for(int i = 0; i < 8; i++){
            if(chessboard[7-1][7] == myplayer)
                SideEval += sideVal[i];
            else
                break;
        }
    }
    else if(chessboard[7][7] == opponent){
        for(int i = 0; i < 8; i++){
            if(chessboard[7][7-i] == opponent)
                SideEval -= sideVal[i];
            else
                break;
        }
        for(int i = 0; i < 8; i++){
            if(chessboard[7-1][7] == opponent)
                SideEval -= sideVal[i];
            else
                break;
        }
    }
    return SideEval;
}

//判断内部稳定子
bool isStable(char chessboard[][SIZE], int x, int y){
    for(int i = 0; i < 8; i++){
        for(int nx = x + dir[i][0], ny = y + dir[i][1];
        inRange(nx,ny); nx += dir[i][0], ny += dir[i][1])
            if(chessboard[nx][ny] == 0)
                return false;
    }
    return true;
}

//设置Eval权重
void setFactor(int turnID){
    if (turnID > 23) //根据回合数调整参数
    {
        BWFACTOR = 0.20; //终局时调高黑白子比例参数
        DCFACTOR = 0.60; //调低危险角子参数
        CNFACTOR = 8.00;
        SDFACTOR = 6.00;
        STFACTOR = 4.00;
        FRFACTOR = 0.24; //调低边界长参数
        MBFACTOR = 0.24; //调低行动力参数
        CRFACTOR = 0.01; //调低特征估值参数
    }
    else if (turnID > 21)
    {
        BWFACTOR = 0.15;
        DCFACTOR = 1.20;
        CNFACTOR = 8.00;
        SDFACTOR = 6.00;
        STFACTOR = 4.00;
        FRFACTOR = 0.24;
        MBFACTOR = 0.24;
        CRFACTOR = 0.01;
    }
    else if (turnID > 18)
    {
        BWFACTOR = 0.12;
        CNFACTOR = 8.00;
        DCFACTOR = 1.50;
        SDFACTOR = 5.00;
        STFACTOR = 3.00;
        FRFACTOR = 0.66;
        MBFACTOR = 0.45;
        CRFACTOR = 0.04;
    }
    else if (turnID > 14)
    {
        BWFACTOR = 0.08;
        CNFACTOR = 9.50;
        DCFACTOR = 4.80;
        SDFACTOR = 4.00;
        STFACTOR = 1.00;
        FRFACTOR = 0.89;
        MBFACTOR = 0.59;
        CRFACTOR = 0.09;
    }
    else if (turnID > 8)
    {
        BWFACTOR = 0.06;
        CNFACTOR = 9.50;
        DCFACTOR = 4.00;
        SDFACTOR = 4.00;
        STFACTOR = 0.00;
        FRFACTOR = 1.02 - double(turnID) / 50;
        MBFACTOR = 0.71 - double(turnID) / 60;
        CRFACTOR = 0.12;
    }
    else
    {
        BWFACTOR = 0.04; //开局时不重视黑白比例
        CNFACTOR = 7.50;
        DCFACTOR = 3.90;
        SDFACTOR = 0.00;
        STFACTOR = 0.00; //不计算基于稳定子的估值
        FRFACTOR = 1.27 - double(turnID) / 50; //重视行动力估值
        MBFACTOR = 0.82 - double(turnID) / 60;
        CRFACTOR = 0.13; //重视位置特征估值
    }
}
