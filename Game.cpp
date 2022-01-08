#include<GL/glut.h>
#include<opencv2/opencv.hpp>
#include"model.hpp"
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

//bgm�� �ֱ����� ����
#include<mmsystem.h>
#pragma comment(lib,"winmm")


#define _WINDOW_WIDTH 1200
#define _WINDOW_HEIGHT 800

#define COIN "bgm/coin.wav"
#define ITEM1 "bgm/item1.wav"
#define ITEM2 "bgm/item2.wav"
#define CRASH1 "bgm/crash1.wav"		//��� �ɷ��� ��
#define CRASH2 "bgm/crash2.wav"		//������ �ɷ��� ��
#define CRASH3 "bgm/crash3.wav"		//���� �ε����� ��
#define JUMP "bgm/jump.wav"			//����
#define VIC "bgm/victory.wav"		//����
#define OVER "bgm/gameover.wav"		//GameOver
#define LOSE "bgm/lose.wav"			//Lose



using namespace cv;

int counter = 0;		//������ �帧�� �����ϱ� ���� counter ����
int flag = 0;
bool cx = false, cy = false, cz = false;
int keystate[256];

//�䳢�� �ź��̰� �޸��� �������� ���� ����
int rab_arm1 = -60;
int rab_arm2 = 60;
int rab_leg1 = -50;
int rab_leg2 = 50;
int tur_1leg = -40;
int tur_2leg = 40;
int dir_rab_arm = 2;
int dir_rab_leg = 2;
int dir_tur_leg = 1;

GLfloat rab_run = 0;	//�䳢 z�� ��ǥ
GLfloat tur_run = 0;	//�ź� z�� ��ǥ

GLfloat speed_r = 0.4;		//�䳢 �ӵ�
GLfloat speed_t = 0.24;		//�ź� �ӵ�

//�ź��� �������
GLfloat turn = 0;

//������ ������
GLfloat cloud = 2;


//gluLookAt
GLfloat camx = 0, camy = 2, camz = -4;
GLfloat cam2x = 0, cam2y = 0, cam2z = 0;
GLfloat cam_upx = 0, cam_upy = 1, cam_upz = 0;

int jump_stat = 0;	//�ź��� ���� ����

//camz ���� �Ͻ������� ����
GLfloat tmpz;
GLfloat tmpz2;
GLfloat tmpz3;

//�þ߿� ���� ����� �׸��� ���� ����
int sight = 1;

//Game Over flag
int game_over = 0;

//�䳢�� ���� �Ͼ �� ���
GLfloat rab_sleep = 0;
GLfloat sleep_down = 0;
GLfloat rab_Ty = 0;
GLfloat sleep_nose = 0;
int wakeup_r = 0;

//�ؽ��� ����
Mat image[12];
GLuint tex_ids[12] = { 1,2,3,4,5,6,7,8,9,10,11,12 };
int nTex = 12;

//������Ʈ
CModel m;
CModel stone;
CModel bird;
CModel h;

int f_hb = 0;	//F5 ������ ��Ʈ�ڽ� �׸��� ���� ����

int i_stat = 0;		//�������� ����
int i_hit = 0;		//������ ȹ���
int s_hit = 0;		//������ �ε����� ��
int h_hit = 0;		//���ۿ� ������ ��
int t_hit = 0;		//��� �ε����� �� flag
int tmptimer = 0;	//�ε��� ������ �Ͻ� ����
int score = 0;		//���� ȹ�� ����

// ������ ��ġ�� �����ϱ� ���� ����
GLfloat dir_coin = -0.5;		//������ x����ǥ �����ϱ� ���� ����
int coin_count[50] = { 0 };		//������ ����� �������� �Ǻ��ϴ� �迭 0�϶� �׸��� 1�϶� X
GLfloat coin_x[50] = { 0 };		//������ x����ǥ ���� �迭

float end_Time = 0;				//������ �����ð��� ����

//position ����ü
struct position {
	GLfloat x;
	GLfloat y;
	GLfloat z;
	//Hitbox 3���� ��ǥ�� �ּ�, �ִ�
	GLfloat hx1;
	GLfloat hx2;
	GLfloat hy1;
	GLfloat hy2;
	GLfloat hz1;
	GLfloat hz2;
};

// position ����ü ����
position* turtle;
position* rabbit;
position* hurdle;
position* coin;
position* stone1, * stone2, * stone3;
position* i_box;
position* hole;

//position ����ü�� ��Ʈ�ڽ��� �׸��� ��Ʈ�ڽ� ������ �����ϴ� ����
void drawHb(position* p, GLfloat xr, GLfloat yr, GLfloat zr) {

	glColor3f(1, 0, 0);
	if (f_hb) {

		glBegin(GL_LINE_LOOP);
		glVertex3f(p->x + xr, p->y + yr, p->z + zr);
		glVertex3f(p->x - xr, p->y + yr, p->z + zr);
		glVertex3f(p->x - xr, p->y - yr, p->z + zr);
		glVertex3f(p->x + xr, p->y - yr, p->z + zr);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3f(p->x + xr, p->y - yr, p->z - zr);
		glVertex3f(p->x + xr, p->y + yr, p->z - zr);
		glVertex3f(p->x - xr, p->y + yr, p->z - zr);
		glVertex3f(p->x - xr, p->y - yr, p->z - zr);
		glEnd();

		glBegin(GL_LINES);
		glVertex3f(p->x + xr, p->y + yr, p->z + zr);
		glVertex3f(p->x + xr, p->y + yr, p->z - zr);
		glVertex3f(p->x - xr, p->y + yr, p->z + zr);
		glVertex3f(p->x - xr, p->y + yr, p->z - zr);
		glVertex3f(p->x - xr, p->y - yr, p->z + zr);
		glVertex3f(p->x - xr, p->y - yr, p->z - zr);
		glVertex3f(p->x + xr, p->y - yr, p->z + zr);
		glVertex3f(p->x + xr, p->y - yr, p->z - zr);
		glEnd();
	}
	p->hx1 = p->x + xr;
	p->hx2 = p->x - xr;
	p->hy1 = p->y + yr;
	p->hy2 = p->y - yr;
	p->hz1 = p->z + zr;
	p->hz2 = p->z - zr;
}


//position ����ü �ʱ�ȭ
void init(position* obj, GLfloat x, GLfloat y, GLfloat z) {
	obj->x = x;
	obj->y = y;
	obj->z = z;
}

void reshape(int width, int height) {
	GLfloat left = 0, bottom = 0;
	glViewport(left, bottom, width - left, height - bottom);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat ratio = (float)(width - left) / (height - bottom);
	gluPerspective(80, ratio, 0.1, 500);
}

// ������ü(Wire)�� �׸��� �Լ�
void drawCuboid(GLfloat sx, GLfloat sy, GLfloat sz) {
	glPushMatrix();
	glScalef(sx, sy, sz);
	glutWireCube(1);
	glPopMatrix();
}

//���� �׸��� �Լ�
void drawCircle(GLfloat sx, GLfloat sy, GLfloat sz) {
	glPushMatrix();
	glScalef(sx, sy, sz);
	glutSolidSphere(0.5, 30, 30);
	glPopMatrix();
}

//�䳢 ��ü
void rab_Body() {
	glRotated(rab_sleep, 0, 0, 1);
	glTranslatef(-rab_sleep * 0.004, rab_Ty, 0);
	glPushMatrix();
	glTranslatef(0, 0.2, 0);
	drawCircle(0.8, 0.96, 0.58);
	glPopMatrix();
	glPushMatrix();
	glColor3f(1, 0.8, 0.9);
	glTranslatef(0, 0.15, 0.2);
	drawCircle(0.48, 0.56, 0.32);
	glPopMatrix();
	glColor3f(1, 1, 1);
}

//�䳢 ��, ��
void rab_UpperArm(GLfloat angle, GLfloat lx, GLfloat ly, GLfloat lz) {
	glTranslatef(lx, ly, lz);
	glRotatef(-90, 0, 1, 0);
	glRotatef(angle, 0, 0, 1);
	glTranslatef(lx, 0.0, 0);
	drawCircle(0.5, 0.2, 0.2);
}

void rab_LowerArm(GLfloat angle, GLfloat lx, GLfloat ly, GLfloat lz) {
	glTranslatef(lx, 0, 0);
	glRotatef(-angle, 0, 0, 1);
	glTranslatef(lx, 0, 0);
	drawCircle(0.5, 0.2, 0.2);
}

void rab_UpperArm2(GLfloat angle, GLfloat lx, GLfloat ly, GLfloat lz) {
	glTranslatef(lx, ly, lz);
	glRotatef(90, 0, 1, 0);
	glRotatef(angle, 0, 0, 1);
	glTranslatef(lx, 0.0, 0);
	drawCircle(0.5, 0.2, 0.2);
}

void rab_LowerArm2(GLfloat angle, GLfloat lx, GLfloat ly, GLfloat lz) {
	glTranslatef(lx, 0, 0);
	glRotatef(angle, 0, 0, 1);
	glTranslatef(lx, 0, 0);
	drawCircle(0.5, 0.2, 0.2);
}

void rab_Hand(GLfloat lx, GLfloat ly, GLfloat lz) {
	glTranslatef(lx, ly, lz);
	drawCircle(0.2, 0.2, 0.2);
}
// �䳢 �ٸ�, ��
void rab_UpperLeg(GLfloat angle, GLfloat lx, GLfloat ly, GLfloat lz) {
	glTranslatef(lx, ly, lz);
	glRotatef(-90, 0, 0, 1);
	glRotatef(angle, 0, 1, 0);
	glTranslatef(lx, 0.0, 0);
	drawCircle(0.5, 0.2, 0.2);
}

void rab_LowerLeg(GLfloat angle, GLfloat lx, GLfloat ly, GLfloat lz) {
	glTranslatef(lx, 0, 0);
	glRotatef(-angle, 0, 1, 0);
	glTranslatef(lx, 0, 0);
	drawCircle(0.5, 0.2, 0.2);
}

void rab_UpperLeg2(GLfloat angle, GLfloat lx, GLfloat ly, GLfloat lz) {
	glTranslatef(lx, ly, lz);
	glRotatef(-90, 0, 0, 1);
	glRotatef(-angle, 0, 1, 0);
	glTranslatef(-lx, 0, 0);
	drawCircle(0.5, 0.2, 0.2);
}

void rab_LowerLeg2(GLfloat angle, GLfloat lx, GLfloat ly, GLfloat lz) {
	glTranslatef(lx, 0, 0);
	glRotatef(angle, 0, 1, 0);
	glTranslatef(lx, 0, 0);
	drawCircle(0.5, 0.2, 0.2);
}

void rab_Foot(GLfloat lx, GLfloat ly, GLfloat lz) {
	glTranslatef(lx, ly, lz);
	glRotatef(90, 1, 0, 0);
	drawCircle(0.1, 0.25, 0.1);
}

//���
void drawCarrot(GLfloat angle, int xyz, GLfloat lx, GLfloat ly, GLfloat lz) {
	glPushMatrix();
	glTranslatef(lx, ly, lz);
	if (xyz == 1)
		glRotatef(angle, 1, 0, 0);
	else if (xyz == 2)
		glRotatef(angle, 0, 1, 0);
	else
		glRotatef(angle, 0, 0, 1);
	glColor3f(1, 0.62, 0.25);      //��� RGB
	glutSolidCone(0.1, 0.4, 15, 15);
	glColor3f(0.54, 0.32, 0.1);
	glutWireCone(0.1, 0.4, 1, 15);
	glColor3f(1, 1, 1);
	glPopMatrix();
}

//�䳢 �Ӹ�
void rab_Head(GLfloat lx, GLfloat ly, GLfloat lz) {
	glTranslatef(lx, ly, lz);
	glPushMatrix();
	glScalef(1, 1.1, 0.5);
	glutSolidSphere(0.3, 30, 30);
	glutWireSphere(0.3, 30, 30);
	glPopMatrix();
}

//�䳢 ��
void rab_ear(GLfloat angle, GLfloat lx, GLfloat ly, GLfloat lz) {
	glPushMatrix();
	glTranslatef(lx, ly, lz);
	glRotatef(angle, 0, 0, 1);
	drawCircle(0.2, 0.6, 0.2);
	glColor3f(1, 0.8, 0.9);      //����ȫ RGB
	glTranslatef(0, 0.065, 0.09);
	drawCircle(0.1, 0.3, 0.05);
	glColor3f(1, 1, 1);
	glPopMatrix();
}

//�䳢 ��
void rab_eye(GLfloat lx, GLfloat ly, GLfloat lz) {

	if (counter <= 700) {
		glPushMatrix();
		glColor3f(0, 0, 0);
		glTranslatef(lx, ly, lz);
		drawCircle(0.1, 0.1, 0.01);
		glColor3f(1, 1, 1);
		glTranslatef(0.02, 0.02, 0);
		drawCircle(0.04, 0.04, 0.01);
		glPopMatrix();
	}
	else {
		glPushMatrix();
		glColor3f(0, 0, 0);
		glTranslatef(lx, ly, lz);
		glScalef(0.12, 0.05, 0.001);
		glutSolidCube(1);
		glPopMatrix();
	}
}

//�䳢 ��, �̻� �ڰ���
void rab_nose(GLfloat lx, GLfloat ly, GLfloat lz) {
	glPushMatrix();
	glColor3f(1, 0.5, 0.7);   //�䳢 ��ȫ�� RGB
	glTranslatef(lx, ly, lz);
	drawCircle(0.12, 0.09, 0.01);
	glColor3f(0, 0, 0);
	glTranslatef(-0.01, -0.06, 0);
	glPushMatrix();
	glScalef(0.025, 0.04, 0);
	glutWireCube(1);
	glPopMatrix();
	glTranslatef(0.02, 0, 0);
	glPushMatrix();
	glScalef(0.025, 0.04, 0);
	glutWireCube(1);
	glPopMatrix();

	if (counter >= 750) {
		glColor3f(0.29, 0.65, 0.84);
		glTranslatef(0.1, 0.1, 0);
		glTranslatef(sleep_nose, 0, 0);
		glRotatef(-90, 0, 0, 1);
		drawCircle(0.05, 0.15, 0.05);
		glTranslatef(0.012, 0.12, 0);
		glTranslatef(sleep_nose, 0, 0);
		drawCircle(0.05, 0.15, 0.05);
	}

	glColor3f(1, 1, 1);
	glPopMatrix();
}

//�ź��� ��ü
void tur_Body(GLfloat angle) {
	glRotatef(turn, 0, 1, 0);
	glPushMatrix();
	glColor3f(1, 0.9, 0.58);	//��ü ��� �Ϻ� RGB

	glTranslatef(0, 0.09, 0);
	drawCircle(0.8, 0.3, 0.9);
	glPopMatrix();
	glPushMatrix();
	glRotatef(32, 0, 1, 1);
	glTranslatef(0.15, 0.35, 0);
	if (i_hit == 1 && i_stat == 0)
		glColor3f(1, 0.19, 0.19);			//�������� �� �����  RGB
	else if (i_hit == 1 && i_stat == 1)
		glColor3f(0.74, 1, 0);			//�������� ��  RGB
	else
		glColor3f(0, 0.55, 0.38);	//����� �ʷ� RGB
	glScalef(0.55, 0.45, 0.55);
	glutSolidIcosahedron();
	glColor3f(0, 0, 0);
	glutWireIcosahedron();
	glPopMatrix();
}

//�ź��� ���� �ٸ�
void tur_leg(GLfloat angle, GLfloat lx, GLfloat ly, GLfloat lz) {
	glPushMatrix();
	glTranslatef(lx, ly, lz);
	//glRotatef(angle, 0, 1, 0);
	glRotatef(angle, 0, !(keystate[GLUT_KEY_UP]), keystate[GLUT_KEY_UP]);
	glRotatef(-40, 0, 1, 1);
	glColor3f(0.5, 0.75, 0.27);	//�ٸ�, �� �ʷ� RGB
	glTranslatef(lx, 0, 0);
	drawCircle(0.3, 0.15, 0.15);
	glTranslatef(0.1, 0, 0);
	glRotatef(180, 1, 0, 0);
	glutSolidCone(0.06, 0.4, 15, 15);
	glPopMatrix();
}

//�ź��� ������ �ٸ�
void tur_leg2(GLfloat angle, GLfloat lx, GLfloat ly, GLfloat lz) {
	glPushMatrix();
	glTranslatef(lx, ly, lz);
	//glRotatef(angle, 0, 1, 0);
	glRotatef(angle, 0, !(keystate[GLUT_KEY_UP]), keystate[GLUT_KEY_UP]);
	glRotatef(40, 0, 1, 1);
	glColor3f(0.5, 0.75, 0.27);
	glTranslatef(lx, 0, 0);
	drawCircle(0.3, 0.15, 0.15);
	glTranslatef(-0.1, 0, 0);
	glRotatef(-180, 1, 0, 0);
	glutSolidCone(0.06, 0.4, 15, 15);
	glPopMatrix();
}

//�ź��� ���� ����
void tur_eye(GLfloat lx, GLfloat ly, GLfloat lz) {
	glPushMatrix();
	glColor3f(0, 0, 0);
	glTranslatef(lx, ly, lz);
	drawCircle(0.1, 0.1, 0.01);
	glColor3f(1, 1, 1);
	glPushMatrix();
	glTranslatef(0.02, 0.02, 0);
	drawCircle(0.04, 0.04, 0.01);
	glPopMatrix();
	glColor3f(0, 0, 0);
	glTranslatef(0, 0.1, -0.05);
	glScalef(0.12, 0.05, 0.001);
	glutSolidCube(1);
	glPopMatrix();
}

//�ź��� �Ӹ�
void tur_Head(GLfloat angle, GLfloat lx, GLfloat ly, GLfloat lz) {
	glPushMatrix();
	glTranslatef(lx, ly, lz);
	glRotatef(angle, 0, 1, 1);
	glColor3f(0.5, 0.75, 0.27);
	glTranslatef(0, 0, lz);
	drawCircle(0.4, 0.5, 0.5);
	tur_eye(0.08, 0.08, 0.24);
	tur_eye(-0.08, 0.08, 0.24);
	glPopMatrix();
}

// �䳢 �𵨸�
void drawRabbit() {
	glTranslatef(rabbit->x, rabbit->y, rabbit->z);
	glPushMatrix();

	//glTranslatef(-1, 0, 0);
	rab_Body();
	glPushMatrix();
	rab_Head(0, 0.9, 0);
	rab_ear(-30, 0.2, 0.3, 0);
	rab_ear(30, -0.2, 0.3, 0);
	rab_eye(0.13, 0, 0.2);
	rab_eye(-0.13, 0, 0.2);
	rab_nose(0, -0.12, 0.2);
	glPopMatrix();



	glPushMatrix();
	rab_UpperLeg(rab_leg1, 0.2, -0.07, 0.0);
	rab_LowerLeg(rab_leg1, 0.18, 0, 0);
	rab_Foot(0.25, 0, 0.1);
	glPopMatrix();

	glPushMatrix();
	rab_UpperLeg2(rab_leg2, -0.2, -0.07, 0.0);
	rab_LowerLeg2(rab_leg2, 0.18, 0, 0);
	rab_Foot(0.25, 0, 0.1);
	glPopMatrix();

	glPushMatrix();
	rab_UpperArm(rab_leg1, 0.32, 0.35, 0.0);
	rab_LowerArm(-90, 0.18, 0, 0);
	rab_Hand(0.3, 0, 0);
	glPushMatrix();
	drawCarrot(-120, 1, 0.1, 0, 0);
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();
	rab_UpperArm2(rab_arm2, -0.32, 0.35, 0.0);
	rab_LowerArm2(-90, -0.18, 0, 0);
	rab_Hand(-0.3, 0, 0);
	glPopMatrix();
	glPopMatrix();

}

// �ź��� �𵨸�
void drawTurtle() {


	glPushMatrix();
	glTranslatef(turtle->x, turtle->y, turtle->z);
	tur_Body(turn);
	tur_leg2(tur_1leg, -0.2, 0.1, 0.2);
	tur_leg2(tur_1leg, -0.2, 0.1, -0.2);
	tur_leg(tur_2leg, 0.2, 0.1, 0.2);
	tur_leg(tur_2leg, 0.2, 0.1, -0.2);
	tur_Head(0, 0, 0.25, 0.25);
	glPopMatrix();
	glColor3f(1, 1, 1);
}

//���� �𵨸�
void drawTree() {
	for (int a = 0; a < 200; a += 5) {
		glPushMatrix();
		if (a % 2 == 0)
			glTranslatef(-10, 0, tmpz3 + a);
		else
			glTranslatef(6, 0, tmpz3 + a);
		glTranslatef(0, 0.6, 0);
		glColor3f(0.54, 0.32, 0.1);		//���� ���� RGB
		glPushMatrix();
		glScalef(0.5, 3, 0.5);
		glutSolidCube(1);
		glPopMatrix();
		glColor3f(0, 0.8, 0);			//������ �ʷϻ� RGB
		glPushMatrix();
		glTranslatef(0, 0.2, 0);
		glRotatef(90, 1, 0, 0);
		for (float i = 0.5; i > 0.3; i -= 0.05) {
			glutSolidTorus(i, i, 15, 15);
			glTranslatef(0, 0, -0.4);
		}

		glPopMatrix();
		glPopMatrix();
	}
}

//�ؽ��� ��ȣ�� �μ��� �޾� �簢���� �׸��� �Լ�
void drawQuad(int idx) {
	GLfloat v[4][3] =
	{
		{1.0,1.0,0.0},
		{-1.0,1.0,0.0},
		{-1.0,-1.0,0.0},
		{1.0,-1.0,0.0}
	};

	GLfloat n[3] = { 0.0,0.0,-1.0 };

	glBindTexture(GL_TEXTURE_2D, tex_ids[idx]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[idx].cols, image[idx].rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image[idx].data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// ��Ƽ���ϸ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// ��Ƽ���ϸ����
	glColor3f(1, 1, 1);


	glBegin(GL_QUADS);
	glNormal3fv(n);
	glTexCoord2f(1.0, 1.0);
	glVertex3fv(v[0]);
	glTexCoord2f(0.0, 1.0);
	glVertex3fv(v[1]);
	glTexCoord2f(0.0, 0.0);
	glVertex3fv(v[2]);
	glTexCoord2f(1.0, 0.0);
	glVertex3fv(v[3]);
	glEnd();

}

// �ٴ� ȸ�� ������ ���
GLfloat ground_r = -90;


// �ٴ� �׸���
void drawGround() {
	glPushMatrix();
	glTranslatef(0, -1.1, -1);
	for (int i = 0; i < 2; i++) {
		glPushMatrix();
		glTranslatef(16, -0.1, tmpz + i * 100);
		glRotatef(ground_r, 1, 0, 0);
		glScalef(10, 50, 0);
		drawQuad(7);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-16, -0.1, tmpz + i * 100);
		glRotatef(ground_r, 1, 0, 0);
		glScalef(7, 50, 0);
		drawQuad(7);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-2, 0.2, tmpz + i * 100);
		glRotatef(ground_r, 1, 0, 0);
		glScalef(8, 50, 0);
		drawQuad(6);
		glPopMatrix();
		ground_r *= -1;
	}
	glPopMatrix();

}

//CModel�� ������Ʈ�� �׸��� �Լ�
void drawObject(CModel obj, int idx) {
	GLfloat x, y, z, nx, ny, nz, tx, ty;
	int v_id, vt_id, vn_id;

	for (int o = 0; o < obj.objs.size(); o++) {

		glBindTexture(GL_TEXTURE_2D, tex_ids[idx]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[idx].cols, image[idx].rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image[idx].data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// ��Ƽ���ϸ����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// ��Ƽ���ϸ����

		int nFaces = obj.objs[o].f.size();
		for (int k = 0; k < nFaces; k++) {
			int nPoints = obj.objs[o].f[k].v_pairs.size();
			glBegin(GL_POLYGON);
			for (int i = 0; i < nPoints; i++) {
				v_id = obj.objs[o].f[k].v_pairs[i].d[0];
				vt_id = obj.objs[o].f[k].v_pairs[i].d[1];
				vn_id = obj.objs[o].f[k].v_pairs[i].d[2];
				x = obj.objs[o].v[v_id - 1].d[0];
				y = obj.objs[o].v[v_id - 1].d[1];
				z = obj.objs[o].v[v_id - 1].d[2];

				nx = obj.objs[o].vn[vn_id - 1].d[0];
				ny = obj.objs[o].vn[vn_id - 1].d[1];
				nz = obj.objs[o].vn[vn_id - 1].d[2];

				tx = obj.objs[o].vt[vt_id - 1].d[0];
				ty = obj.objs[o].vt[vt_id - 1].d[1];

				glNormal3f(nx, ny, nz);
				glTexCoord2f(tx, ty);
				glVertex3f(x, y, z);
			}
			glEnd();
		}
	}
}


GLfloat hur_x = 0;
GLfloat sto1_x = 0;
GLfloat sto2_x = 0;
GLfloat sto3_x = 0;
GLfloat sto_z = 0;

//����� �׸��� �Լ�
void drawHurdle() {

	init(hurdle, hur_x, -0.3, tmpz + 50);
	drawHb(hurdle, 1.4, 0.4, 0.1);
	glPushMatrix();
	glColor3f(0.54, 0.3, 0.2);
	glRotatef(90, 0, 1, 0);
	glTranslatef(-tmpz - 50, -1, hur_x);
	glScalef(0.5, 0.5, 0.8);
	drawObject(m, 0);
	glPopMatrix();

}



// ���� �ʱ�ȭ
void initCoin() {
	for (int i = 1; i < 50; i++) {
		coin_x[i] = coin_x[i - 1] + dir_coin;
		if (coin_x[i] <= -2)
			dir_coin = 0.5;
		if (coin_x[i] >= 2)
			dir_coin = -0.5;
	}
}

//������ �׸��� �Լ�
void drawCoin() {
	for (int i = 0; i < 50; i++) {
		if (coin_count[i] == 0) {
			init(&coin[i], coin_x[i], -0.4, tmpz2 - 3 + i * 3);
			drawHb(&coin[i], 0.3, 0.3, 0.3);
			glPushMatrix();
			glTranslatef(coin_x[i], -0.4, tmpz2 - 3 + i * 3);
			glScalef(1, 1, 0.3);
			glColor3f(1, 0.62, 0.25);
			glutSolidSphere(0.3, 20, 20);
			glPopMatrix();
		}
	}
}

//�ڽ��� �׸��� �Լ�
void drawBox() {
	GLfloat v1[3] = { -1,-1,1 };
	GLfloat v2[3] = { -1,1,1 };
	GLfloat v3[3] = { 1,1,1 };
	GLfloat v4[3] = { 1,-1,1 };
	GLfloat v5[3] = { -1,-1,-1 };
	GLfloat v6[3] = { -1,1,-1 };
	GLfloat v7[3] = { 1,1,-1 };
	GLfloat v8[3] = { 1,-1,-1 };

	//�ո�
	glBegin(GL_POLYGON);
	glNormal3f(0, 0, 1);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(v1[0], v1[1], v1[2]);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(v2[0], v2[1], v2[2]);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(v3[0], v3[1], v3[2]);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(v4[0], v4[1], v4[2]);
	glEnd();

	//�޸�
	glBegin(GL_POLYGON);
	glNormal3f(0, 0, -1);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(v8[0], v8[1], v8[2]);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(v7[0], v7[1], v7[2]);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(v6[0], v6[1], v6[2]);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(v5[0], v5[1], v5[2]);
	glEnd();

	//��
	glBegin(GL_POLYGON);
	glNormal3f(0, 1, 0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(v6[0], v6[1], v6[2]);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(v2[0], v2[1], v2[2]);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(v3[0], v3[1], v3[2]);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(v7[0], v7[1], v7[2]);
	glEnd();

	//�Ʒ�
	glBegin(GL_POLYGON);
	glNormal3f(0, -1, 0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(v5[0], v5[1], v5[2]);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(v1[0], v1[1], v1[2]);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(v4[0], v4[1], v4[2]);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(v8[0], -v8[1], v8[2]);
	glEnd();

	// ������
	glBegin(GL_POLYGON);
	glNormal3f(1, 0, 0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(v4[0], v4[1], v4[2]);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(v3[0], v3[1], v3[2]);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(v7[0], v7[1], v7[2]);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(v8[0], v8[1], v8[2]);
	glEnd();

	//����
	glBegin(GL_POLYGON);
	glNormal3f(-1, 0, 0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(v5[0], v5[1], v5[2]);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(v6[0], v6[1], v6[2]);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(v2[0], v2[1], v2[2]);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(v1[0], v1[1], v1[2]);
	glEnd();
}


//���� �������� �׸��� �ڽ�
void drawItemBox() {

	glBindTexture(GL_TEXTURE_2D, tex_ids[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[1].cols, image[1].rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image[1].data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// ��Ƽ���ϸ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// ��Ƽ���ϸ����

	init(i_box, hur_x, 1, tmpz + 50);
	drawHb(i_box, 0.7, 0.7, 0.7);

	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(hur_x, 1, tmpz + 50);
	glRotatef(45, 0, 0, 1);
	glScalef(0.5, 0.5, 0.5);
	drawBox();
	glPopMatrix();
}

//��ֹ� ������ �׸��� �Լ�
void drawStone() {
	init(stone1, sto1_x, -0.3, tmpz3 + 30);
	drawHb(stone1, 1.3, 1, 1.4);
	init(stone2, sto2_x, -0.3, tmpz3 + 60);
	drawHb(stone2, 1.3, 1, 1);
	init(stone3, sto3_x, -0.3, tmpz3 + 90);
	drawHb(stone3, 1.3, 1, 1);
	glPushMatrix();
	glColor3f(0.69, 0.69, 0.69);
	glRotatef(90, 0, 1, 0);
	glPushMatrix();
	glTranslatef(-tmpz3 - 30, -0.3, sto1_x);
	glScalef(1.2, 1, 1.3);
	drawObject(stone, 2);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-tmpz3 - 60, -0.3, sto2_x);
	glScalef(1.6, 1, 1.2);
	drawObject(stone, 2);
	glPopMatrix();
	glTranslatef(-tmpz3 - 90, -0.3, sto3_x);
	glScalef(1.2, 1.3, 1.4);
	drawObject(stone, 2);
	glPopMatrix();
}

//�����̸� �׸��� �Լ�
void drawHole() {
	init(hole, 0, -0.8, tmpz3 + 40);
	drawHb(hole, 0.5, 0.1, 0.5);
	glPushMatrix();
	glTranslatef(0, -0.8, tmpz3 + 40);
	drawObject(h, 8);
	glPopMatrix();
}


//����� �׸��� �Լ�
int daycount = 3;
void drawBack() {
	switch (sight) {
	case 1:		//�⺻
		glPushMatrix();
		glTranslatef(0, 15, camz + 100);
		if (daycount == 11)
			glScalef(130, 40, 0);
		else
			glScalef(160, 40, 0);
		drawQuad(daycount);
		glPopMatrix();
		break;
	case 2:		//�ź��� ������ �� ��
		glPushMatrix();
		glRotatef(90, 0, 1, 0);
		glTranslatef(-camz, 10, 10);
		glScalef(130, 25, 0);
		drawQuad(daycount);
		glPopMatrix();
		break;
	case 3:		//�ź��� �տ��� �� ��
		glPushMatrix();
		glRotatef(180, 0, 1, 0);
		glTranslatef(0, 10, -camz + 100);
		if (daycount == 11)
			glScalef(130, 40, 0);
		else
			glScalef(160, 40, 0);
		drawQuad(daycount);
		glPopMatrix();
		break;
	}

}

//���� ��ġ�� ����
GLfloat bird_x = 0;
GLfloat bird_z = 0;
GLfloat speed_bird = 0.8;
int bird_t = 8;


//���� �׸��� �Լ�
void drawBird() {
	bird_z += speed_bird;
	glPushMatrix();
	glTranslatef(bird_x, 1.5, tmpz + 90 - bird_z);
	drawObject(bird, bird_t);
	glPopMatrix();
}

void drawFinal() {
	glBindTexture(GL_TEXTURE_2D, tex_ids[10]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[10].cols, image[10].rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image[10].data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// ��Ƽ���ϸ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// ��Ƽ���ϸ����
	glDisable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	glPushMatrix();
	glTranslatef(7, 1.5, 315);
	glScalef(1, 2.5, 0.5);
	drawBox();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-11, 1.5, 315);
	glScalef(1, 2.5, 0.5);
	drawBox();
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslatef(-2, 4.8, 315);
	glScalef(10, 0.8, 0.5);
	drawBox();
	glPopMatrix();
}


void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camx, camy, camz, cam2x, cam2y, cam2z, cam_upx, cam_upy, cam_upz);

	glColor3f(1, 1, 1);

	glPushMatrix();
	drawRabbit();		//�䳢 �𵨸�
	glPopMatrix();
	glPushMatrix();
	drawTurtle();		//�ź��� �𵨸�
	glPopMatrix();
	glPushMatrix();
	drawTree();			//���� �𵨸�
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, 0.4, 0);
	drawHb(turtle, 0.5, 0.35, 0.5);
	drawHb(rabbit, 0.5, 1, 0.3);
	glPopMatrix();

	glColor3f(1, 1, 1);
	drawCoin();

	glEnable(GL_TEXTURE_2D);

	drawGround();				//�ٴ� �𵨸�
	drawHurdle();				//���
	drawStone();				//����
	if (i_hit == 0 || tmpz + 50 > turtle->z)
		drawItemBox();				//�����۹ڽ�
	drawBack();					//���
	drawBird();					//��
	drawHole();					//������
	if (turtle->z >= 200)
		drawFinal();			//�����

	glDisable(GL_TEXTURE_2D);
	glutSwapBuffers();

}


void End_Scene() {
	daycount = 11;
}


void print_Over() {
	printf("\n\n\n");
	printf("         #####      #      #     #  #####         ####    #      #   #####    #####      \n");
	printf("        #          # #     ##   ##  #            #    #   #      #   #        #   #      \n");
	printf("       #    ##    #   #    # # # #  #####        #    #    #    #    #####    ####       \n");
	printf("        #    #   # # # #   #  #  #  #            #    #     #  #     #        #   #      \n");
	printf("         ####   #       #  #     #  #####         ####       ##      #####    #    #     \n");
	printf("\n\n");

	printf("* Distance(m)      : %.2fM\n\n", turtle->z);
	printf("* Survival time(s) : %.2f(s)\n", end_Time);
	printf("\n\n");
}

void print_Victory() {
	printf("\n\n\n");
	printf("             #      #   #####    ####    #####   ####    #####   #   #              \n");
	printf("             #      #     #     #    #     #    #    #   #   #    # #          \n");
	printf("              #    #      #    #           #    #    #   ####      #                    \n");
	printf("               #  #       #     #    #     #    #    #   #   #     #             \n");
	printf("                ##      #####    ####      #     ####    #    #    #                 \n");
	printf("\n\n");

	printf("* Arrival time(s) : %.2f(s)\n", end_Time);
	printf("*      Coin       : %d", score);
	printf("\n\n");
}


void End_fun() {
	print_Over();
	free(turtle);
	free(rabbit);
	free(hurdle);
	free(coin);
	free(stone1);
	free(stone2);
	free(stone3);
	free(i_box);
	free(hole);
	exit(0);
}

int end_jump = 0;
int sound_jump = 0;

void jump() {
	if (t_hit != 1) {
		if (sound_jump == 0)
			PlaySound(TEXT(JUMP), NULL, SND_ASYNC);	//BGM�� ����ϱ� ���� �Լ�
		sound_jump = 1;
		if (turtle->y >= 1.65)
			jump_stat = 1;

		if (jump_stat == 1 && turtle->y < -0.65)
			jump_stat = 2;

		switch (jump_stat) {
		case 0: turtle->y += 0.2;
			break;
		case 1: turtle->y -= 0.2;
			break;
		case 2: keystate[GLUT_KEY_UP] = false;
			jump_stat = 0;
			sound_jump = 0;
			break;
		}
	}

}



//���ο� �ε������� �߻��ϴ� �̺�Ʈ
void hit_Coin(int c) {
	if (coin_count[c] == 0) {
		score++;
		coin_count[c] = 1;
		PlaySound(TEXT(COIN), NULL, SND_ASYNC);	//BGM�� ����ϱ� ���� �Լ�
	}
}


//��鿡 �ε������� �߻��ϴ� �̺�Ʈ
void hit_bar() {
	speed_t = 0;
	speed_r = 0;
	dir_tur_leg = 0;
	turn += 7;
	camz -= 0.005;

	keystate[GLUT_KEY_RIGHT] = false;
	keystate[GLUT_KEY_LEFT] = false;


	if (t_hit == 0) {
		PlaySound(TEXT(CRASH1), NULL, SND_ASYNC);	//��� �ε����� sound
		t_hit++;
		keystate[GLUT_KEY_F1] = true;
		tmptimer = counter;
		end_Time = (int)clock() / 1000.0;
	}

	if (tmptimer + 30 == counter) {
		PlaySound(TEXT(OVER), NULL, SND_ASYNC);		//Game Over sound
		End_Scene();
	}

	if (t_hit == 1 && (tmptimer + 80) == counter) {
		End_fun();
	}

}

//������ �ε������� �߻��ϴ� �̺�Ʈ
void hit_stone() {
	speed_t = 0;
	dir_tur_leg = 0;
	speed_r = 0;
	turn += 7;
	camz -= 0.005;
	if (s_hit == 1) {
		end_Time = (int)clock() / 1000.0;
		sound_jump = 1;
		keystate[GLUT_KEY_F1] = true;
		PlaySound(TEXT(CRASH3), NULL, SND_ASYNC);	//���� �ε�ġ�� sound
		s_hit++;
	}
	keystate[GLUT_KEY_RIGHT] = false;
	keystate[GLUT_KEY_LEFT] = false;

	if (tmptimer + 30 == counter) {
		PlaySound(TEXT(OVER), NULL, SND_ASYNC);		//Game Over sound
		End_Scene();
	}

	if ((tmptimer + 80) == counter) {
		End_fun();
	}
}



void hit_hole() {
	speed_t = 0;
	speed_r = 0;
	dir_tur_leg = 0;
	turn += 7;
	camz -= 0.005;
	if (h_hit == 1) {
		end_Time = (int)clock() / 1000.0;
		sound_jump = 1;
		keystate[GLUT_KEY_F1] = true;
		PlaySound(TEXT(CRASH2), NULL, SND_ASYNC);	//������ ������ �� sound
		h_hit++;
	}
	else if (keystate[GLUT_KEY_F1] == false)
		turtle->y -= 0.05;

	keystate[GLUT_KEY_RIGHT] = false;
	keystate[GLUT_KEY_LEFT] = false;

	if (tmptimer + 50 == counter) {
		PlaySound(TEXT(OVER), NULL, SND_ASYNC);		//Game Over sound
		End_Scene();
	}

	if ((tmptimer + 90) == counter) {
		End_fun();
	}
}

void Sleep_rab() {
	speed_r = 0;
	dir_rab_arm = 0;
	dir_rab_leg = 0;
	sleep_down = 2;

	if (rab_sleep < 90) {
		rab_sleep += sleep_down;
		if (rab_sleep > 45)
			rab_Ty -= 0.004;

	}
}

void Wakeup_rab() {
	if (rab_sleep >= 0) {
		rab_sleep -= sleep_down;
		if (rab_sleep > 45)
			rab_Ty += 0.004;
	}
	if (wakeup_r == 0) {
		speed_r = 0.4;
		dir_rab_arm = 2;
		dir_rab_leg = 2;
		wakeup_r++;
	}
}

//flag
int branch1 = 0;
int branch2 = 0;
int branch3 = 0;
int last_jump = 0;
int victory_bgm = 0;
int end_flag = 0;

void End_Game(int winner) {		//0�� �ź� 1�� �䳢
	speed_t = 0;
	speed_r = 0;
	keystate[GLUT_KEY_F2] = true;
	i_hit = 0;

	if (end_flag == 0) {
		end_Time = (int)clock() / 1000.0;
		tmptimer = counter;
		end_flag++;
	}

	if (winner == 0) {
		Sleep_rab();
		turn += 20;
		if (last_jump == 0) {
			keystate[GLUT_KEY_UP] = true;
			last_jump++;
		}
		if (keystate[GLUT_KEY_UP] == false && victory_bgm == 0) {
			PlaySound(TEXT(VIC), NULL, SND_ASYNC || SND_LOOP);
			victory_bgm++;
		}
		if ((tmptimer + 100) == counter) {
			print_Victory();
			free(turtle);
			free(rabbit);
			free(hurdle);
			free(coin);
			free(stone1);
			free(stone2);
			free(stone3);
			free(i_box);
			free(hole);
			exit(0);
		}
	}

	if (winner == 1) {
		if (victory_bgm == 0) {
			PlaySound(TEXT(LOSE), NULL, SND_ASYNC);
			victory_bgm++;

		}
		if (tmptimer + 150 == counter) {
			PlaySound(TEXT(OVER), NULL, SND_ASYNC);

		}
		else if (tmptimer + 150 <= counter)
		{
			End_Scene();
		}
		if ((tmptimer + 190) == counter) {
			End_fun();
		}
	}


	keystate[GLUT_KEY_RIGHT] = false;
	keystate[GLUT_KEY_LEFT] = false;



}

void idle() {
	counter += 1;

	//�䳢�� �ź� �����̴� ��� ����
	rab_arm1 += dir_rab_arm;
	rab_arm2 += dir_rab_arm;
	rab_leg1 += dir_rab_leg;
	rab_leg2 += dir_rab_leg;
	tur_1leg += dir_tur_leg;
	tur_2leg += dir_tur_leg;

	//�䳢�� �ź� ī�޶� ���� �ӵ���� z�� ����
	turtle->z += speed_t;
	rabbit->z += speed_r;
	camz += speed_t;
	cam2z += speed_t;


	if (rab_arm1 >= -10)
		dir_rab_arm = -2;
	else if (rab_arm1 <= -110)
		dir_rab_arm = 2;

	if (rab_leg1 >= 10)
		dir_rab_leg = -2;
	else if (rab_leg1 <= -90)
		dir_rab_leg = 2;

	if (tur_1leg >= -20)
		dir_tur_leg = -1;
	else if (tur_1leg <= -60)
		dir_tur_leg = 1;


	//����� �ؽ��ĸ� ����
	if (counter % 500 == 0) {
		if (daycount == 3)
			daycount = 4;
		else if (daycount == 4)
			daycount = 5;
		else
			daycount = 3;
	}

	//�ź��� ����
	if (keystate[GLUT_KEY_UP] == true)
		jump();

	//�ź��� �¿� ������
	if (keystate[GLUT_KEY_RIGHT] == true && keystate[GLUT_KEY_UP] == false) {
		if (turtle->x >= -2)
			turtle->x -= 0.1;
	}
	if (keystate[GLUT_KEY_LEFT] == true && keystate[GLUT_KEY_UP] == false) {
		if (turtle->x <= 2)
			turtle->x += 0.1;
	}

	//�䳢�� ���� ����� ����
	if (rabbit->z >= 150 && turtle->z < 220)
		Sleep_rab();

	if (turtle->z >= 220)
		Wakeup_rab();


	//�ź��̰� ���� ��¼��� �������� ��
	if (turtle->z >= 320) {
		End_Game(0);
	}
	//�䳢�� ���� ��¼��� �������� ��
	if (rabbit->z >= 320) {
		End_Game(1);
	}

	/* ȭ���� �ٽ� �׸��� �б��� */
	//�ź����� z�� ��ǥ�� 60�� �� ����� ��ġ�� ���� tmpz
	if ((int)turtle->z % 60 == 0 && branch1 == 0) {
		if (branch1 == 0) {
			tmpz = turtle->z;
			hur_x = ((rand() & 400) / 100.0) - 2;
			bird_x = ((rand() & 800) / 100.0) - 6;
			bird_z = 0;
			if ((rand() & 2) == 0)
				bird_t = 8;
			else
				bird_t = 9;
			branch1++;

		}
	}
	else
		branch1 = 0;

	//�ź����� z�� ��ǥ�� 90�� �� ������ �ٽ� ��ġ tmpz2
	if ((int)turtle->z % 90 == 0 && (int)turtle->z < 270) {
		if (branch2 == 0) {
			tmpz2 = turtle->z;
			//���� ȹ���� �Ǻ��ϴ� �迭 Reset
			for (int i = 0; i < 50; i++) {
				coin_count[i] = 0;
			}
			branch2++;
		}
	}
	else
		branch2 = 0;

	//�ź����� z�� ��ǥ�� 110�� �� ������ �ٽ� ��ġ tmpz3
	if ((int)turtle->z % 110 == 0) {
		if (branch3 == 0) {
			tmpz3 = turtle->z;
			sto1_x = ((rand() & 400) / 100.0) - 2;
			sto2_x = ((rand() & 400) / 100.0) - 2;
			sto3_x = ((rand() & 400) / 100.0) - 2;
			branch3++;
		}
	}
	else
		branch3 = 0;



	//F1 �⺻����
	if (keystate[GLUT_KEY_F1] == true) {
		camz = turtle->z - 4;
		camy = 2;
		camx = 0;
		sight = 1;
		keystate[GLUT_KEY_F1] = false;

	}

	//F2 �տ��� �ڷκ��� ����
	if (keystate[GLUT_KEY_F2] == true) {
		camz = turtle->z + 4;
		camy = 2;
		camx = 0;
		sight = 3;
		keystate[GLUT_KEY_F2] = false;
	}

	//F3 �ź��� ������ ���� ����
	if (keystate[GLUT_KEY_F3] == true) {
		camz = turtle->z;
		camx = -4;
		camy = 0;
		sight = 2;
		keystate[GLUT_KEY_F3] = false;
	}

	//F5������ ������ ��Ʈ�ڽ� �׸��� f_hb = 1
	if (keystate[GLUT_KEY_F5] == true) {
		f_hb = 1;
	}

	if (keystate[GLUT_KEY_F5] == false) {
		f_hb = 0;
	}


	//���α׷� ����
	if (keystate[GLUT_KEY_DOWN] == true) {
		End_fun();
	}


	if (turtle->z <= tmpz + 60 && turtle->z >= tmpz + 40) {
		if (i_hit == 0) {
			//��� �浹����
			if (turtle->hx1 >= hurdle->hx2 && turtle->hx2 <= hurdle->hx1)
				if (turtle->hy1 >= hurdle->hy2 && turtle->hy2 <= hurdle->hy1)
					if (turtle->hz1 >= hurdle->hz2 && turtle->hz2 <= hurdle->hz1)
						hit_bar();

			//������ �ڽ� �浹����
			if (turtle->hx1 >= i_box->hx2 && turtle->hx2 <= i_box->hx1)
				if (turtle->hy1 >= i_box->hy2 && turtle->hy2 <= i_box->hy1)
					if (turtle->hz1 >= i_box->hz2 && turtle->hz2 <= i_box->hz1) {
						i_hit++;
						tmptimer = counter;
						i_stat = rand() % 2;

						if (i_stat == 0)
							PlaySound(TEXT(ITEM1), NULL, SND_ASYNC);	//BGM�� ����ϱ� ���� �Լ�
						if (i_stat == 1)
							PlaySound(TEXT(ITEM2), NULL, SND_ASYNC);	//BGM�� ����ϱ� ���� �Լ�
					}
		}
	}

	//���� �����۹ڽ��� ����� �� 0�̸� ������ 1�϶� ������
	if (i_hit == 1) {
		switch (i_stat) {
		case 0:
			if (tmptimer + 150 >= counter) {
				speed_t = 0.35;
			}
			else {
				speed_t = 0.24;
				i_hit = 0;
			}
			break;
		case 1:
			if (tmptimer + 100 >= counter) {
				speed_t = 0.05;
			}
			else {
				speed_t = 0.24;
				i_hit = 0;
			}
			break;
		}
	}

	//����, ������ �浹����
	if (i_hit == 0) {
		if (s_hit == 0 && h_hit == 0) {
			if (turtle->z <= tmpz3 + 50 && turtle->z >= tmpz3 + 20) {

				if (turtle->hx1 >= stone1->hx2 && turtle->hx2 <= stone1->hx1)
					if (turtle->hy1 >= stone1->hy2 && turtle->hy2 <= stone1->hy1)
						if (turtle->hz1 >= stone1->hz2 && turtle->hz2 <= stone1->hz1) {
							s_hit = 1;
							tmptimer = counter;
							keystate[GLUT_KEY_UP] = true;
						}
				//������ �浹 ����
				if (turtle->hx1 >= hole->hx2 && turtle->hx2 <= hole->hx1)
					if (turtle->y <= -0.8)
						if (turtle->hz1 >= hole->hz2 && turtle->hz2 <= hole->hz1) {
							h_hit = 1;
							tmptimer = counter;
							keystate[GLUT_KEY_UP] = true;
						}
			}

			else if (turtle->z <= tmpz3 + 70 && turtle->z >= tmpz3 + 50) {
				if (turtle->hx1 >= stone2->hx2 && turtle->hx2 <= stone2->hx1)
					if (turtle->hy1 >= stone2->hy2 && turtle->hy2 <= stone2->hy1)
						if (turtle->hz1 >= stone2->hz2 && turtle->hz2 <= stone2->hz1) {
							s_hit = 1;
							tmptimer = counter;
							keystate[GLUT_KEY_UP] = true;
						}
			}
			else if (turtle->z <= tmpz3 + 100 && turtle->z >= tmpz3 + 80) {
				if (turtle->hx1 >= stone3->hx2 && turtle->hx2 <= stone3->hx1)
					if (turtle->hy1 >= stone3->hy2 && turtle->hy2 <= stone3->hy1)
						if (turtle->hz1 >= stone3->hz2 && turtle->hz2 <= stone3->hz1) {
							s_hit = 1;
							tmptimer = counter;
							keystate[GLUT_KEY_UP] = true;
						}
			}
		}
		//������ �ε������� �̺�Ʈ �߻�
		if (s_hit >= 1) {
			hit_stone();
		}
		//���ۿ� �������� �̺�Ʈ �߻�
		if (h_hit >= 1) {
			hit_hole();
		}
	}

	//���� �浹����
	int i = 0;
	while (i < 50) {
		if (turtle->hx1 >= coin[i].hx2 && turtle->hx2 <= coin[i].hx1)
			if (turtle->hy1 >= coin[i].hy2 && turtle->hy2 <= coin[i].hy1)
				if (turtle->hz1 >= coin[i].hz2 && turtle->hz2 <= coin[i].hz1)
					hit_Coin(i);
		i++;
	}

	glutPostRedisplay();
}

void S_key_down(int KeyPressed, int x, int y) {


	if (KeyPressed == GLUT_KEY_LEFT)
	{
		keystate[GLUT_KEY_LEFT] = true;
	}
	if (KeyPressed == GLUT_KEY_RIGHT)
	{
		keystate[GLUT_KEY_RIGHT] = true;
	}
	if (KeyPressed == GLUT_KEY_UP)
	{
		keystate[GLUT_KEY_UP] = true;
	}
	if (KeyPressed == GLUT_KEY_DOWN)
	{
		keystate[GLUT_KEY_DOWN] = true;
	}
	if (KeyPressed == GLUT_KEY_F1)
	{
		keystate[GLUT_KEY_F1] = true;
	}
	if (KeyPressed == GLUT_KEY_F2)
	{
		keystate[GLUT_KEY_F2] = true;
	}
	if (KeyPressed == GLUT_KEY_F3)
	{
		keystate[GLUT_KEY_F3] = true;
	}

	if (KeyPressed == GLUT_KEY_F5)
	{
		keystate[GLUT_KEY_F5] = true;
	}

	if (KeyPressed == GLUT_KEY_F6)
	{
		keystate[GLUT_KEY_F6] = true;
	}

	if (KeyPressed == GLUT_KEY_DOWN)
	{
		keystate[GLUT_KEY_DOWN] = true;
	}

}

void S_key_up(int KeyPressed, int x, int y) {

	if (KeyPressed == GLUT_KEY_LEFT)
	{
		keystate[GLUT_KEY_LEFT] = false;
	}
	if (KeyPressed == GLUT_KEY_RIGHT)
	{
		keystate[GLUT_KEY_RIGHT] = false;
	}

	if (KeyPressed == GLUT_KEY_F5)
	{
		keystate[GLUT_KEY_F5] = false;
	}

	if (KeyPressed == GLUT_KEY_F6)
	{
		keystate[GLUT_KEY_F6] = false;
	}
}

void keyboard(unsigned char Key, int X, int Y) {
	switch (Key) {
	case 32:
		keystate[GLUT_KEY_UP] = true;
		break;
	case 'q':
		End_fun(); break;
	}
}

void init_light()
{
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_SMOOTH);
	glEnable(GL_LIGHTING);

	GLfloat ambientLight[] = { 0.3f,0.3f,0.3f,1.0f };
	GLfloat diffuseLight[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat specular[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat specref[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat position[] = { 0,5,camz,1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMateriali(GL_FRONT, GL_SHININESS, 128);
}

void init_texture() {
	glGenTextures(nTex, tex_ids);
	glEnable(GL_TEXTURE_2D);
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);	//�ִϸ��̼�
	glutInitWindowSize(_WINDOW_WIDTH, _WINDOW_HEIGHT);
	glutCreateWindow("FINAL_GAME");
	init_light();
	init_texture();

	if (flag == 0) {
		//�� obj �о����
		string filepath = "model/hurdle.obj";
		ifstream fin(filepath);
		m.loadObj(fin);
		fin.close();


		ifstream fin2("model/stone.obj");
		stone.loadObj(fin2);
		fin2.close();

		ifstream fin3("model/bird.obj");
		bird.loadObj(fin3);
		fin3.close();


		ifstream fin4("model/hole.obj");
		h.loadObj(fin4);
		fin4.close();

		//�ؽ��� �̹��� �о����
		image[0] = imread("images\\bar3.jpg", IMREAD_COLOR);
		cvtColor(image[0], image[0], COLOR_BGR2RGB);

		image[1] = imread("images\\item.jpg", IMREAD_COLOR);
		cvtColor(image[1], image[1], COLOR_BGR2RGB);

		image[2] = imread("images\\stone.jpg", IMREAD_COLOR);
		cvtColor(image[2], image[2], COLOR_BGR2RGB);

		image[3] = imread("images\\day.jpg", IMREAD_COLOR);
		cvtColor(image[3], image[3], COLOR_BGR2RGB);

		image[4] = imread("images\\sunset.jpg", IMREAD_COLOR);
		cvtColor(image[4], image[4], COLOR_BGR2RGB);

		image[5] = imread("images\\night.jpg", IMREAD_COLOR);
		cvtColor(image[5], image[5], COLOR_BGR2RGB);

		image[6] = imread("images\\road.jpg", IMREAD_COLOR);
		cvtColor(image[6], image[6], COLOR_BGR2RGB);

		image[7] = imread("images\\green.jpg", IMREAD_COLOR);
		cvtColor(image[7], image[7], COLOR_BGR2RGB);

		image[8] = imread("images\\black.jpg", IMREAD_COLOR);
		cvtColor(image[8], image[8], COLOR_BGR2RGB);

		image[9] = imread("images\\white.jpg", IMREAD_COLOR);
		cvtColor(image[9], image[9], COLOR_BGR2RGB);

		image[10] = imread("images\\final.jpg", IMREAD_COLOR);
		cvtColor(image[10], image[10], COLOR_BGR2RGB);

		image[11] = imread("images\\gameover.jpg", IMREAD_COLOR);
		cvtColor(image[11], image[11], COLOR_BGR2RGB);


		turtle = (position*)malloc(sizeof(position));
		rabbit = (position*)malloc(sizeof(position));
		hurdle = (position*)malloc(sizeof(position));
		coin = (position*)malloc(sizeof(position) * 50);
		stone1 = (position*)malloc(sizeof(position));
		stone2 = (position*)malloc(sizeof(position));
		stone3 = (position*)malloc(sizeof(position));
		i_box = (position*)malloc(sizeof(position));
		hole = (position*)malloc(sizeof(position));
		init(turtle, 1, -0.85, 0);
		init(rabbit, -5, 0, 0);
		init(hurdle, 0, 0, 0);
		init(stone1, 0, 0, 0);
		init(stone2, 0, 0, 0);
		init(stone3, 0, 0, 0);
		init(i_box, 0, 0, 0);
		init(hole, 0, 0, 0);
		initCoin();

		srand((unsigned)time(NULL));

		flag++;
	}

	glEnable(GL_DEPTH_TEST);	//���� ���� ���
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIgnoreKeyRepeat(1);
	glutIdleFunc(idle);
	glutSpecialFunc(S_key_down);
	glutSpecialUpFunc(S_key_up);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	End_fun();

	return 0;
}