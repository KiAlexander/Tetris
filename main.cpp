
#include <conio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <fstream>
#include <mmsystem.h>
#include "colorConsole.h"     
#pragma comment(lib,"winmm.lib")

//方向键
#define KEY_UP      72
#define KEY_DOWN    80
#define KEY_LEFT    75
#define KEY_RIGHT   77
#define KEY_ESC     27

#define IKEY_UP      'w'
#define IKEY_DOWN    's'
#define IKEY_LEFT    'a'
#define IKEY_RIGHT   'd'

#define KEY_RETURN  13 //回车键
#define KEY_PAUSE  32  //空格键

#define MAPW    12     //地图的宽度
#define MAPH    20     //地图的高度

//初始化工作
void Init();			
//方块转动
void Turn(int a[][4], int w, int h, int *x, int y, int MAP[][MAPW]);   
//判定是否能放下
bool IsAvailable(int a[], int x, int y, int w, int h, int MAP[][MAPW]); 
//绘制方块
void DrawBlocks(int a[], int w, int h, int x, int y, WORD wColors[], int nColors, int p, int q);
//清除方块
void ClearSquare(int *a, int w, int h, int x, int y, int p, int q);
//打印游戏结束
void GameOver();
//消除一行
void DeleteLine(int m[][MAPW], int row, int p, int q);  
//增加一行
void AddLine(int m[][MAPW], int row, int p, int q);  
//获取历史最高分
int GetMaxScore();
//整体清屏
void ClearBlank(int i=0);
//正经模式
void DoubleMode();
//地狱模式
void HeheMode();
//继续or退出
void Change();
//模式选择
void ModeAlter();
//游戏说明
void GameRule();
//难度选择
void LevelChoose();
//左工作区线程
DWORD WINAPI ThreadFunc0(LPVOID lpParam);
//右工作区线程
DWORD WINAPI ThreadFunc(LPVOID lpParam);
//void MainThread();

HANDLE handle;
int MaxSCORE;//历史最高分
//7种不同形状的方块
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
int map[MAPH][MAPW] = { 0 }; //保存工作区的区域1
int map1[MAPH][MAPW] = { 0 }; //保存工作区的区域2

int dx = 12, //初始化屏幕时起始坐标
	dy = 3,
	dm = 44,
	dn = 3;
int mark1 = 1, //addline
    mark2 = 1;
bool flag1 = false, //结束线程的命令判定
     flag2 = false;
bool Hehe = false; //地狱模式
int sign = 1;//Turn
int over = 0;
int score1,//分数
    score2;
bool ok1 = false,//线程结束的判定
     ok2 = false;
int T;//时间难度

DWORD dwThreadId;
DWORD dwThreadId1;
HANDLE hThread0;
HANDLE hThread;
CRITICAL_SECTION cs;//临界区

int main()
{
	mciSendString("play Fade.mp3 repeat", 0, 0, 0);
	handle = initiate();
	//textout(handle, 10, 3, SQUARE_COLOR, 1, "　┓╭┓┓╮┏━┳━┳┓　┓┏┏━╮　　━━╮　　┓　┓　　");
	//textout(handle, 10, 4, SQUARE_COLOR, 1, "┏╯┛┃┃┛┃　┃　┃┃┏┣┣┫　　┏━┳━━┛┏┣┏┣━┓");
	//textout(handle, 10, 5, SQUARE_COLOR, 1, "┃┃━┣┣┛┗━┻━┻╯　┣┫┃　　　　┣━━┓　┃　┃　┃");
	//textout(handle, 10, 6, SQUARE_COLOR, 1, "┃┃╭┣┃┃┣━━━━┓　┣┫┣┳┛╭　┃　　┃　┃━┣━┻");
	//textout(handle, 10, 7, SQUARE_COLOR, 1, "　┃┛┃┣╯╯┗━━╮┃┗┻┻┫┃　┃　┃　　┃　┃　┃　　");
	//textout(handle, 10, 8, SQUARE_COLOR, 1, "╰┛━╯╰┛┗━━━━╯┗╯┗┛┗╯┗━╯　━╯┗┻┗╯━╯");
	textout(handle, 27, 10, SQUARE_COLOR, 1, "欢迎进入俄罗斯方块的世界");
	Sleep(1000);
	textout(handle, 27, 10, SQUARE_COLOR, 1, "                        ");
	
	ModeAlter();

	return EXIT_SUCCESS;
}

void Init()
{
	ClearBlank();//清除屏幕上原有的图案

	memset(map, NULL, sizeof(map));//将左右工作区赋零
	memset(map1, NULL, sizeof(map));
	mark1 = 1;//Deleteline函数参考值初始化
	mark2 = 1;

	for (int i = 0;i<20;i++)//标记右边界
	{
		map[i][0] = -2;
		map1[i][0] = -2;
		map[i][11] = -2;
		map1[i][11] = -2;
	}
	for (int i = 0;i<12;i++)//标记上下边界
	{
		map[0][i] = -1;
		map1[0][i] = -1;
		map[19][i] = -1;
		map1[19][i] = -1;
	}
	map[0][0] = -3;//标记四个角
	map[0][11] = -3;
	map[19][0] = -3;
	map[19][11] = -3;
	map1[0][0] = -3;
	map1[0][11] = -3;
	map1[19][0] = -3;
	map1[19][11] = -3;

	WORD wColors[1] = { FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY };
	textout(handle, 60, 1, wColors, 1, "MaxSCORE:");//在合适位置打印最高分
	char string0[5];
	MaxSCORE = GetMaxScore();//获取最高分
	textout(handle, 72, 1, wColors, 1, itoa(MaxSCORE, string0, 10));

	//在合适位置上打印游戏提示
	textout(handle, dx - 8, 3 + dy, wColors, 1, "SCORE");
	textout(handle, dx - 8, 7 + dy, wColors, 1, "LEVEL");
	textout(handle, dx - 8, 11 + dy, wColors, 1, "NEXT");
	textout(handle, dm + 27, 3 + dn, wColors, 1, "SCORE");
	textout(handle, dm + 27, 7 + dn, wColors, 1, "LEVEL");
	textout(handle, dm + 27, 11 + dn, wColors, 1, "NEXT");

	//绘制初始界面
	wColors[0] = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	DrawBlocks(&map[0][0], 12, 20, 0, 0, wColors, 1, dx, dy);
	DrawBlocks(&map1[0][0], 12, 20, 0, 0, wColors, 1, dm, dn);

	textout(handle, dx, dy, wColors, 1, "◇══════════◇");
	textout(handle, dm, dn, wColors, 1, "◇══════════◇");

		textout(handle, 30, 1, wColors, 1, "                     ");
		wColors[0] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		textout(handle, 26, 1, wColors, 1, "                    ");
		textout(handle, 36, 1, wColors, 1, "按任意键开始");

		int ch = _getch();//阻塞函数，按键后继续
		textout(handle, 36, 1, wColors, 1, "            ");
		textout(handle, 26, 1, wColors, 1, "双人俄罗斯方块小游戏");
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
					textout(handle, 2 * (x + j) + p, y + i + q, wColors, nColors, "◆");
				else if (temp == -2)
					textout(handle, 2 * (x + j) + p, y + i + q, wColors, nColors, "║");
				else if (temp == -1)
					textout(handle, 2 * (x + j) + p, y + i + q, wColors, nColors, "═");
				else if (temp >= 1)
					textout(handle, 2 * (x + j) + p, y + i + q, wColors, nColors, "■");
			
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
	ClearBlank();//清空上一屏幕界面
	WORD wColors[1] = { FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY};
	textout(handle, 10, 5, wColors, 1, " ■■■        ■         ■    ■  ■■■■");
	textout(handle, 10, 6, wColors, 1, "■     ■     ■■       ■■  ■■ ■      ");
	textout(handle, 10, 7, wColors, 1, "■           ■  ■     ■ ■ ■ ■ ■■■■");
	textout(handle, 10, 8, wColors, 1, "■     ■   ■■■■   ■  ■■  ■ ■      ");
	textout(handle, 10, 9, wColors, 1, " ■■■ ■ ■      ■ ■   ■    ■ ■■■■");

	textout(handle, 16, 14, wColors, 1, "  ■■■   ■      ■  ■■■■  ■■■■  ");
	textout(handle, 16, 15, wColors, 1, " ■    ■   ■    ■   ■        ■     ■ ");
	textout(handle, 16, 16, wColors, 1, "■      ■   ■  ■    ■■■■  ■■■■  ");
	textout(handle, 16, 17, wColors, 1, " ■    ■     ■■     ■        ■ ■     ");
	textout(handle, 16, 18, wColors, 1, "  ■■■       ■      ■■■■  ■  ■■■");
}

void DeleteLine(int m[][MAPW], int row, int p, int q)
{
	WORD wColors[1] = { FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY };
	textout(handle, 2 + p, row + q, wColors, 1, "﹌﹌﹌﹌﹌﹌﹌﹌﹌﹌");
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

	if (dx == p)//若为左工作区则对标记1更新，使新加行无法被消去
	{
		mark1++;
	   /* maxRow = MAPH - mark1;*/
	}
	else if (dm == p)//若为右工作区则对标记1更新，使新加行无法被消去
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

	for (int i = 1;i< MAPH - 2;i++)//从工作区最顶上一行向下更新
	{
		for (int j = 1;j<MAPW - 1;j++)//从左到右
		{
			m[i][j] = m[i + 1][j];
			/*if ((i == (maxRow - 1)) && (m[i][j] == 0))*/
			if (m[i][j] == 0)//若工作区内该位置为0，则清空该位置
			{
				wColors[0] = 0;
				textout(handle, 2 * j + p, i + q, wColors, 1, "  ");
			}
			else
			{
				wColors[0] = SQUARE_COLOR[m[i][j] - 1];//确保上移后的颜色一致
				/*textout(handle, 2 * j + p, i + q, wColors, 1, "■");*/
			}
	
			/*if ((i== (maxRow - 1))&&(m[i][j] == 0))
			{
				textout(handle, 2 * j + p, i+ q, wColors, 1, "  ");
			}
			else*/
			  //绘制上移的方块
			   DrawBlocks(&m[i][j], 1, 1, j, i, wColors, 1, p, q);//
		}
	}
	//在最底下增加一行
	for (int i = 1;i < MAPW;i++)
		m[MAPH - 2][i] = 5;
	wColors[0] = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	DrawBlocks(m[MAPH-2], 11, 1, 0, MAPH-2, wColors, 1, p, q);

}

int  GetMaxScore()
{
	int buffer = 0;//文件读取值的缓存量
	int s = 0;//将要写入文本的值
	ifstream MyMaxScore("MaxScore.txt");//读取记录文本
	/*if (!MyMaxScore)
	{
	cout << "Unable to open MaxScore";
	exit(1);
	}*/
	buffer = (int)MyMaxScore.get();//得到文本的值
	if (buffer < 0)//首次生成文本时得到值为-1
		buffer = 0;
	//比较两个玩家当前游戏分数，取较高值
	s = score1 > score2 ? score1 : score2;
	//比较较高值和历史最高分，取较高值
	s = s > buffer ? s : buffer;
	//更新记录文本
	ofstream outMacSocre("MaxScore.txt");
	outMacSocre << (char)s << endl;
	return buffer;

}

void ClearBlank(int i)
{
	switch (i)//根据参数判读清空区域范围
	{
	case 0://清空整个屏幕
	{
		WORD wColors[1] = { FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY };
		for (int i = 0;i<80;i++)
			for (int j = 0;j<MAPH + 4;j++)
				textout(handle, i, j, wColors, 1, "  ");
	}break;
	case 1://清空左边方框内方块
	{
		for (int i = dx + 2;i<2 * MAPW + dx - 4;i++)
			for (int j = dy + 1;j<MAPH + dy - 1;j++)
				textout(handle, i, j, SQUARE_COLOR, 1, "  ");
	
	}break;
	case 2://清空右边方框内方块
	{
		for (int i = dm + 2;i<2 * MAPW + dm - 4;i++)
			for (int j = dn + 1;j<MAPH + dn - 1;j++)
				textout(handle, i, j, SQUARE_COLOR, 1, "  ");
	}break;
	}

}

void DoubleMode()
{
	LevelChoose();//游戏难度选择

	//线程结束条件标记，若为true则分别结束各自线程
	flag1 = false;
	flag2 = false;
	//线程已经结束标志，若为true则表明对应线程已结束
	ok1 = false;
	ok2 = false;

	Init();//初始化界面
	
	InitializeCriticalSection(&cs);//初始化临界区变量
	hThread0 = CreateThread(NULL, 0, ThreadFunc0, NULL, 0, &dwThreadId1);//开启线程0，对应左框
	hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &dwThreadId);//开启线程1，对应右框
	
	while (1)//当两个线程都已结束时，跳出循环
	{

	   if(ok1&&ok2)
		  break;
	}

	//关闭对应的内核对象
	CloseHandle(hThread0);
	CloseHandle(hThread);

	Sleep(50);

	ModeAlter();//跳转到模式选择界面
}

void Change()
{
	WORD wColors[1] = { FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY };
	textout(handle, 30, 10, wColors, 1, "你确定不再玩一把?");
	textout(handle, 23, 12, wColors, 1, "y:那就再玩一把     n:残忍拒绝退出");
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
	ClearBlank();//清空上一界面
	WORD wColors[1] = { FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY };
	textout(handle, 1,  2, wColors, 1, "                           游戏说明                                           ");
	textout(handle, 1,  4, wColors, 1, "本游戏共有两种模式供玩家选择：1、正经模式  2、地狱模式"                        );
	textout(handle, 1,  6, wColors, 1, "正经模式就是正经地玩，地狱模式就是不正经地玩，两种模式均是双人对战，操作如下：");
	textout(handle, 1,  8, wColors, 1, "      左区域：  旋转： W键             右区域：  旋转： ↑键                  ");
	textout(handle, 1, 10, wColors, 1, "                左移： A键                       左移： ←键                  ");
	textout(handle, 1, 12, wColors, 1, "                右移： D键                       右移： →键                   ");
	textout(handle, 1, 14, wColors, 1, "                下降： S键                       下降： ↓键                   ");
	textout(handle, 1, 16, wColors, 1, "         空格键：暂停开始，Esc键：退出游戏，回车键：返回选择界面。            ");
	textout(handle, 1, 18, wColors, 1, "         祝您和您的伙伴游戏愉快！                                             ");
	textout(handle, 1, 22, wColors, 1, "                                                                 返回：回车键");
	while (1)//若读取键为回车键则跳出循环
	{
		int CH = _getch();
		if (CH == KEY_RETURN)
			break;
	}
	ModeAlter();//跳转到模式选择界面
}

void LevelChoose()
{
	ClearBlank();//清空上一界面

	WORD wColors[1] = { FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY };
	textout(handle, 27, 10, wColors, 1, "请选择当前游戏难度：0-9");
	textout(handle, 25, 14, wColors, 1, " 0  1  2  3  4  5  6  7  8  9");

	while (1)//只对相应数字键有操作
	{
		int CH = _getch();//读取键
		switch (CH)
		{
		case '0':
		{
			//T为线程中最大延迟时间参数 max_delay = 10*T - T * level; 
			T = 10;
			ClearBlank();//清空本界面
			return;//退出游戏难度选择
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
	ClearBlank();//清空上一界面内容
	
	textout(handle, 32, 6, SQUARE_COLOR, 1, "请选择模式：1/2/3");
	textout(handle, 32, 10, SQUARE_COLOR, 1, "1：正经模式");
	textout(handle, 32, 13, SQUARE_COLOR, 1, "2：地狱模式");
	textout(handle, 32, 16, SQUARE_COLOR, 1, "3：游戏说明");
	textout(handle, 32, 19, SQUARE_COLOR, 1, "4：退出游戏");
	
	while (1)//只允许对特定键执行操作
	{
		int CH = _getch();//获取玩家选定的按键
		switch (CH)
		{
		case '1'://正经模式
		{
			textout(handle, 32, 6, SQUARE_COLOR, 1, "                 ");
			textout(handle, 32, 10, SQUARE_COLOR, 1, "           ");
			textout(handle, 32, 13, SQUARE_COLOR, 1, "           ");
			textout(handle, 32, 16, SQUARE_COLOR, 1, "           ");
			DoubleMode();//调用正经模式函数
		}
		break;
		case '2'://地狱模式
		{
			textout(handle, 32, 6, SQUARE_COLOR, 1, "                 ");
			textout(handle, 32, 10, SQUARE_COLOR, 1, "           ");
			textout(handle, 32, 13, SQUARE_COLOR, 1, "           ");
			textout(handle, 32, 16, SQUARE_COLOR, 1, "           ");
			HeheMode();//调用地狱模式函数
		};
		break;
		case '3'://游戏说明
		{
			Gamerule();
		};
		break;
		case '4'://退出游戏
		{
			GameOver();
			exit(EXIT_SUCCESS);
		};
		break;
		}
	}

}

//改变了音乐风格，随机初始下落位置
void HeheMode()
{
	mciSendString("stop Fade.mp3 ", 0, 0, 0);//关闭原有音乐
	mciSendString("play 马东梅.mp3 repeat", 0, 0, 0);//播放此模式特有音乐

	LevelChoose();//游戏难度选择

	//线程结束条件标记，若为true则分别结束各自线程
	flag1 = false;
	flag2 = false;
	//线程已经结束标志，若为true则表明对应线程已结束
	ok1 = false;
	ok2 = false;
	//此模式标记，若为true，则表明进入地狱模式
	Hehe = true;

	Init();//初始化界面

	InitializeCriticalSection(&cs);//初始化临界区变量
	hThread0 = CreateThread(NULL, 0, ThreadFunc0, NULL, 0, &dwThreadId1);//开启线程0，对应左框
	hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &dwThreadId);//开启线程1，对应右框
	
	while (1)//当两个线程都已结束时，跳出循环
	{
		if (ok1&&ok2)
			break;
	}

	//关闭对应的内核对象
	CloseHandle(hThread0);
	CloseHandle(hThread);

	Sleep(50);
	Hehe = false;//设置标记，表明退出地狱模式
	mciSendString("stop 马东梅.mp3 ", 0, 0, 0);//关闭本模式音乐
	mciSendString("play Fade.mp3 repeat", 0, 0, 0);//k开启主循环音乐
	ModeAlter();//跳转到模式选择界面

}

DWORD WINAPI ThreadFunc(LPVOID lpParam)
{
	ClearBlank(2);//清空框内方块
	score2 = 0;  //初始化分数
	int level = 0;  //初始化游戏级别

	Sleep(16);

	srand(time(NULL)+1); //时间种子+1

	int Num = rand() % 7; //创建第一个方块编号
	int nextNum2 = Num;    //保存下一个方块编号

	int blank;  //记录每个方块起始位置
	int x = 0, y = 0;  //记录游戏开始的相对坐标

	int a1[4][4] = { 0 }; //临时使用用来保存当前方块


	while (1)
	{
		
		for (int i = 0;i<4;i++)          //复制方块
			for (int j = 0;j<4;j++)
				if (a1[i][j] = b[nextNum2][i][j])
					blank = i;

		y = 1 - blank;
		x = 4;

		if (Hehe)//是否是地狱模式
		{
			x = rand() % 7 + 1;//设定随机初始掉落位置
		}

		//创建下一个方块
		Num = nextNum2;
		ClearSquare(b[Num][0], 4, 4, 13, 13, dm, dn);
		nextNum2 = rand() % 7;
		WORD wColors[1] = { SQUARE_COLOR[nextNum2] };
		DrawBlocks(b[nextNum2][0], 4, 4, 13, 13, wColors, 1, dm, dn);

		wColors[0] = SQUARE_COLOR[Num];
		DrawBlocks(&a1[0][0], 4, 4, x, y, wColors, 1, dm, dn);

		//显示分数信息
		char string[5];
		wColors[0] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		textout(handle, dm + 27, 5 + dy, wColors, 1, itoa(score2, string, 10));
		textout(handle, dm + 27, 9 + dy, wColors, 1, itoa(level, string, 10));

		int max_delay = 10*T - T * level; //计算不同游戏级别的下落时间间隔

		while (1)
		{
			int delay = 0; //延迟量
			while (delay<max_delay)
			{
				if (_kbhit())  //用if避免按住键使方块卡住
				{
					
					EnterCriticalSection(&cs);//进入临界区
					int key = _getch();
					LeaveCriticalSection(&cs);//退出临界区
					switch (key)
					{
					case KEY_PAUSE:   //暂停
					{
						WORD wColors[1] = { FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY };
						textout(handle, 2, 1, wColors, 1, "暂停。。。。。。");
						while (_getch() != KEY_PAUSE)
						{
						}
						textout(handle, 2, 1, wColors, 1, "                ");
					}
					break;
					case KEY_UP: //旋转
					{
						ClearSquare(&a1[0][0], 4, 4, x, y, dm, dn);
						Turn(a1, 4, 4, &x, y, map1);
						wColors[0] = SQUARE_COLOR[Num];
						DrawBlocks(&a1[0][0], 4, 4, x, y, wColors, 1, dm, dn);
					}
					break;
					case KEY_DOWN://下降
						delay = max_delay;
						break;
					case KEY_LEFT: //左移
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
					case KEY_RIGHT://右移
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
						flag1 = true;//设置结束另一线程的命令
						ok2 = true; //标志此线程结束
						return 0;
					}
					break;
					case KEY_ESC://退出游戏
					{
						MaxSCORE = GetMaxScore();
						GameOver();
						exit(EXIT_SUCCESS);

					}
					break;
				
					}

				}
				Sleep(8);delay++;

				wColors[0] = SQUARE_COLOR[Num];//加快更新方块速度，防止调用Addline函数时闪烁
				DrawBlocks(&a1[0][0], 4, 4, x, y, wColors, 1, dm, dn);

			}
			if (flag2)//若真则结束此线程
			{
				ok2 = true;//标志此线程结束
				return 0;
			}

			if (IsAvailable(&a1[0][0], x, y + 1, 4, 4, map1)) //是否能下移
			{
				ClearSquare(&a1[0][0], 4, 4, x, y, dm, dn);
				y++;
				wColors[0] = SQUARE_COLOR[Num];
				DrawBlocks(&a1[0][0], 4, 4, x, y, wColors, 1, dm, dn);
			}
			else
			{
				if (y <= 1)//判断游戏是否结束
				{
					flag1 = true;//设置结束另一线程的命令
					wColors[0] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
					textout(handle, 50, 12, wColors, 1, "你不小心输了");
					textout(handle, 18, 12, wColors, 1, "你轻松地赢了");
					Sleep(800);
					textout(handle, 50, 12, wColors, 1, "            ");
					textout(handle, 18, 12, wColors, 1, "            ");
					Change();
					ok2 = true; //标志此线程结束
					return 0;
				}

				for (int i = 0;i<4;i++)     //放下方块，更新工作区
					for (int j = 0;j<4;j++)
						if (a1[i][j] && ((i + y)<MAPH - 1) && ((j + x)<MAPW - 1))
							map1[i + y][j + x] = a1[i][j];

				int full, k = 0;
				for (int i = y;i<min(y + 4, MAPH - mark2);i++)
				{
					full = 1;
					for (int j = 1;j<11;j++)
						if (!map1[i][j]) full = 0;
					if (full)   //消掉一行
					{
						DeleteLine(map1, i, dm, dn);
						AddLine(map, i, dx, dy);//消行后在另一工作区增加一行
						k++;
						score2 = score2 + k;
						MaxSCORE = GetMaxScore();//若当前得分超过最高分，则写入txt记录
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
	ClearBlank(1);//清空框内方块
	score1 = 0;  //初始化分数
	int level = 0;  //初始化游戏级别

	Sleep(16);

	srand(time(NULL));//时间种子

	int Num = rand() % 7; //创建第一个方块编号
	int nextNum1 = Num;    //保存下一个方块编号

	int blank;  //记录每个方块起始位置
	int x = 0, y = 0;  //记录游戏开始的相对坐标

	int a[4][4] = { 0 }; //临时使用用来保存当前方块


	while (1)
	{ 	
		for (int i = 0;i<4;i++)          //复制方块
			for (int j = 0;j<4;j++)
				if (a[i][j] = b[nextNum1][i][j])
					blank = i;

		y = 1 - blank;
		x = 4;

		if (Hehe)//是否是地狱模式
		{
			x = rand()%7+1;//设定随机初始掉落位置
		}

		//创建下一个方块
		Num = nextNum1;
		ClearSquare(b[Num][0], 4, 4, 13, 13, dx - 36, dy);
		nextNum1 = rand() % 7;
		WORD wColors[1] = { SQUARE_COLOR[nextNum1] };
		DrawBlocks(b[nextNum1][0], 4, 4, 13, 13, wColors, 1, dx - 36, dy);

		wColors[0] = SQUARE_COLOR[Num];
		DrawBlocks(&a[0][0], 4, 4, x, y, wColors, 1, dx, dy);

		//显示分数信息
		char string[5];
		wColors[0] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		textout(handle, dx - 8, 5 + dy, wColors, 1, itoa(score1, string, 10));
		textout(handle, dx - 8, 9 + dy, wColors, 1, itoa(level, string, 10));

		int max_delay = 10*T - T * level; //计算不同游戏级别的下落时间间隔

		while (1)
		{
			int delay = 0; //延迟量
			while (delay<max_delay)
			{
				if (_kbhit())  //用if避免按住键使方块卡住
				{
					
					EnterCriticalSection(&cs);//进入临界区
					int key = _getch();
					LeaveCriticalSection(&cs);//退出临界区
					switch (key)
					{
					case KEY_PAUSE://暂停
					{
						WORD wColors[1] = { FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY };
						textout(handle, 2, 1, wColors, 1, "暂停。。。。。。");
						while (_getch() != KEY_PAUSE)
						{
						}
						textout(handle, 2, 1, wColors, 1, "                ");
					}
					break;
					case IKEY_UP://旋转
					{
						ClearSquare(&a[0][0], 4, 4, x, y, dx, dy);
						Turn(a, 4, 4, &x, y, map);
						wColors[0] = SQUARE_COLOR[Num];
						DrawBlocks(&a[0][0], 4, 4, x, y, wColors, 1, dx, dy);
					}
					break;
					case IKEY_DOWN://下降
						delay = max_delay;
						break;
					case IKEY_LEFT://左移
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
					case IKEY_RIGHT://右移
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
						flag2 = true;//设置结束另一线程的命令
						ok1 = true; //标志此线程结束
						return 0;
					}
					case KEY_ESC://退出游戏
					{
						MaxSCORE = GetMaxScore();
						GameOver();
						exit(EXIT_SUCCESS);
					}
				 }

				}
				Sleep(8);delay++;

				wColors[0] = SQUARE_COLOR[Num];//加快更新方块速度，防止调用Addline函数时闪烁
				DrawBlocks(&a[0][0], 4, 4, x, y, wColors, 1, dx, dy);
			}
			if (flag1)//若真则结束此线程
			{
				ok1 = true;//标志此线程结束
				return 0;
			}
			if (IsAvailable(&a[0][0], x, y + 1, 4, 4, map)) //是否能下移
			{
				ClearSquare(&a[0][0], 4, 4, x, y, dx, dy);
				y++;
				wColors[0] = SQUARE_COLOR[Num];
				DrawBlocks(&a[0][0], 4, 4, x, y, wColors, 1, dx, dy);
			}
			else
			{
				if (y <= 1)//判断游戏是否结束
				{
					flag2 = true;//设置结束另一线程的命令
					WORD wColors[1] = { FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY };
					textout(handle, 18, 12, wColors, 1, "你不小心输了");
					textout(handle, 50, 12, wColors, 1, "你轻松地赢了");
					Sleep(800);
					textout(handle, 18, 12, wColors, 1, "            ");
					textout(handle, 50, 12, wColors, 1, "            ");
					Change();
					ok1 = true;//标志此线程结束
					return 0;
				}
			
				for (int i = 0;i<4;i++)     //放下方块，更新工作区
					for (int j = 0;j<4;j++)
						if (a[i][j] && ((i + y)<MAPH - 1) && ((j + x)<MAPW - 1))
							map[i + y][j + x] = a[i][j];

				int full, k = 0;
				for (int i = y;i<min(y + 4, MAPH - mark1);i++)
				{
					full = 1;
					for (int j = 1;j<11;j++)
						if (!map[i][j]) full = 0;
					if (full)   //消掉一行
					{
						DeleteLine(map, i, dx, dy);
						AddLine(map1, i, dm, dn);//消行后在另一工作区增加一行
						k++;
						score1 = score1 + k;
						MaxSCORE = GetMaxScore();//若当前得分超过最高分，则写入txt记录
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

