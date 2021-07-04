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
//��ʾ���������ӵ�״̬
void Output(char chessboard[][SIZE], int player);
//���¿��ж�����ͬʱ�����ж�����Ŀ(��������Ŀ)
int Check(char chessboard[][SIZE], int isDown[][SIZE], char player);
//��ָ��λ������,�����м����Ӵ�����ҷ�����
void PlayStep(char chessboard[][SIZE], int row, int col, char player);
//��ȡ��ǰ��ַ���
double GetMaxScore(char chessboard[][SIZE], char player, int myMov, int opMov);
//��������
void copyChessBoard(char newChessBoard[][SIZE], char oldChessBoard[][SIZE]);
//alpha-beita��֦�㷨�����������������
int alphaBeita(char chessboard[][SIZE], int isDown[][SIZE], char player, int depth, int alpha, int beta);
//AI��ʼ�Զ����ӣ�������е�һ�����
void AIPlayStep(char chessboard[][SIZE], int isDown[][SIZE], int AI, int turnID);


//�ж������Ƿ��������ڲ�
bool inRange(int x, int y){
    return (x>=0 && x<8 && y>=0 && y<8);
}
//����߽��ȶ��ӹ�ֵ
double countSideEval(char chessboard[][SIZE], int myplayer);
//�ж��ڲ��ȶ���
bool isStable(char chessboard[][SIZE], int i, int j);
//����EvalȨ��
void setFactor(int trunID);


//��ʾ���������ӵ�״̬
void Output(char chessboard[][SIZE], int player)
{
    int isDown[SIZE][SIZE];
    Check(chessboard, isDown, player);
	int row, col;
	printf("\n   ");
	//����б��
	for (col = 0; col < SIZE; col++)
		printf("  %c ", '1' + col);
	printf("\n");
	//��������
	printf("  ��");
	//���һ��
	for (col = 0; col < SIZE-1; col++)
		printf("��������");
	printf("��������\n");
	for (row = 0; row < SIZE; row++)
	{
		//����к�
		printf("%2d��", row + 1);
		//������̸���Ԫ�������ӵ�״̬
		for (col = 0; col < SIZE; col++)
		{
			if (chessboard[row][col] == -1)//����
				printf(" ��");//�����Ͽ����Ǹ�������ʵ��������Ǹ����򣬿������Ӱ���
			else if (chessboard[row][col] == 1)//����
				printf(" ��");
			else//δ���Ӵ�
			{
			    if(isDown[row][col] == 0)
                    printf("   ��");
                else
                    printf(" x ��");
			}
		}
		printf("\n");
		if (row < SIZE - 1)
		{
			printf("  ����");	//���������
			//���һ��
			for (col = 0; col < SIZE - 1; col++)
			{
				printf("�����੤");
			}
			printf("������\n");
		}
	}
	printf("  ����");
	//���һ�еĺ���
	for (col = 0; col < SIZE - 1; col++)
		printf("�����ة�");
	printf("������\n");
}

//���¿��ж�����ͬʱ�����ж�����Ŀ(��������Ŀ)
int Check(char chessboard[][SIZE], int isDown[][SIZE], char player)
{
	int rowdelta, coldelta, row, col, x, y = 0;
	int iStep = 0;
	char opponent = -1*player;	//�Է�����
	char myplayer = player;	//�ҷ�����
	//��isDown����ȫ����0
	for (row = 0; row < SIZE; row++)
		for (col = 0; col < SIZE; col++)
			isDown[row][col] = 0;

	//ѭ���ж���������Щ��Ԫ���������
	for (row = 0; row < SIZE; row++)
	{
		for (col = 0; col < SIZE; col++)
		{
			//�������϶�Ӧλ�ò�Ϊ��(��ʾ�Ѿ�����)
			if (chessboard[row][col] != 0)
				continue;//����������һ����Ԫ��

			//ѭ�����������
			for (rowdelta = -1; rowdelta <= 1; rowdelta++)
			{
				//ѭ�����������
				for (coldelta = -1; coldelta <= 1; coldelta++)
				{
					//��������곬������ ��Ϊ ��ǰ��Ԫ��
					if (row + rowdelta < 0 || row + rowdelta >= SIZE
						|| col + coldelta < 0 || col + coldelta >= SIZE
						|| (rowdelta == 0 && coldelta == 0))
						continue;	//����ѭ��

					//��(row,col)�����ж����µ���
					if (chessboard[row + rowdelta][col + coldelta] == opponent)
					{
						//�Զ�������λ��Ϊ����
						x = row + rowdelta;
						y = col + coldelta;
						//�Զ�������Ϊ��ʼ�㣬�����ܲ����Լ��������ӣ��Թ����Է�����
						while(1)
						{
							//�������ӵ���������
							x += rowdelta;
							y += coldelta;

							//��������
							if (x < 0 || x >= SIZE || y < 0 || y >= SIZE)
								break;	//�˳�ѭ��

							//����Ӧλ��Ϊ��
							if (chessboard[x][y] == 0)
								break;

							//����Ӧλ���µ����ǵ�ǰ���ֵ�
							if (chessboard[x][y] == myplayer)
							{
								//�����ƶ������ж�Ӧλ��Ϊ1 (��λ�ÿ����ӣ��γ�����ֽ���������)
								isDown[row][col] = 1;
								iStep++;	//�ۼӿ����ӵ�λ������
								break;
							}
						}
					}
				}
			}
		}
	}
	//���ؿ��µ�λ��������������ֵΪ0����ʾû�ط����£�
	return iStep;
}

//��ָ��λ������,�����м����Ӵ�����ҷ�����
void PlayStep(char chessboard[][SIZE], int row, int col, char player)
{
	int rowdelta = 0;
	int coldelta = 0;
	int x = 0;
	int y = 0;
	char opponent = -1*player;	//�Է�����
	char myplayer = player;	//�ҷ�����
	chessboard[row][col] = myplayer;	//�������µ�����
	//������������ܵ�����
	for (rowdelta = -1; rowdelta <= 1; rowdelta++)
	{
		for (coldelta = -1; coldelta <= 1; coldelta++)
		{
			//�����곬�����̽���or�����ƶ�
			if (row + rowdelta < 0 || row + rowdelta >= SIZE || col + coldelta < 0
				|| col + coldelta >= SIZE || (rowdelta == 0 && coldelta == 0))
			{
				continue;	//������һλ��
			}
			//����λ���Ƕ��ֵ�����
			if (chessboard[row + rowdelta][col + coldelta] == opponent)
			{
				//�Զ�����Ϊ����
				x = row + rowdelta;
				y = col + coldelta;
				//�ڶ�����������Ѱ���ҷ�����
				while(1)
				{
					x += rowdelta;
					y += coldelta;
					//�����곬������
					if (x < 0 || x >= SIZE || y < 0 || y >= SIZE)
						break;	//�˳�ѭ��

					//����Ӧλ��Ϊ��
					if (chessboard[x][y] == 0)
						break;	//�˳�ѭ��

					//����Ӧλ�����ҷ�ģ��
					if (chessboard[x][y] == myplayer)
					{
						//ѭ������
						while (chessboard[x -= rowdelta][y -= coldelta] == opponent)
							//���м�����Ӷ�����ҷ�����
							chessboard[x][y] = myplayer;

						break;	//�˳�ѭ��
					}
				}
			}
		}
	}
}

//��ȡ��ǰ��ַ���
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
    char opponent = -1*player;	//�Է�����
	char myplayer = player;
    for(int row = 0; row < SIZE; row++){
        for(int col = 0; col < SIZE; col++){
            //�����̶�Ӧλ���Ƕ����µ����ӣ����ܷ��м�1
			Score -= weight[row][col]*(chessboard[row][col] == opponent);
			//�����̶�Ӧλ�����ҷ������ӣ��ܷ��м�1��
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
    char opponent = -1*player;	//�Է�����
	char myplayer = player;

	//����λ�������Ĺ�ֵ------------------------------------------------------
    for(int row = 0; row < SIZE; row++){
        for(int col = 0; col < SIZE; col++){
            //�����̶�Ӧλ���Ƕ����µ����ӣ����ܷ��м�1
			CharaEval -= weight[row][col]*(chessboard[row][col] == opponent);
			opNum += (chessboard[row][col] == opponent);
			//�����̶�Ӧλ�����ҷ������ӣ��ܷ��м�1��
            CharaEval += weight[row][col]*(chessboard[row][col] == myplayer);
            myNum += (chessboard[row][col] == myplayer);
        }
    }

    //���ںڰ����������Ĺ�ֵ--------------------------------------------------
    if(myNum > opNum)
        BWRateEval = 100.0 * myNum/ (myNum + opNum);
    else if(myNum < opNum)
        BWRateEval = -100.0* opNum/ (myNum + opNum);
    else
        BWRateEval = 0;

    //���ڽ�λ���ӵĹ�ֵ------------------------------------------------------
    int Corner[4][2] = {{0,0},{0,7},{7,0},{7,7}};
    for(int i = 0; i < 4; i++){
        CornerEval += (chessboard[Corner[i][0]][Corner[i][1]] == myplayer);
        CornerEval -= (chessboard[Corner[i][0]][Corner[i][1]] == opponent);
    }
    CornerEval = 25*CornerEval;

    //���ڿ�����λ��Σ��λ�õĹ�ֵ��*λ��+λ��--------------------------------
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

    //���ڱ߽��ȶ��ӵĹ�ֵ----------------------------------------------------
    SideEval = 2.5*countSideEval(chessboard, myplayer);

    //�����ڲ��ȶ��ӵĹ�ֵ----------------------------------------------------
    for(int i = 1; i < 7; i++){
        for(int j = 1; j < 7; j++){
            if(isStable(chessboard, i, j)){
                StableEval += (chessboard[i][j] == myplayer);
                StableEval -= (chessboard[i][j] == opponent);
            }
        }
    }
    StableEval = 12.5*StableEval;

    //�����ж����Ĺ�ֵ--------------------------------------------------------
    if(!opMov)//���Է����ӿ���
        MobEval = 150;
    else if(!myMov)
        MobEval = -450;
    else if(myMov > opMov)
        MobEval = (100.0 * myMov)/(myMov + opMov);
    else if(myMov < opMov)
        MobEval = (-100.0* opMov)/(myMov + opMov);
    else
        MobEval = 0;

    //���ںڰױ߽�Ĺ�ֵ------------------------------------------------------
    int myFront = 0;
    int opFront = 0;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(chessboard[i][j] != 0) //������ǿ�λ��������
                continue;
            for(int k = 0; k < 8; k++){ //��ÿһ����λ��������Χ����
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


//��������
void copyChessBoard(char newChessBoard[][SIZE], char oldChessBoard[][SIZE]){
    for(int row = 0; row < SIZE; row++)
        for(int col = 0; col < SIZE; col++)
            newChessBoard[row][col] = oldChessBoard[row][col];
}

//alpha-beita��֦�㷨�����������������
int alphaBeita(char chessboard[][SIZE], int isDown[][SIZE], char player, int depth, int alpha, int beta, int AI)
{
    //�����µ�chessboard��isDown
    char newchessboard[SIZE][SIZE];
    int newisDown[SIZE][SIZE];
    int AIMov, opMov;

    AIMov = Check(chessboard, newisDown, AI);
    opMov = Check(chessboard, newisDown, -1*AI);

    //����������� or ���������ȣ�������������ֵ��ͬʱ����newisDownȨֵ
    if(depth == maxDepth || !Check(chessboard, newisDown, player)){
        return GetMaxScore(chessboard, AI, AIMov, opMov);
    }

    if(player == AI){ //���AI����,��������ֵ
        for(int row = 0; row < SIZE; row++){ // ������������
            for(int col = 0; col < SIZE; col++){
                if(!newisDown[row][col]) // ����˴���������
                    continue;

                copyChessBoard(newchessboard, chessboard);//��������
                PlayStep(newchessboard, row, col, player);//�ڸ��Ƶ�����������
                alpha = max(alpha, alphaBeita(newchessboard, newisDown, -1*AI, depth+1, alpha, beta, AI));
                if(alpha >= beta) //alpha-beta��֦
                    return alpha;
            }
        }
        return alpha;
    }
    else{//�����������
        for(int row = 0; row < SIZE; row++){//������������
            for(int col = 0; col < SIZE; col++){
                if(!newisDown[row][col]) // ����˴���������
                    continue;

                copyChessBoard(newchessboard, chessboard);//��������
                PlayStep(newchessboard, row, col, player);//�ڸ��Ƶ�����������
                beta = min(beta, alphaBeita(newchessboard, newisDown, AI, depth+1, alpha, beta, AI));
                if(alpha >= beta)//alpha-beta��֦
                    return beta;
            }
        }
        return beta;
    }
}

//AI��ʼ�Զ����ӣ�������е�һ�����, player����AI, turnID����غ���
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
            copyChessBoard(newchessboard, chessboard);//��������
            PlayStep(newchessboard, row, col, AI);//�ڸ��Ƶ�����������
            //�õ��ӽڵ��alphaֵ
            nodealpha = alphaBeita(newchessboard, newisDown, -1*AI, 1, alpha, beta, AI);
            if(alpha < nodealpha){ //alpha-beta��֦�㷨
                copyChessBoard(bestchessboard, newchessboard);
                alpha = nodealpha;
                bestrow = row;
                bestcol = col;
            }
        }
    }
    copyChessBoard(chessboard, bestchessboard); //�����̸���Ϊ��õ����ӵ�
    cout<<"Depth:"<<maxDepth<<"  Ԥ��alpha:"<<alpha<<endl;
    cout<<"AI find: "<<bestrow+1<<" "<<bestcol+1<<endl;
    cout<<"��ǰAI����ֵ��"<<GetMaxScore(chessboard, AI, myMov, opMov);
}
//����߽��ȶ��ӹ�ֵ
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

//�ж��ڲ��ȶ���
bool isStable(char chessboard[][SIZE], int x, int y){
    for(int i = 0; i < 8; i++){
        for(int nx = x + dir[i][0], ny = y + dir[i][1];
        inRange(nx,ny); nx += dir[i][0], ny += dir[i][1])
            if(chessboard[nx][ny] == 0)
                return false;
    }
    return true;
}

//����EvalȨ��
void setFactor(int turnID){
    if (turnID > 23) //���ݻغ�����������
    {
        BWFACTOR = 0.20; //�վ�ʱ���ߺڰ��ӱ�������
        DCFACTOR = 0.60; //����Σ�ս��Ӳ���
        CNFACTOR = 8.00;
        SDFACTOR = 6.00;
        STFACTOR = 4.00;
        FRFACTOR = 0.24; //���ͱ߽糤����
        MBFACTOR = 0.24; //�����ж�������
        CRFACTOR = 0.01; //����������ֵ����
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
        BWFACTOR = 0.04; //����ʱ�����Ӻڰױ���
        CNFACTOR = 7.50;
        DCFACTOR = 3.90;
        SDFACTOR = 0.00;
        STFACTOR = 0.00; //����������ȶ��ӵĹ�ֵ
        FRFACTOR = 1.27 - double(turnID) / 50; //�����ж�����ֵ
        MBFACTOR = 0.82 - double(turnID) / 60;
        CRFACTOR = 0.13; //����λ��������ֵ
    }
}
