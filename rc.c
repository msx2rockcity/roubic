#ifdef unix
#include <GL/gl.h>
#include <GL/glu.h>
#include <aux.h>
#include <stdio.h>
#include <math.h>
#else
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glaux.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#endif

/* 角度の回転用マクロ x を y 度回転させた数値を与える */
#define turn( x, y )  (((x) + (y) > 360.0 )?  (x) + (y) - 360.0 : ((x) + (y) < 0)? 360-(x)+(y) : (x)+(y))

#define FPS 30

/* 初期ウインドウサイズ */
#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 600

/*-------------------------------------------------------

               各種データ構造体の定義

  -------------------------------------------------------*/

/*  一つのキューブを表す構造体の定義 */
typedef struct{
	GLfloat spin_x;   /* キューブの中心に対する回転角度 */
	GLfloat spin_y;
	GLfloat spin_z;
	GLfloat pos_x;    /* キューブの中心からの位置 */
	GLfloat pos_y;
	GLfloat pos_z;
	int color[6];     /* キューブの初期色状態 */
	int now_color[6]; /* キューブの現在の色状態 */
} CUBE;

/* ルービックキューブデータ構造体型 */
struct ROUBIC{
	CUBE cube[27];          /* 個々のキューブのデータ構造体 */
	GLfloat center_spin_x;	/* キューブ全体の回転角 */
	GLfloat center_spin_y;
	GLfloat center_spin_z;
} ric;

/* カラーコード */
struct COLOR{
	GLfloat r;
	GLfloat g;
	GLfloat b;
} COLOR[] = {
	{  0.3, 0.3, 0.3  }, /* 表になっていない面の色 */
	{  1.0, 0.0, 0.0  }, /* ６面の対応する色 */
	{  0.0, 1.0, 0.0  },
	{  0.0, 0.0, 1.0  },
	{  1.0, 1.0, 0.0  },
	{  1.0, 0.0, 1.0  },
	{  0.0, 1.0, 1.0  }
};

/* キューブの動きのデータ構造体 */
struct MOVE{
	int move[9];     /* 回転前のキューブ全体の位置     */
	int moved[9];    /* 回転後のキューブ全体の位置変化 */
	int colord[6];   /* 回転後の個々のキューブの向きの変化 */
	int pole[3];     /* 回転軸 ( x, y, z ) */
	float rad;       /* 回転方向 */
} MOVE[18] =
{
	{{  1, 2, 3, 4, 5, 6, 7, 8, 9 },{  7, 4, 1, 8, 5, 2, 9, 6, 3 }, {2,6,3,1,5,4}, {0,0,1}, 1.0 },
	{{  1, 2, 3, 4, 5, 6, 7, 8, 9 },{  3, 6, 9, 2, 5, 8, 1, 4, 7 }, {4,1,3,6,5,2}, {0,0,1},-1.0 },
	{{ 10,11,12,13,14,15,16,17,18 },{ 16,13,10,17,14,11,18,15,12 }, {2,6,3,1,5,4}, {0,0,1}, 1.0 },
	{{ 10,11,12,13,14,15,16,17,18 },{ 12,15,18,11,14,17,10,13,16 }, {4,1,3,6,5,2}, {0,0,1},-1.0 },
	{{ 19,20,21,22,23,24,25,26,27 },{ 25,22,19,26,23,20,27,24,21 }, {2,6,3,1,5,4}, {0,0,1}, 1.0 },
	{{ 19,20,21,22,23,24,25,26,27 },{ 21,24,27,20,23,26,19,22,25 }, {4,1,3,6,5,2}, {0,0,1},-1.0 },
	
	{{ 19,10, 1,22,13, 4,25,16, 7 },{ 25,22,19,16,13,10, 7, 4, 1 }, {3,2,6,4,1,5}, {1,0,0},-1.0 },
	{{ 19,10, 1,22,13, 4,25,16, 7 },{  1, 4, 7,10,13,16,19,22,25 }, {5,2,1,4,6,3}, {1,0,0}, 1.0 },
	{{ 20,11, 2,23,14, 5,26,17, 8 },{ 26,23,20,17,14,11, 8, 5, 2 }, {3,2,6,4,1,5}, {1,0,0},-1.0 },
	{{ 20,11, 2,23,14, 5,26,17, 8 },{  2, 5, 8,11,14,17,20,23,26 }, {5,2,1,4,6,3}, {1,0,0}, 1.0 },
	{{ 21,12, 3,24,15, 6,27,18, 9 },{ 27,24,21,18,15,12, 9, 6, 3 }, {3,2,6,4,1,5}, {1,0,0},-1.0 },
	{{ 21,12, 3,24,15, 6,27,18, 9 },{  3, 6, 9,12,15,18,21,24,27 }, {5,2,1,4,6,3}, {1,0,0}, 1.0 },
	
	{{  1, 2, 3,10,11,12,19,20,21 },{ 19,10, 1,20,11, 2,21,12, 3 }, {1,3,4,5,2,6}, {0,1,0}, 1.0 },
	{{  1, 2, 3,10,11,12,19,20,21 },{  3,12,21, 2,11,20, 1,10,19 }, {1,5,2,3,4,6}, {0,1,0},-1.0 },
	{{  4, 5, 6,13,14,15,22,23,24 },{ 22,13, 4,23,14, 5,24,15, 6 }, {1,3,4,5,2,6}, {0,1,0}, 1.0 },
	{{  4, 5, 6,13,14,15,22,23,24 },{  6,15,24, 5,14,23, 4,13,22 }, {1,5,2,3,4,6}, {0,1,0},-1.0 },
	{{  7, 8, 9,16,17,18,25,26,27 },{ 25,16, 7,26,17, 8,27,18, 9 }, {1,3,4,5,2,6}, {0,1,0}, 1.0 },
	{{  7, 8, 9,16,17,18,25,26,27 },{  9,18,27, 8,17,26, 7,16,25 }, {1,5,2,3,4,6}, {0,1,0},-1.0 },
};

/* 矢印のデータ */
struct ALLOW{
	GLfloat pos_x;
	GLfloat pos_y;
	GLfloat pos_z;
	GLfloat spin_x;
	GLfloat spin_y;
	GLfloat spin_z;
	GLfloat scale_x;
	GLfloat scale_y;
	GLfloat scale_z;
	GLubyte color_r; /* モーションを選択するので，すべて，違う色にすること */
	GLubyte color_g;
	GLubyte color_b;
} allow[] = {
	{ -2.0, 1.0,  1.0,    0.0,-90.0, 0.0,  1.0, 1.0, 1.0,   130, 130, 130 },
	{  1.0,-2.0,  1.0,   90.0, 90.0, 0.0,  1.0, 1.0, 1.0,   129, 129, 129 },
	{ -2.0, 1.0,  0.0,    0.0,-90.0, 0.0,  1.0, 1.0, 1.0,   128, 128, 128 },
	{  1.0,-2.0,  0.0,   90.0, 90.0, 0.0,  1.0, 1.0, 1.0,   127, 127, 127 },
	{ -2.0, 1.0, -1.0,    0.0,-90.0, 0.0,  1.0, 1.0, 1.0,   126, 126, 126 },
	{  1.0,-2.0, -1.0,   90.0, 90.0, 0.0,  1.0, 1.0, 1.0,   125, 125, 125 },
	
	{ -1.0, 1.0, -2.0,    0.0,180.0, 0.0,  1.0, 1.0, 1.0,   124, 124, 124 },
	{ -1.0,-2.0,  1.0,   90.0,  0.0, 0.0,  1.0, 1.0, 1.0,   123, 123, 123 },
	{  0.0, 1.0, -2.0,    0.0,180.0, 0.0,  1.0, 1.0, 1.0,   122, 122, 122 },
	{  0.0,-2.0,  1.0,   90.0,  0.0, 0.0,  1.0, 1.0, 1.0,   121, 121, 121 },
	{  1.0, 1.0, -2.0,    0.0,180.0, 0.0,  1.0, 1.0, 1.0,   120, 120, 120 },
	{  1.0,-2.0,  1.0,   90.0,  0.0, 0.0,  1.0, 1.0, 1.0,   119, 119, 119 },
	
	{  1.0,-1.0, -2.0,   0.0, 180.0,-90.0, 1.0, 1.0, 1.0,   118, 118, 118 },
	{ -2.0,-1.0,  1.0,   90.0, 0.0, -90.0, 1.0, 1.0, 1.0,   117, 117, 117 },
	{  1.0, 0.0, -2.0,   0.0, 180.0,-90.0, 1.0, 1.0, 1.0,   116, 116, 116 },
	{ -2.0, 0.0,  1.0,   90.0, 0.0, -90.0, 1.0, 1.0, 1.0,   115, 115, 115 },
	{  1.0, 1.0, -2.0,   0.0, 180.0,-90.0, 1.0, 1.0, 1.0,   114, 114, 114 },
	{ -2.0, 1.0,  1.0,   90.0, 0.0, -90.0, 1.0, 1.0, 1.0,   113, 113, 113 },
	
	{ -3.0, 1.0,  0.0,   0.0,-90.0, 0.0,   3.0, 1.0, 1.0,   112, 112, 112 },
	{  1.0,-3.0,  0.0,  90.0, 90.0, 0.0,   3.0, 1.0, 1.0,   111, 111, 111 },
	{  0.0, 1.0, -3.0,   0.0,180.0, 0.0,   3.0, 1.0, 1.0,   110, 110, 110 },
	{  0.0,-3.0,  1.0,  90.0,  0.0, 0.0,   3.0, 1.0, 1.0,   109, 109, 109 },
	{  1.0, 0.0, -3.0,   0.0,180.0,-90.0,  3.0, 1.0, 1.0,   108, 108, 108 },
	{ -3.0, 0.0,  1.0,  90.0, 0.0, -90.0,  3.0, 1.0, 1.0,   107, 107, 107 }
};
	
/* ウインドウサイズ */
GLint xsize = WINDOW_WIDTH;
GLint ysize = WINDOW_HEIGHT;

/* アニメーションのフレーム数（９０度回転するまでのフレーム数）*/
GLint FRAME = 10;  

/* アニメーション用フラグ */
GLint Anime = 0;
GLint Move  = 0;
GLint Turn  = 0;


/***************************************
 *
 * 初期化とキューブのデータを作る関数 
 *
 ***************************************/
static void CUBE_MAKE( void )
{
	int x, y, z, no = 0;

	for(z = 1; z > -2; z-- ){
		for(y = -1; y < 2; y++ ){
			for(x = -1; x < 2; x++ ){

				/* 個々のキューブの位置初期化 */
				ric.cube[no].pos_x = (float)x;
				ric.cube[no].pos_y = (float)y;
				ric.cube[no].pos_z = (float)z; 

				/* 個々のキューブの回転角度初期化 */
				ric.cube[no].spin_x = 0.0; 
				ric.cube[no].spin_y = 0.0;
				ric.cube[no].spin_z = 0.0;

				/* 個々のキューブの面の色初期化 */
				ric.cube[no].now_color[0] = ric.cube[no].color[0] = (y ==  1)? 1 : 0;
				ric.cube[no].now_color[1] = ric.cube[no].color[1] = (x ==  1)? 2 : 0;
				ric.cube[no].now_color[2] = ric.cube[no].color[2] = (z ==  1)? 3 : 0;
				ric.cube[no].now_color[3] = ric.cube[no].color[3] = (x == -1)? 4 : 0;
				ric.cube[no].now_color[4] = ric.cube[no].color[4] = (z == -1)? 5 : 0;
				ric.cube[no].now_color[5] = ric.cube[no].color[5] = (y == -1)? 6 : 0;
				no++;
			}
		}
	}

	/* キューブ全体の回転初期化 */
	ric.center_spin_x = 0.0;
	ric.center_spin_y = 0.0;
	ric.center_spin_z = 0.0;

	/* アニメーションフラグ初期化 */
	Anime = 0;
	Move = 0;
}

/*******************************************
 *
 * 矢印を書く関数
 *
 *******************************************/
static void Allow_DRAW( void )
{
	GLfloat half = 1.0 / 2.0;

	glBegin(GL_TRIANGLES);
   	glVertex3f( half    ,half,  0.0);
   	glVertex3f(-half    ,half,  0.0);
   	glVertex3f(      0.0,half, half);
	glEnd();

	glBegin(GL_QUADS);
   	glVertex3f( half/2.0,half,  0.0);
   	glVertex3f( half/2.0,half,-half);
   	glVertex3f(-half/2.0,half,-half);
   	glVertex3f(-half/2.0,half,  0.0);

	glEnd();
}

static void ALLOW(void)
{
	int i;

	for(i = 0; i < 24; i++ ){
		glPushMatrix();
		if((Anime - 1) > 0 && i == Move && (Anime & 1) != 0 ) glColor3f( 1.0, 1.0, 0.0 );
		else glColor4ub(allow[i].color_r,allow[i].color_g,allow[i].color_b,1.0);
		glTranslatef(allow[i].pos_x,allow[i].pos_y,allow[i].pos_z);
		glRotatef(allow[i].spin_z,0.0,0.0,1.0);
		glRotatef(allow[i].spin_y,0.0,1.0,0.0);
		glRotatef(allow[i].spin_x,1.0,0.0,0.0);
		glScalef(allow[i].scale_x,allow[i].scale_y,allow[i].scale_z);

		Allow_DRAW();
		
		glPopMatrix();
	}
}

/****************************************
 *
 * キューブを書く関数 
 *
 ****************************************/
 

static void CUBE_ONE_DRAW(int no)
{
	/*  Object draw */
	GLfloat harf = 0.9 / 2.0;

	CUBE *cube = &(ric.cube[no]);

	glBegin(GL_QUADS);
	glNormal3f(0.0,  1.0, 0.0);
	glColor4f(COLOR[cube->now_color[0]].r,COLOR[cube->now_color[0]].g,COLOR[cube->now_color[0]].b,1.0);
	glVertex3f(-harf, harf,-harf);
        glVertex3f(-harf, harf, harf);
        glVertex3f( harf, harf, harf);
        glVertex3f( harf, harf,-harf);
	glEnd();

	glBegin(GL_QUADS);
	glColor4f(COLOR[cube->now_color[1]].r,COLOR[cube->now_color[1]].g,COLOR[cube->now_color[1]].b,1.0);
        glNormal3f(1.0, 0.0, 0.0);
	glVertex3f( harf,-harf, harf);
        glVertex3f( harf,-harf,-harf);
        glVertex3f( harf, harf,-harf);
        glVertex3f( harf, harf, harf);
	glEnd();

	glBegin(GL_QUADS);
	glColor4f(COLOR[cube->now_color[2]].r,COLOR[cube->now_color[2]].g,COLOR[cube->now_color[2]].b,1.0);
        glNormal3f(0.0, 0.0,  1.0);
	glVertex3f(-harf,-harf, harf);
        glVertex3f( harf,-harf, harf);
        glVertex3f( harf, harf, harf);
        glVertex3f(-harf, harf, harf);
	glEnd();

	glBegin(GL_QUADS);
	glColor4f(COLOR[cube->now_color[3]].r,COLOR[cube->now_color[3]].g,COLOR[cube->now_color[3]].b,1.0);
        glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(-harf,-harf,-harf);
        glVertex3f(-harf,-harf, harf);
        glVertex3f(-harf, harf, harf);
        glVertex3f(-harf, harf,-harf);
	glEnd();

	glBegin(GL_QUADS);
	glColor4f(COLOR[cube->now_color[4]].r,COLOR[cube->now_color[4]].g,COLOR[cube->now_color[4]].b,1.0);
        glNormal3f(0.0, 0.0, -1.0);
	glVertex3f( harf,-harf,-harf);
        glVertex3f(-harf,-harf,-harf);
        glVertex3f(-harf, harf,-harf);
        glVertex3f( harf, harf,-harf);
	glEnd();
	glBegin(GL_QUADS);
	glColor4f(COLOR[cube->now_color[5]].r,COLOR[cube->now_color[5]].g,COLOR[cube->now_color[5]].b,1.0);
        glNormal3f(0.0, -1.0, 0.0);
	glVertex3f( harf,-harf, harf);
        glVertex3f(-harf,-harf, harf);
        glVertex3f(-harf,-harf,-harf);
        glVertex3f( harf,-harf,-harf);
	glEnd();
}

static void CUBE_DRAW(int no)
{

	glPushMatrix();

	/* キューブの回転（中心に対する回転＋キューブ全体の回転）*/

	glRotatef(ric.cube[no].spin_z,0.0,0.0,1.0);
	glRotatef(ric.cube[no].spin_y,0.0,1.0,0.0);
	glRotatef(ric.cube[no].spin_x,1.0,0.0,0.0);

	/* キューブの平行移動 */
	glTranslatef(ric.cube[no].pos_x,ric.cube[no].pos_y,ric.cube[no].pos_z);

	/* キューブ書きルーチンを呼ぶ */
	CUBE_ONE_DRAW(no);

	glPopMatrix();

}
/************************************************************* 
 *
 * キューブのデータを操作する関数 （引数は操作パターンｎｏ)
 *
 *************************************************************/
static void CUBE_CONTROL_SUB(int no)
{
	int i;

	for( i = 0; i < 9; i++ ){
		if (MOVE[no].pole[0] == 1){
			ric.cube[MOVE[no].move[i]-1].spin_x = 
			      turn(ric.cube[MOVE[no].move[i]-1].spin_x , 90.0 / (GLfloat)FRAME * MOVE[no].rad);
		}
		if (MOVE[no].pole[1] == 1){ 
			ric.cube[MOVE[no].move[i]-1].spin_y = 
                  turn(ric.cube[MOVE[no].move[i]-1].spin_y , 90.0 / (GLfloat)FRAME * MOVE[no].rad);
		}
		if (MOVE[no].pole[2] == 1){
			ric.cube[MOVE[no].move[i]-1].spin_z = 
			      turn(ric.cube[MOVE[no].move[i]-1].spin_z , 90.0 / (GLfloat)FRAME * MOVE[no].rad);
		}
	}
}

static void CUBE_CONTROL(int no)
{
	switch(no)
	{
		case 18: CUBE_CONTROL_SUB(0);
		         CUBE_CONTROL_SUB(2);
		         CUBE_CONTROL_SUB(4);
		         break;

		case 19: CUBE_CONTROL_SUB(1);
		         CUBE_CONTROL_SUB(3);
		         CUBE_CONTROL_SUB(5);
		         break;

		case 20: CUBE_CONTROL_SUB(6);
		         CUBE_CONTROL_SUB(8);
		         CUBE_CONTROL_SUB(10);
		         break;

		case 21: CUBE_CONTROL_SUB(7);
		         CUBE_CONTROL_SUB(9);
		         CUBE_CONTROL_SUB(11);
		         break;

		case 22: CUBE_CONTROL_SUB(12);
		         CUBE_CONTROL_SUB(14);
		         CUBE_CONTROL_SUB(16);
		         break;

		case 23: CUBE_CONTROL_SUB(13);
		         CUBE_CONTROL_SUB(15);
		         CUBE_CONTROL_SUB(17);
		         break;

		default: CUBE_CONTROL_SUB(no);
	}
}

/**************************************************************** 
 *
 * キューブの色配列を並び変える関数 (引数は操作パターンｎｏ)
 *
 ****************************************************************/
static void CUBE_MOVE_SUB(int no)
{
	int i, j, stack[9][6];

	for(i = 0; i < 9; i++ )
	{
		for(j = 0; j < 6; j++ )
		{
			stack[i][j] = ric.cube[MOVE[no].moved[i]-1].now_color[MOVE[no].colord[j]-1];
		}
	}

	for(i = 0; i < 9; i++ )
	{
		for(j = 0; j < 6; j++ )
		{
			ric.cube[MOVE[no].move[i]-1].now_color[j] = stack[i][j];
		}

		ric.cube[MOVE[no].move[i]-1].spin_x = 0.0;  /* 回転角を０にもどす */
		ric.cube[MOVE[no].move[i]-1].spin_y = 0.0;
		ric.cube[MOVE[no].move[i]-1].spin_z = 0.0;

	}
}


static void CUBE_MOVE(int no)
{
	switch(no)
	{
		case 18: CUBE_MOVE_SUB(0);
		         CUBE_MOVE_SUB(2);
		         CUBE_MOVE_SUB(4);
		         break;

		case 19: CUBE_MOVE_SUB(1);
		         CUBE_MOVE_SUB(3);
		         CUBE_MOVE_SUB(5);
		         break;

		case 20: CUBE_MOVE_SUB(6);
		         CUBE_MOVE_SUB(8);
		         CUBE_MOVE_SUB(10);
		         break;

		case 21: CUBE_MOVE_SUB(7);
		         CUBE_MOVE_SUB(9);
		         CUBE_MOVE_SUB(11);
		         break;

		case 22: CUBE_MOVE_SUB(12);
		         CUBE_MOVE_SUB(14);
		         CUBE_MOVE_SUB(16);
		         break;

		case 23: CUBE_MOVE_SUB(13);
		         CUBE_MOVE_SUB(15);
		         CUBE_MOVE_SUB(17);
		         break;

		default: CUBE_MOVE_SUB(no);
	}
}

/********************************************
 *
 * キューブ全部を書く関数 
 *
 ********************************************/
static void CALLBACK ALL_DRAW( void )
{
	int i;

	/* 画面を消す */
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	

	glPushMatrix();

	/* キューブ全体の回転をする */
	glRotatef(ric.center_spin_z,0.0,0.0,1.0);
	glRotatef(ric.center_spin_y,0.0,1.0,0.0);
	glRotatef(ric.center_spin_x,1.0,0.0,0.0);

	/* キューブ個々の回転，表示を行う */
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	for(i = 0;i < 27;i++ ){
		CUBE_DRAW(i);
	}

	/* 矢印の表示をする */
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	ALLOW();

	glPopMatrix();

/* FPS同期処理 */
#ifdef FPS
  	{
    		float tt = (float)timeGetTime();
    		float ts = tt / (1000.0/(float)FPS);
    		int tn = (int)((int)(ts+1) * (1000.0/(float)FPS));
    		if(tn > (int)tt){
      			Sleep(tn-(int)tt);
    		}
  	}
#endif
	glFlush();
	auxSwapBuffers();
}

/*****************************************************
 *
 * キューブを回転させるプログラム（毎回呼ばれる関数）
 *
 *****************************************************/
static void CALLBACK CUBE_SPIN(void)
{
	int x,y;
	if( Turn == 1){           /* マウスのドラッグフラグが立っているなら */
		auxGetMouseLoc(&x,&y);
		ric.center_spin_y = (float)(x) * 360.0/ (float)xsize;
		ric.center_spin_x = (float)(y) * 360.0/ (float)ysize;
	}
	else if(Anime > 0){       /* アニメーションフラグが立っているなら */
		CUBE_CONTROL( Move );
		if( --Anime == 0 ){   /* アニメーションが終了したら，キューブ配列を変化させる */
			CUBE_MOVE( Move );
 		}
	}
	
	ALL_DRAW();
}

/****************************************
 *
 * 各種キーの関数
 *
 ****************************************/

/* 左ボタンクリックで回転パターン検索 ＆ アニメーションフラグセット */
static void CALLBACK DragOn(AUX_EVENTREC *event)
{
	GLint x,y,i;
	GLubyte color[3];

	if( Anime == 0 ){
		auxGetMouseLoc(&x,&y);

		glReadBuffer(GL_FRONT);
		glReadPixels(x,ysize-y,1,1,GL_RGB,GL_UNSIGNED_BYTE,color);

		for( i = 0; i < 24; i++ ){
			if(allow[i].color_r == color[0] && allow[i].color_g == color[1] && allow[i].color_b == color[2]){
				Move = i;
				Anime = (GLint)FRAME;
				break;
			}
		}
	}
}

/* 右ボタンクリックで元の状態に戻す */	
static void CALLBACK DragOff(AUX_EVENTREC *event)
{
	CUBE_MAKE();
}

/* 真ん中ボタンドラッグのフラグを立てる */
static void CALLBACK TurnOn(AUX_EVENTREC *event)
{
	Turn = 1;
}

static void CALLBACK TurnOff(AUX_EVENTREC *event)
{
	Turn = 0;
}

/* ｒボタンでランダムに変化させる */
static void CALLBACK RANDOM_CUBE( void )
{
	unsigned int i, r;

	for ( i = 0; i < 50; i++ ){
		r = rand() % 24;
		CUBE_MOVE(r);
	}
}

/**********************************************
 *
 * ウインドウの形状変化ルーチン
 *
 **********************************************/
static void CALLBACK ReshapeWindow(int x,int y)
{
	glViewport(0,0,x,y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(40.0,(GLfloat)x/(GLfloat)y,0.1,25.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(6.0,6.0,6.0, 0.0,0.0,0.0, 0.0,1.0,0.0);

	xsize=x;
	ysize=y;
}

/**********************************************
 *
 * メイン関数
 *
 **********************************************/

int main(int argc,char** argv)
{
	int i,x,y;

	GLfloat light0_position[] = {  0.0,  0.0,  5.0, 0.0 };  /* 照明の位置 */

	auxInitDisplayMode(AUX_DOUBLE|AUX_RGBA|AUX_DEPTH);
	auxInitPosition(0,0,xsize,ysize);
	auxInitWindow("ROUBIC.CUBE");

	/* 乱数の基数をセットする */
	time( &i ); srand( i );

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0,0.0,0.0,1.0);

	/* ポリゴンの処理方法を決定 */
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glShadeModel(GL_FLAT);

	/* Backface Culling */ 
	glCullFace(GL_BACK);

	/* 光源セット */
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position );
	glEnable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	/* 材質パラメータ自動設定 */
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
	glEnable(GL_COLOR_MATERIAL);

	/* キューブデータ初期化 */
	CUBE_MAKE();

	/* コールバック関数設定 */
	auxReshapeFunc(ReshapeWindow);
	auxIdleFunc(CUBE_SPIN); 
	auxMouseFunc(AUX_LEFTBUTTON,AUX_MOUSEDOWN,DragOn);   /* マウスの左で矢印をクリックすると回転 */
	auxMouseFunc(AUX_RIGHTBUTTON,AUX_MOUSEDOWN,DragOff); /* マウスの右をクリックすると元に戻る */
	auxMouseFunc(AUX_MIDDLEBUTTON,AUX_MOUSEDOWN,TurnOn); /* マウスの真ん中でドラッグすると全体が回転する */
	auxMouseFunc(AUX_MIDDLEBUTTON,AUX_MOUSEUP,TurnOff);
	auxKeyFunc(AUX_r,RANDOM_CUBE);                       /* キーのｒを押すとランダムにしてくれる */
	
	auxGetMouseLoc(&x,&y);
	ric.center_spin_x = 0;/*ric.center_spin_x;*/
	ric.center_spin_y = 0;/*ric.center_spin_y;*/
	ric.center_spin_z = 0;
		
	/* メイン処理へ */
	auxMainLoop(ALL_DRAW);

}



















