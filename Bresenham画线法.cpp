#include <cmath>
#include <gl\glut.h>
using namespace std;

/*ȷ��һ����*/
void set_pixel(int x, int y)
{
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}

/* Bresenham�����㷨���岿��*/
void Bresenham_line(int x0, int y0, int x1, int y1)
{
	int x, y, dx, dy, e;
	int i, temp;
	int change = 0;			// ���ֱ��б��<=1, change=0; ����change=1
	int Step_X = 1, Step_Y = 1;			//ȡֵΪ1 �� -1 ,��ʾ���������� x�����y����仯�ķ���
	dx = x1 - x0, dy = y1 - y0;

	if (dx<0) 
	{
		dx = -dx;  //dxʼ��Ϊ��
		Step_X = -1;//�ı�x�ĵ�������Ϊ��,��ÿ�ε���x��1
	}
	if (dy<0) 
	{
		dy = -dy;              //dyʼ��Ϊ��
		Step_Y = -1;  //�ı�y�ĵ�������Ϊ��,��ÿ�ε���y��1
	}

	if (dy>dx)		//���б��>1
	{      
		temp = dx; //����dx��dy
		dx = dy;
		dy = temp;
		change = 1;   //����б��>1��־
	}

	e = -dx;
	x = x0;
	y = y0;

	for (i = 0; i <= dx; i++)
	{
		//����
		set_pixel(x, y);
		if (change) 
			y = y + Step_Y; //���б��>1, ÿ���ȵ���(��ݼ�)y,�����ж�x�ı仯
		else         
			x = x + Step_X; //���б��<=1,ÿ���ȵ���(��ݼ�)x,�����ж�y�ı仯
		e = e + 2 * dy;

		if (e >= 0)
		{
			if (change)       
				x = x + Step_X; //x��ֵ����һ���б仯
			else       
				y = y + Step_Y; //y��ֵ����һ���б仯
			e = e - 2 * dx;
		}
	}
}

/*��ʼ��*/
void init()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, 250.0, 0.0, 250.0);
}

void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-width / 2, width / 2, -height / 2, height / 2);
	glMatrixMode(GL_MODELVIEW);//����ģ�͹۲�任����
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);
}

/*��ε��� Bresenham����������������*/
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	/*45��*/
	glColor3f(1.0, 0.0, 0.0);
	Bresenham_line(0, 0, 150, 150);
	/*135��*/
	glColor3f(1.0, 1.0, 0.0);
	Bresenham_line(0, 0, -150, 150);
	/*225��*/
	glColor3f(1.0, 0.0, 1.0);
	Bresenham_line(0, 0, 150, -150);
	/*315��*/
	glColor3f(0.0, 1.0, 1.0);
	Bresenham_line(0, 0, -150, -150);

	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	/*��ʼ��*/
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	/*���ô��ڲ�����*/
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(300, 300);
	glutCreateWindow("Brensenham �����㷨��ʾ");

	/*����*/
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(Reshape);
	glutMainLoop();

	return 0;

}