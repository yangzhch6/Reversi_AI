#include <iostream>
#include <windows.h>
#include <stdio.h>
#include "head.cpp"

#define SIZE 8
#define max(a,b) a>b?a:b
#define min(a,b) a<b?a:b
using namespace std;
/*
//��ʾ���������ӵ�״̬
void Output(char chessboard[][SIZE]);

//���¿��ж�����ͬʱ�����ж�����Ŀ(��������Ŀ)
int Check(char chessboard[][SIZE], int isDown[][SIZE], char player);

//��ָ��λ������,�����м����Ӵ�����ҷ�����
void PlayStep(char chessboard[][SIZE], int row, int col, char player);

//��ȡ��ǰ��ַ���
int GetMaxScore(char chessboard[][SIZE], char player);

//��������
void copyChessBoard(char newChessBoard[][SIZE], char oldChessBoard[][SIZE]);

//alpha-beita��֦�㷨�����������������
int alphaBeita(char chessboard[][SIZE], int isDown[][SIZE], char player, int depth, int alpha, int beta);

//AI��ʼ�Զ����ӣ�������е�һ�����
void AIPlayStep(char chessboard[][SIZE], int isDown[][SIZE], int AI, int turnID);


�������ݽṹ:
    //���������и���Ԫ�����ӵ�״̬, -1Ϊ��(��Ϸ��)�� 1Ϊ��(AI)
	char chessboard[8][8];

�������̶�ĳһ��������λ�ã�
    //���������и�λ���Ƿ�������ӣ������ӵ�λ��Ϊ1������λ��Ϊ0
	int isDown[8][8] = { 0 };

��ǰ�غ����巽��
    //0��ʾ��Ϸ��-��,1��ʾAI-��
    int player = -1;	//���巽��-1ΪAI���֣�1ΪAI����
    ������һ��ѭ�������
    player = -1 ��ʾ��Ϸ����������
    player = 1  ��ʾAI��������

ע��
1.��������ʱΪÿһ������һ�����̾���
2.��Ϸ����ʵ����main��
3.���õ��Ǿ���λ����������
4.��Ϸ��Զ�Ǻ�������
*/
int main()
{
	//���������и���Ԫ�����ӵ�״̬
	char chessboard[SIZE][SIZE];
	//���������и�λ���Ƿ�������ӣ������ӵ�λ��Ϊ1������λ��Ϊ0
	int isDown[SIZE][SIZE] = { 0 };
	int row, col, x, y;
	//������������
	int iCount = 0;
	int player, AI;	//��ǰ���巽,AI��
	//�������ӵĴ�������Ϊ2����ʾ˫������������
	int SkipPlay = 0;

	//����AI����Ϸ�ߵĵ÷�
	int Score[2];
	char select;
	printf("�ڰ���\n\n");
	printf("��Ϸ��ִ�����£�AIִ�ڣ����س�����ʼ:\n");
	getchar();

    cout<<"��ѡ��AI����or���֣�����1(��)������-1(��): ";
    cin>>AI;

    //���õ�ǰ���巽���������¹�����Ϊ1
    player = 1;
    iCount = 4;	//�ۼ�������
    //���̸�λ�����
    for (row = 0; row < SIZE; row++)
        for (col = 0; col < SIZE; col++)
            chessboard[row][col] = 0;

    //�������м�λ�÷��ú���
    chessboard[SIZE/2 - 1][SIZE/2 - 1] = chessboard[SIZE/2][SIZE/2] = 1;
    //�������м�λ�÷��ð���
    chessboard[SIZE/2 - 1][SIZE/2] = chessboard[SIZE/2][SIZE/2 - 1] = -1;

    printf("\n���̳�ʼ״̬:\n");
    //��ʾ��ʼ�������ӵ�״��
    Output(chessboard, -1*AI);

    int turnID = 0;//�غ���
    do
    {
        //����AI����
        if (player != AI)
        {
            //�ж��Ƿ���°���
            if (Check(chessboard, isDown, player))
            {
                SkipPlay = 0;	//�����Ч���Ӵ���
                //��ѭ����ֱ���û�������ȷ������Ϊֹ
                while(1)
                {
                    printf("�������ӵ�λ��(�� ��):");
                    cin>>x>>y;
                    x--;	//����������λ��
                    y--;	//����������λ��
                    //����������������Ч

                    if (x >= 0 && y >= 0 && x < SIZE && y < SIZE && isDown[x][y])
                    {
                        //��ָ������λ������
                        PlayStep(chessboard, x, y, player);
                        iCount++;	//�ۼ�������
                        break;
                    }
                    else
                    {
                        fflush(stdin);	//������뻺����
                        printf("��������������������롣\n\n");
                    }
                }

                printf("\n�����Ӻ��״̬:\n");
                Output(chessboard, -1*AI);	//��ʾ����״̬
                printf("�������AI���ӡ�\n");
                getchar();getchar();
            }
            else if (++SkipPlay < 2)//����Ч���ӵĴ���С��2
            {
                printf("��ûλ�ÿ��£����س����öԷ����ӡ�");
                getchar();
            }else
                printf("˫����û�ط����ӣ���Ϸ����!\n");
        }
        //����AI����
        else
        {

            if (Check(chessboard, isDown, player))
            {
                SkipPlay = 0;	//�����Ч���Ӵ���
                AIPlayStep(chessboard, isDown, player, turnID);
                iCount++;	//�ۼ�������
                printf("\nAI���Ӻ��״̬:\n");
                Output(chessboard, -1*AI);	//��ʾ����״̬
            }
            else
            {
                //����Ч���Ӵ���С��2
                if (++SkipPlay < 2)
                    printf("��ûλ�ÿ��ߣ������ߡ�\n");
                else
                    printf("˫����û�ط����ӣ���Ϸ����!");
            }
        }
        player = -1*player;//�������巽
        turnID++;//�غ���++
    }while (iCount < SIZE*SIZE && SkipPlay < 2);//��������С��64 ����Ч���ӵĴ���С��2

    //��ʾ��˫�����ӵ�״��
    Output(chessboard, -1*AI);
    Score[0] = Score[1] = 0;//��ռƷֱ���
    //ѭ��ͳ�Ƹ���Ԫ��ڰ����ӵ�����
    for (row = 0; row < SIZE; row++)
    {
        for (col = 0; col < SIZE; col++)
        {
            //ͳ�ƺ�����
            Score[0] += chessboard[row][col] == 1;
            //ͳ�ư�����
            Score[1] += chessboard[row][col] == -1;
        }
    }
    printf("���ճɼ�:\n");
    printf("��Ϸ��:%d\n  AI:%d\n", Score[1], Score[0]);
    fflush(stdin);	//������뻺����

	printf("Game Over!\n");
	getchar();
	return 0;
}

