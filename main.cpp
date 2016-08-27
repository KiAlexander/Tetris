
#include <conio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <fstream>
#include <mmsystem.h>
#include "colorConsole.h"     
#pragma comment(lib,"winmm.lib")

//�����
#define KEY_UP      72
#define KEY_DOWN    80
#define KEY_LEFT    75
#define KEY_RIGHT   77
#define KEY_ESC     27

#define IKEY_UP      'w'
#define IKEY_DOWN    's'
#define IKEY_LEFT    'a'
#define IKEY_RIGHT   'd'

#define KEY_RETURN  13 //�س���
#define KEY_PAUSE  32  //�ո��

#define MAPW    12     //��ͼ�Ŀ��
#define MAPH    20     //��ͼ�ĸ߶�

//��ʼ������
void Init();			
//����ת��
void Turn(int a[][4], int w, int h, int *x, int y, int MAP[][MAPW]);   
//�ж��Ƿ��ܷ���
bool IsAvailable(int a[], int x, int y, int w, int h, int MAP[][MAPW]); 
//���Ʒ���
void DrawBlocks(int a[], int w, int h, int x, int y, WORD wColors[], int nColors, int p, int q);
//�������
void ClearSquare(int *a, int w, int h, int x, int y, int p, int q);
//��ӡ��Ϸ����
void GameOver();
//����һ��
void DeleteLine(int m[][MAPW], int row, int p, int q);  
//����һ��
void AddLine(int m[][MAPW], int row, int p, int q);  
//��ȡ��ʷ��߷�
int GetMaxScore();
//��������
void ClearBlank(int i=0);
//����ģʽ
void DoubleMode();
//����ģʽ
void HeheMode();
//����or�˳�
void Change();
//ģʽѡ��
void ModeAlter();
//��Ϸ˵��
void GameRule();
//�Ѷ�ѡ��
void LevelChoose();
//�������߳�
DWORD WINAPI ThreadFunc0(LPVOID lpParam);
//�ҹ������߳�
DWORD WINAPI ThreadFunc(LPVOID lpParam);
//void MainThread();

HANDLE handle;
int MaxSCORE;//��ʷ��߷�
//7�ֲ�ͬ��״�ķ���
int b[7][4][4] = { { { 1 },{ 1,1,1 } },
                   { { 0,2 },{ 2,2,2 } },
                   { { 3,3 },{ 0,3,3 } },
                   { { 0,0,4 },{ 4,4,4 } },
                   { { 0,5,5 },{ 5,5 } },
                   { { 6,6,6,6 } },
                   { { 7,7 },{ 7,7 } }
                  };
WORD SQUARE_COLOR[7] = { FOREGROUND_RED | FOREGROUND_INTENSITY,
FOREGROUND_GREEN | FOREGROUND_INTENSITY,
FOREGROUND_BLUE | FOREGROUND_INTENSITY,
FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
};
int map[MAPH][MAPW] = { 0 }; //���湤����������1
int map1[MAPH][MAPW] = { 0 }; //���湤����������2

int dx = 12, //��ʼ����Ļʱ��ʼ����
	dy = 3,
	dm = 44,
	dn = 3;
int mark1 = 1, //addline
    mark2 = 1;
bool flag1 = false, //�����̵߳������ж�
     flag2 = false;
bool Hehe = false; //����ģʽ
int sign = 1;//Turn
int over = 0;
int score1,//����
    score2;
bool ok1 = false,//�߳̽������ж�
     ok2 = false;
int T;//ʱ���Ѷ�

DWORD dwThreadId;
DWORD dwThreadId1;
HANDLE hThread0;
HANDLE hThread;
CRITICAL_SECTION cs;//�ٽ���

int main()
{
	mciSendString("play Fade.mp3 repeat", 0, 0, 0);
	handle = initiate();
	//textout(handle, 10, 3, SQUARE_COLOR, 1, "�����q�����r�����ש��ש������������r���������r��������������");
	//textout(handle, 10, 4, SQUARE_COLOR, 1, "���s�����������������������ǩǩϡ��������ש��������ǩ��ǩ���");
	//textout(handle, 10, 5, SQUARE_COLOR, 1, "�������ǩǩ������ߩ��ߨs���ǩϩ����������ǩ�����������������");
	//textout(handle, 10, 6, SQUARE_COLOR, 1, "�����q�ǩ����ǩ������������ǩϩǩש��q�����������������ǩ���");
	//textout(handle, 10, 7, SQUARE_COLOR, 1, "���������Ǩs�s�������r�����ߩߩϩ���������������������������");
	//textout(handle, 10, 8, SQUARE_COLOR, 1, "�t�����s�t�������������s���s�������s�����s�����s���ߩ��s���s");
	textout(handle, 27, 10, SQUARE_COLOR, 1, "��ӭ�������˹���������");
	Sleep(1000);
	textout(handle, 27, 10, SQUARE_COLOR, 1, "                        ");
	
	ModeAlter();

	return EXIT_SUCCESS;
}

void Init()
{
	ClearBlank();//�����Ļ��ԭ�е�ͼ��

	memset(map, NULL, sizeof(map));//�����ҹ���������
	memset(map1, NULL, sizeof(map));
	mark1 = 1;//Deleteline�����ο�ֵ��ʼ��
	mark2 = 1;

	for (int i = 0;i<20;i++)//����ұ߽�
	{
		map[i][0] = -2;
		map1[i][0] = -2;
		map[i][11] = -2;
		map1[i][11] = -2;
	}
	for (int i = 0;i<12;i++)//������±߽�
	{
		map[0][i] = -1;
		map1[0][i] = -1;
		map[19][i] = -1;
		map1[19][i] = -1;
	}
	map[0][0] = -3;//����ĸ���
	map[0][11] = -3;
	map[19][0] = -3;
	map[19][11] = -3;
	map1[0][0] = -3;
	map1[0][11] = -3;
	map1[19][0] = -3;
	map1[19][11] = -3;

	WORD wColors[1] = { FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY };
	textout(handle, 60, 1, wColors, 1, "MaxSCORE:");//�ں���λ�ô�ӡ��߷�
	char string0[5];
	MaxSCORE = GetMaxScore();//��ȡ��߷�
	textout(handle, 72, 1, wColors, 1, itoa(MaxSCORE, string0, 10));

	//�ں���λ���ϴ�ӡ��Ϸ��ʾ
	textout(handle, dx - 8, 3 + dy, wColors, 1, "SCORE");
	textout(handle, dx - 8, 7 + dy, wColors, 1, "LEVEL");
	textout(handle, dx - 8, 11 + dy, wColors, 1, "NEXT");
	textout(handle, dm + 27, 3 + dn, wColors, 1, "SCORE");
	textout(handle, dm + 27, 7 + dn, wColors, 1, "LEVEL");
	textout(handle, dm + 27, 11 + dn, wColors, 1, "NEXT");

	//���Ƴ�ʼ����
	wColors[0] = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	DrawBlocks(&map[0][0], 12, 20, 0, 0, wColors, 1, dx, dy);
	DrawBlocks(&map1[0][0], 12, 20, 0, 0, wColors, 1, dm, dn);

	textout(handle, dx, dy, wColors, 1, "��T�T�T�T�T�T�T�T�T�T��");
	textout(handle, dm, dn, wColors, 1, "��T�T�T�T�T�T�T�T�T�T��");

		textout(handle, 30, 1, wColors, 1, "                     ");
		wColors[0] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		textout(handle, 26, 1, wColors, 1, "                    ");
		textout(handle, 36, 1, wColors, 1, "���������ʼ");

		int ch = _getch();//�������������������
		textout(handle, 36, 1, wColors, 1, "            ");
		textout(handle, 26, 1, wColors, 1, "˫�˶���˹����С��Ϸ");
}

bool IsAvailable(int a[], int x, int y, int w, int h, int MAP[][MAPW])
{
	for (int i = y;i<y + h;i++)
		for (int j = x;j < x + w;j++)
			if (MAP[i][j] && a[w*(i - y) + j - x])
				return 0;

	return 1;
}

void DrawBlocks(int a[], int w, int h, int x, int y, WORD wColors[], int nColors, int p, int q)
{
	int temp;

	for (int i = 0;i<h;i++)
		for (int j = 0;j<w;j++)
			if ((temp = a[i*w + j]) && y + i>0)
			{
				if (temp == -3)
					textout(handle, 2 * (x + j) + p, y + i + q, wColors, nColors, "��");
				else if (temp == -2)
					textout(handle, 2 * (x + j) + p, y + i + q, wColors, nColors, "�U");
				else if (temp == -1)
					textout(handle, 2 * (x + j) + p, y + i + q, wColors, nColors, "�T");
				else if (temp >= 1)
					textout(handle, 2 * (x + j) + p, y + i + q, wColors, nColors, "��");
			
			}
}

void Turn(int a[][4], int w, int h, int *x, int y, int MAP[][MAPW])
{
	int b[4][4] = { { 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 } };
	int sign = 0, line = 0;
	for (int i = h - 1;i >= 0;i--)
	{
		for (int j = 0;j<w;j++)
			if (a[i][j])
			{
				b[j][line] = a[i][j];
				sign = 1;
			}
		if (sign)
		{
			line++;
			sign = 0;
		}
	}
	for (int i = 0;i<4;i++)
		if (IsAvailable(b[0], *x - i, y, w, h, MAP))
		{
			*x -= i;
			for (int k = 0;k<h;k++)
				for (int j = 0;j<w;j++)
					a[k][j] = b[k][j];
			break;
		}
}

void ClearSquare(int *a, int w, int h, int x, int y, int p, int q)
{
	WORD wColors[1] = { 0 };
	for (int i = 0;i<h;i++)
		for (int j = 0;j<w;j++)
		{
			if (a[i*w + j]>0 && (i + y>0))
			{
				textout(handle, 2 * (x + j) + p, y + i + q, wColors, 1, "  ");
			}
		}

}

void GameOver()
{
	ClearBlank();//�����һ��Ļ����
	WORD wColors[1] = { FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY};
	textout(handle, 10, 5, wColors, 1, " ������        ��         ��    ��  ��������");
	textout(handle, 10, 6, wColors, 1, "��     ��     ����       ����  ���� ��      ");
	textout(handle, 10, 7, wColors, 1, "��           ��  ��     �� �� �� �� ��������");
	textout(handle, 10, 8, wColors, 1, "��     ��   ��������   ��  ����  �� ��      ");
	textout(handle, 10, 9, wColors, 1, " ������ �� ��      �� ��   ��    �� ��������");

	textout(handle, 16, 14, wColors, 1, "  ������   ��      ��  ��������  ��������  ");
	textout(handle, 16, 15, wColors, 1, " ��    ��   ��    ��   ��        ��     �� ");
	textout(handle, 16, 16, wColors, 1, "��      ��   ��  ��    ��������  ��������  ");
	textout(handle, 16, 17, wColors, 1, " ��    ��     ����     ��        �� ��     ");
	textout(handle, 16, 18, wColors, 1, "  ������       ��      ��������  ��  ������");
}

void DeleteLine(int m[][MAPW], int row, int p, int q)
{
	WORD wColors[1] = { FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY };
	textout(handle, 2 + p, row + q, wColors, 1, "�k�k�k�k�k�k�k�k�k�k");
	Sleep(100);

	for (int i = row;i>1;i--)
	{
		ClearSquare(&m[i][1], MAPW - 2, 1, 1, i, p, q);
		for (int j = 1;j<MAPW - 1;j++)
		{
			m[i][j] = m[i - 1][j];
			if (m[i][j] == 0)
				wColors[0] = 0;
			else
				wColors[0] = SQUARE_COLOR[m[i][j] - 1];
			DrawBlocks(&m[i][j], 1, 1, j, i, wColors, 1, p, q);
		}
	}
	for (int i = 1;i<MAPW - 1;i++)
		m[1][i] = 0;
}

void AddLine(int m[][MAPW], int row, int p, int q)
{

	WORD wColors[1] = { FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY };

	if (dx == p)//��Ϊ��������Ա��1���£�ʹ�¼����޷�����ȥ
	{
		mark1++;
	   /* maxRow = MAPH - mark1;*/
	}
	else if (dm == p)//��Ϊ�ҹ�������Ա��1���£�ʹ�¼����޷�����ȥ
	{
		mark2++;
	   /* maxRow = MAPH - mark2;*/
	}

	/*int isEmpty = 0;
	int Begin = 1;
	for (int i = 2;i < MAPH - 2;i++)
	{
		for (int j = 1;j < MAPW - 1;j++)
		{
			isEmpty += m[i][j];
		}
		if (0 != isEmpty)
		{
			Begin = i-1;
			break;
		}
		isEmpty = 0;
	}*/

	for (int i = 1;i< MAPH - 2;i++)//�ӹ��������һ�����¸���
	{
		for (int j = 1;j<MAPW - 1;j++)//������
		{
			m[i][j] = m[i + 1][j];
			/*if ((i == (maxRow - 1)) && (m[i][j] == 0))*/
			if (m[i][j] == 0)//���������ڸ�λ��Ϊ0������ո�λ��
			{
				wColors[0] = 0;
				textout(handle, 2 * j + p, i + q, wColors, 1, "  ");
			}
			else
			{
				wColors[0] = SQUARE_COLOR[m[i][j] - 1];//ȷ�����ƺ����ɫһ��
				/*textout(handle, 2 * j + p, i + q, wColors, 1, "��");*/
			}
	
			/*if ((i== (maxRow - 1))&&(m[i][j] == 0))
			{
				textout(handle, 2 * j + p, i+ q, wColors, 1, "  ");
			}
			else*/
			  //�������Ƶķ���
			   DrawBlocks(&m[i][j], 1, 1, j, i, wColors, 1, p, q);//
		}
	}
	//�����������һ��
	for (int i = 1;i < MAPW;i++)
		m[MAPH - 2][i] = 5;
	wColors[0] = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	DrawBlocks(m[MAPH-2], 11, 1, 0, MAPH-2, wColors, 1, p, q);

}

int  GetMaxScore()
{
	int buffer = 0;//�ļ���ȡֵ�Ļ�����
	int s = 0;//��Ҫд���ı���ֵ
	ifstream MyMaxScore("MaxScore.txt");//��ȡ��¼�ı�
	/*if (!MyMaxScore)
	{
	cout << "Unable to open MaxScore";
	exit(1);
	}*/
	buffer = (int)MyMaxScore.get();//�õ��ı���ֵ
	if (buffer < 0)//�״������ı�ʱ�õ�ֵΪ-1
		buffer = 0;
	//�Ƚ�������ҵ�ǰ��Ϸ������ȡ�ϸ�ֵ
	s = score1 > score2 ? score1 : score2;
	//�ȽϽϸ�ֵ����ʷ��߷֣�ȡ�ϸ�ֵ
	s = s > buffer ? s : buffer;
	//���¼�¼�ı�
	ofstream outMacSocre("MaxScore.txt");
	outMacSocre << (char)s << endl;
	return buffer;

}

void ClearBlank(int i)
{
	switch (i)//���ݲ����ж��������Χ
	{
	case 0://���������Ļ
	{
		WORD wColors[1] = { FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY };
		for (int i = 0;i<80;i++)
			for (int j = 0;j<MAPH + 4;j++)
				textout(handle, i, j, wColors, 1, "  ");
	}break;
	case 1://�����߷����ڷ���
	{
		for (int i = dx + 2;i<2 * MAPW + dx - 4;i++)
			for (int j = dy + 1;j<MAPH + dy - 1;j++)
				textout(handle, i, j, SQUARE_COLOR, 1, "  ");
	
	}break;
	case 2://����ұ߷����ڷ���
	{
		for (int i = dm + 2;i<2 * MAPW + dm - 4;i++)
			for (int j = dn + 1;j<MAPH + dn - 1;j++)
				textout(handle, i, j, SQUARE_COLOR, 1, "  ");
	}break;
	}

}

void DoubleMode()
{
	LevelChoose();//��Ϸ�Ѷ�ѡ��

	//�߳̽���������ǣ���Ϊtrue��ֱ���������߳�
	flag1 = false;
	flag2 = false;
	//�߳��Ѿ�������־����Ϊtrue�������Ӧ�߳��ѽ���
	ok1 = false;
	ok2 = false;

	Init();//��ʼ������
	
	InitializeCriticalSection(&cs);//��ʼ���ٽ�������
	hThread0 = CreateThread(NULL, 0, ThreadFunc0, NULL, 0, &dwThreadId1);//�����߳�0����Ӧ���
	hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &dwThreadId);//�����߳�1����Ӧ�ҿ�
	
	while (1)//�������̶߳��ѽ���ʱ������ѭ��
	{

	   if(ok1&&ok2)
		  break;
	}

	//�رն�Ӧ���ں˶���
	CloseHandle(hThread0);
	CloseHandle(hThread);

	Sleep(50);

	ModeAlter();//��ת��ģʽѡ�����
}

void Change()
{
	WORD wColors[1] = { FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY };
	textout(handle, 30, 10, wColors, 1, "��ȷ��������һ��?");
	textout(handle, 23, 12, wColors, 1, "y:�Ǿ�����һ��     n:���ܾ̾��˳�");
	while (1)
	{
		int CH = _getch();
		switch (CH)
		{
		case 'y':
		{
			textout(handle, 30, 10, wColors, 1, "                 ");
			textout(handle, 23, 12, wColors, 1, "                                 ");
			return;
		}break;
		case 'n':
		{
			textout(handle, 30, 10, wColors, 1, "                 ");
			textout(handle, 23, 12, wColors, 1, "                                 ");
			MaxSCORE = GetMaxScore();
			ClearBlank();
			GameOver();
			exit(EXIT_SUCCESS);
			return;
		}break;
    	}
	}
	

}

void Gamerule()
{
	ClearBlank();//�����һ����
	WORD wColors[1] = { FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY };
	textout(handle, 1,  2, wColors, 1, "                           ��Ϸ˵��                                           ");
	textout(handle, 1,  4, wColors, 1, "����Ϸ��������ģʽ�����ѡ��1������ģʽ  2������ģʽ"                        );
	textout(handle, 1,  6, wColors, 1, "����ģʽ�����������棬����ģʽ���ǲ��������棬����ģʽ����˫�˶�ս���������£�");
	textout(handle, 1,  8, wColors, 1, "      ������  ��ת�� W��             ������  ��ת�� ����                  ");
	textout(handle, 1, 10, wColors, 1, "                ���ƣ� A��                       ���ƣ� ����                  ");
	textout(handle, 1, 12, wColors, 1, "                ���ƣ� D��                       ���ƣ� ����                   ");
	textout(handle, 1, 14, wColors, 1, "                �½��� S��                       �½��� ����                   ");
	textout(handle, 1, 16, wColors, 1, "         �ո������ͣ��ʼ��Esc�����˳���Ϸ���س���������ѡ����档            ");
	textout(handle, 1, 18, wColors, 1, "         ף�������Ļ����Ϸ��죡                                             ");
	textout(handle, 1, 22, wColors, 1, "                                                                 ���أ��س���");
	while (1)//����ȡ��Ϊ�س���������ѭ��
	{
		int CH = _getch();
		if (CH == KEY_RETURN)
			break;
	}
	ModeAlter();//��ת��ģʽѡ�����
}

void LevelChoose()
{
	ClearBlank();//�����һ����

	WORD wColors[1] = { FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY };
	textout(handle, 27, 10, wColors, 1, "��ѡ��ǰ��Ϸ�Ѷȣ�0-9");
	textout(handle, 25, 14, wColors, 1, " 0  1  2  3  4  5  6  7  8  9");

	while (1)//ֻ����Ӧ���ּ��в���
	{
		int CH = _getch();//��ȡ��
		switch (CH)
		{
		case '0':
		{
			//TΪ�߳�������ӳ�ʱ����� max_delay = 10*T - T * level; 
			T = 10;
			ClearBlank();//��ձ�����
			return;//�˳���Ϸ�Ѷ�ѡ��
		}
		break;

		case '1':
		{
			T = 9;
			ClearBlank();
			return;
		};
		break;

		case '2':
		{
			T = 8;
			ClearBlank();
			return;
		};
		break;

		case '3':
		{
			T = 7;
			ClearBlank();
			return;
		};
		break;

		case '4':
		{
			T = 6;
			ClearBlank();
			return;
		};
		break;

		case '5':
		{
			T = 5;
			ClearBlank();
			return;
		};
		break;

		case '6':
		{
			T = 4;
			ClearBlank();
			return;
		};
		break;

		case '7':
		{
			T = 3;
			ClearBlank();
			return;
		};
		break;

		case '8':
		{
			T = 2;
			ClearBlank();
			return;
		};
		break;

		case '9':
		{
			T = 1;
			ClearBlank();
			return;
		};
		break;

		}
	   
	}

	
}

void ModeAlter()
{
	ClearBlank();//�����һ��������
	
	textout(handle, 32, 6, SQUARE_COLOR, 1, "��ѡ��ģʽ��1/2/3");
	textout(handle, 32, 10, SQUARE_COLOR, 1, "1������ģʽ");
	textout(handle, 32, 13, SQUARE_COLOR, 1, "2������ģʽ");
	textout(handle, 32, 16, SQUARE_COLOR, 1, "3����Ϸ˵��");
	textout(handle, 32, 19, SQUARE_COLOR, 1, "4���˳���Ϸ");
	
	while (1)//ֻ������ض���ִ�в���
	{
		int CH = _getch();//��ȡ���ѡ���İ���
		switch (CH)
		{
		case '1'://����ģʽ
		{
			textout(handle, 32, 6, SQUARE_COLOR, 1, "                 ");
			textout(handle, 32, 10, SQUARE_COLOR, 1, "           ");
			textout(handle, 32, 13, SQUARE_COLOR, 1, "           ");
			textout(handle, 32, 16, SQUARE_COLOR, 1, "           ");
			DoubleMode();//��������ģʽ����
		}
		break;
		case '2'://����ģʽ
		{
			textout(handle, 32, 6, SQUARE_COLOR, 1, "                 ");
			textout(handle, 32, 10, SQUARE_COLOR, 1, "           ");
			textout(handle, 32, 13, SQUARE_COLOR, 1, "           ");
			textout(handle, 32, 16, SQUARE_COLOR, 1, "           ");
			HeheMode();//���õ���ģʽ����
		};
		break;
		case '3'://��Ϸ˵��
		{
			Gamerule();
		};
		break;
		case '4'://�˳���Ϸ
		{
			GameOver();
			exit(EXIT_SUCCESS);
		};
		break;
		}
	}

}

//�ı������ַ�������ʼ����λ��
void HeheMode()
{
	mciSendString("stop Fade.mp3 ", 0, 0, 0);//�ر�ԭ������
	mciSendString("play ��÷.mp3 repeat", 0, 0, 0);//���Ŵ�ģʽ��������

	LevelChoose();//��Ϸ�Ѷ�ѡ��

	//�߳̽���������ǣ���Ϊtrue��ֱ���������߳�
	flag1 = false;
	flag2 = false;
	//�߳��Ѿ�������־����Ϊtrue�������Ӧ�߳��ѽ���
	ok1 = false;
	ok2 = false;
	//��ģʽ��ǣ���Ϊtrue��������������ģʽ
	Hehe = true;

	Init();//��ʼ������

	InitializeCriticalSection(&cs);//��ʼ���ٽ�������
	hThread0 = CreateThread(NULL, 0, ThreadFunc0, NULL, 0, &dwThreadId1);//�����߳�0����Ӧ���
	hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &dwThreadId);//�����߳�1����Ӧ�ҿ�
	
	while (1)//�������̶߳��ѽ���ʱ������ѭ��
	{
		if (ok1&&ok2)
			break;
	}

	//�رն�Ӧ���ں˶���
	CloseHandle(hThread0);
	CloseHandle(hThread);

	Sleep(50);
	Hehe = false;//���ñ�ǣ������˳�����ģʽ
	mciSendString("stop ��÷.mp3 ", 0, 0, 0);//�رձ�ģʽ����
	mciSendString("play Fade.mp3 repeat", 0, 0, 0);//k������ѭ������
	ModeAlter();//��ת��ģʽѡ�����

}

DWORD WINAPI ThreadFunc(LPVOID lpParam)
{
	ClearBlank(2);//��տ��ڷ���
	score2 = 0;  //��ʼ������
	int level = 0;  //��ʼ����Ϸ����

	Sleep(16);

	srand(time(NULL)+1); //ʱ������+1

	int Num = rand() % 7; //������һ��������
	int nextNum2 = Num;    //������һ��������

	int blank;  //��¼ÿ��������ʼλ��
	int x = 0, y = 0;  //��¼��Ϸ��ʼ���������

	int a1[4][4] = { 0 }; //��ʱʹ���������浱ǰ����


	while (1)
	{
		
		for (int i = 0;i<4;i++)          //���Ʒ���
			for (int j = 0;j<4;j++)
				if (a1[i][j] = b[nextNum2][i][j])
					blank = i;

		y = 1 - blank;
		x = 4;

		if (Hehe)//�Ƿ��ǵ���ģʽ
		{
			x = rand() % 7 + 1;//�趨�����ʼ����λ��
		}

		//������һ������
		Num = nextNum2;
		ClearSquare(b[Num][0], 4, 4, 13, 13, dm, dn);
		nextNum2 = rand() % 7;
		WORD wColors[1] = { SQUARE_COLOR[nextNum2] };
		DrawBlocks(b[nextNum2][0], 4, 4, 13, 13, wColors, 1, dm, dn);

		wColors[0] = SQUARE_COLOR[Num];
		DrawBlocks(&a1[0][0], 4, 4, x, y, wColors, 1, dm, dn);

		//��ʾ������Ϣ
		char string[5];
		wColors[0] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		textout(handle, dm + 27, 5 + dy, wColors, 1, itoa(score2, string, 10));
		textout(handle, dm + 27, 9 + dy, wColors, 1, itoa(level, string, 10));

		int max_delay = 10*T - T * level; //���㲻ͬ��Ϸ���������ʱ����

		while (1)
		{
			int delay = 0; //�ӳ���
			while (delay<max_delay)
			{
				if (_kbhit())  //��if���ⰴס��ʹ���鿨ס
				{
					
					EnterCriticalSection(&cs);//�����ٽ���
					int key = _getch();
					LeaveCriticalSection(&cs);//�˳��ٽ���
					switch (key)
					{
					case KEY_PAUSE:   //��ͣ
					{
						WORD wColors[1] = { FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY };
						textout(handle, 2, 1, wColors, 1, "��ͣ������������");
						while (_getch() != KEY_PAUSE)
						{
						}
						textout(handle, 2, 1, wColors, 1, "                ");
					}
					break;
					case KEY_UP: //��ת
					{
						ClearSquare(&a1[0][0], 4, 4, x, y, dm, dn);
						Turn(a1, 4, 4, &x, y, map1);
						wColors[0] = SQUARE_COLOR[Num];
						DrawBlocks(&a1[0][0], 4, 4, x, y, wColors, 1, dm, dn);
					}
					break;
					case KEY_DOWN://�½�
						delay = max_delay;
						break;
					case KEY_LEFT: //����
					{
						if (IsAvailable(&a1[0][0], x - 1, y, 4, 4, map1))
						{
							ClearSquare(&a1[0][0], 4, 4, x, y, dm, dn);
							x--;
							wColors[0] = SQUARE_COLOR[Num];
							DrawBlocks(&a1[0][0], 4, 4, x, y, wColors, 1, dm, dn);
						}
					}
					break;
					case KEY_RIGHT://����
					{
						if (IsAvailable(&a1[0][0], x + 1, y, 4, 4, map1))
						{
							ClearSquare(&a1[0][0], 4, 4, x, y, dm, dn);
							x++;
							wColors[0] = SQUARE_COLOR[Num];
							DrawBlocks(&a1[0][0], 4, 4, x, y, wColors, 1, dm, dn);
						}
					}
					break;
					case KEY_RETURN:
					{
						MaxSCORE = GetMaxScore();
						flag1 = true;//���ý�����һ�̵߳�����
						ok2 = true; //��־���߳̽���
						return 0;
					}
					break;
					case KEY_ESC://�˳���Ϸ
					{
						MaxSCORE = GetMaxScore();
						GameOver();
						exit(EXIT_SUCCESS);

					}
					break;
				
					}

				}
				Sleep(8);delay++;

				wColors[0] = SQUARE_COLOR[Num];//�ӿ���·����ٶȣ���ֹ����Addline����ʱ��˸
				DrawBlocks(&a1[0][0], 4, 4, x, y, wColors, 1, dm, dn);

			}
			if (flag2)//������������߳�
			{
				ok2 = true;//��־���߳̽���
				return 0;
			}

			if (IsAvailable(&a1[0][0], x, y + 1, 4, 4, map1)) //�Ƿ�������
			{
				ClearSquare(&a1[0][0], 4, 4, x, y, dm, dn);
				y++;
				wColors[0] = SQUARE_COLOR[Num];
				DrawBlocks(&a1[0][0], 4, 4, x, y, wColors, 1, dm, dn);
			}
			else
			{
				if (y <= 1)//�ж���Ϸ�Ƿ����
				{
					flag1 = true;//���ý�����һ�̵߳�����
					wColors[0] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
					textout(handle, 50, 12, wColors, 1, "�㲻С������");
					textout(handle, 18, 12, wColors, 1, "�����ɵ�Ӯ��");
					Sleep(800);
					textout(handle, 50, 12, wColors, 1, "            ");
					textout(handle, 18, 12, wColors, 1, "            ");
					Change();
					ok2 = true; //��־���߳̽���
					return 0;
				}

				for (int i = 0;i<4;i++)     //���·��飬���¹�����
					for (int j = 0;j<4;j++)
						if (a1[i][j] && ((i + y)<MAPH - 1) && ((j + x)<MAPW - 1))
							map1[i + y][j + x] = a1[i][j];

				int full, k = 0;
				for (int i = y;i<min(y + 4, MAPH - mark2);i++)
				{
					full = 1;
					for (int j = 1;j<11;j++)
						if (!map1[i][j]) full = 0;
					if (full)   //����һ��
					{
						DeleteLine(map1, i, dm, dn);
						AddLine(map, i, dx, dy);//���к�����һ����������һ��
						k++;
						score2 = score2 + k;
						MaxSCORE = GetMaxScore();//����ǰ�÷ֳ�����߷֣���д��txt��¼
						level = min(score2 / 30, 9);
						max_delay = 10*T - T * level;
					}
				}
				break;
			}
		}
	}
	return 0;
}

DWORD WINAPI ThreadFunc0(LPVOID lpParam)
{
	ClearBlank(1);//��տ��ڷ���
	score1 = 0;  //��ʼ������
	int level = 0;  //��ʼ����Ϸ����

	Sleep(16);

	srand(time(NULL));//ʱ������

	int Num = rand() % 7; //������һ��������
	int nextNum1 = Num;    //������һ��������

	int blank;  //��¼ÿ��������ʼλ��
	int x = 0, y = 0;  //��¼��Ϸ��ʼ���������

	int a[4][4] = { 0 }; //��ʱʹ���������浱ǰ����


	while (1)
	{ 	
		for (int i = 0;i<4;i++)          //���Ʒ���
			for (int j = 0;j<4;j++)
				if (a[i][j] = b[nextNum1][i][j])
					blank = i;

		y = 1 - blank;
		x = 4;

		if (Hehe)//�Ƿ��ǵ���ģʽ
		{
			x = rand()%7+1;//�趨�����ʼ����λ��
		}

		//������һ������
		Num = nextNum1;
		ClearSquare(b[Num][0], 4, 4, 13, 13, dx - 36, dy);
		nextNum1 = rand() % 7;
		WORD wColors[1] = { SQUARE_COLOR[nextNum1] };
		DrawBlocks(b[nextNum1][0], 4, 4, 13, 13, wColors, 1, dx - 36, dy);

		wColors[0] = SQUARE_COLOR[Num];
		DrawBlocks(&a[0][0], 4, 4, x, y, wColors, 1, dx, dy);

		//��ʾ������Ϣ
		char string[5];
		wColors[0] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		textout(handle, dx - 8, 5 + dy, wColors, 1, itoa(score1, string, 10));
		textout(handle, dx - 8, 9 + dy, wColors, 1, itoa(level, string, 10));

		int max_delay = 10*T - T * level; //���㲻ͬ��Ϸ���������ʱ����

		while (1)
		{
			int delay = 0; //�ӳ���
			while (delay<max_delay)
			{
				if (_kbhit())  //��if���ⰴס��ʹ���鿨ס
				{
					
					EnterCriticalSection(&cs);//�����ٽ���
					int key = _getch();
					LeaveCriticalSection(&cs);//�˳��ٽ���
					switch (key)
					{
					case KEY_PAUSE://��ͣ
					{
						WORD wColors[1] = { FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY };
						textout(handle, 2, 1, wColors, 1, "��ͣ������������");
						while (_getch() != KEY_PAUSE)
						{
						}
						textout(handle, 2, 1, wColors, 1, "                ");
					}
					break;
					case IKEY_UP://��ת
					{
						ClearSquare(&a[0][0], 4, 4, x, y, dx, dy);
						Turn(a, 4, 4, &x, y, map);
						wColors[0] = SQUARE_COLOR[Num];
						DrawBlocks(&a[0][0], 4, 4, x, y, wColors, 1, dx, dy);
					}
					break;
					case IKEY_DOWN://�½�
						delay = max_delay;
						break;
					case IKEY_LEFT://����
					{
						if (IsAvailable(&a[0][0], x - 1, y, 4, 4, map))
						{
							ClearSquare(&a[0][0], 4, 4, x, y, dx, dy);
							x--;
							wColors[0] = SQUARE_COLOR[Num];
							DrawBlocks(&a[0][0], 4, 4, x, y, wColors, 1, dx, dy);
						}
					}
					break;
					case IKEY_RIGHT://����
					{
						if (IsAvailable(&a[0][0], x + 1, y, 4, 4, map))
						{
							ClearSquare(&a[0][0], 4, 4, x, y, dx, dy);
							x++;
							wColors[0] = SQUARE_COLOR[Num];
							DrawBlocks(&a[0][0], 4, 4, x, y, wColors, 1, dx, dy);
						}
					}
					break;
					case KEY_RETURN:
					{
						MaxSCORE = GetMaxScore();
						flag2 = true;//���ý�����һ�̵߳�����
						ok1 = true; //��־���߳̽���
						return 0;
					}
					case KEY_ESC://�˳���Ϸ
					{
						MaxSCORE = GetMaxScore();
						GameOver();
						exit(EXIT_SUCCESS);
					}
				 }

				}
				Sleep(8);delay++;

				wColors[0] = SQUARE_COLOR[Num];//�ӿ���·����ٶȣ���ֹ����Addline����ʱ��˸
				DrawBlocks(&a[0][0], 4, 4, x, y, wColors, 1, dx, dy);
			}
			if (flag1)//������������߳�
			{
				ok1 = true;//��־���߳̽���
				return 0;
			}
			if (IsAvailable(&a[0][0], x, y + 1, 4, 4, map)) //�Ƿ�������
			{
				ClearSquare(&a[0][0], 4, 4, x, y, dx, dy);
				y++;
				wColors[0] = SQUARE_COLOR[Num];
				DrawBlocks(&a[0][0], 4, 4, x, y, wColors, 1, dx, dy);
			}
			else
			{
				if (y <= 1)//�ж���Ϸ�Ƿ����
				{
					flag2 = true;//���ý�����һ�̵߳�����
					WORD wColors[1] = { FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY };
					textout(handle, 18, 12, wColors, 1, "�㲻С������");
					textout(handle, 50, 12, wColors, 1, "�����ɵ�Ӯ��");
					Sleep(800);
					textout(handle, 18, 12, wColors, 1, "            ");
					textout(handle, 50, 12, wColors, 1, "            ");
					Change();
					ok1 = true;//��־���߳̽���
					return 0;
				}
			
				for (int i = 0;i<4;i++)     //���·��飬���¹�����
					for (int j = 0;j<4;j++)
						if (a[i][j] && ((i + y)<MAPH - 1) && ((j + x)<MAPW - 1))
							map[i + y][j + x] = a[i][j];

				int full, k = 0;
				for (int i = y;i<min(y + 4, MAPH - mark1);i++)
				{
					full = 1;
					for (int j = 1;j<11;j++)
						if (!map[i][j]) full = 0;
					if (full)   //����һ��
					{
						DeleteLine(map, i, dx, dy);
						AddLine(map1, i, dm, dn);//���к�����һ����������һ��
						k++;
						score1 = score1 + k;
						MaxSCORE = GetMaxScore();//����ǰ�÷ֳ�����߷֣���д��txt��¼
						level = min(score1 / 30, 9);
						max_delay = 10*T - T * level;
					}
				}
				break;
			}
		}
	}
	return 0;
}

