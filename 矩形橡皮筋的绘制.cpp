#include <GL\glut.h>
#include <Windows.h>

int x4, y4;		//�������ϵ�
int x1=0, y1=0;		//�������ϵ�
int x2=0, y2=0;		//�������µ�
int x3, y3;		//�������µ�

int first = 500;
int iPointNum = 0;			//��ȷ���ĵ���
int winWidth = 400, winHeight = 300;		//���ڿ��

/*��ʼ��*/
void Initial()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

/*�趨���ڴ�С*/
void ChangeSize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, winWidth, 0.0, winHeight);
}

/*��������*/
void Display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 0.0f, 0.0f);

	/*����Ѿ�ѡ����һ����*/
	if (iPointNum >= 1)
	{
		/*����������ʾ�ĵ��˳�򻭳���·*/
		glBegin(GL_LINE_LOOP);
		glVertex2i(x1, y1);
		glVertex2i(x3, y3);
		glVertex2i(x2, y2);
		glVertex2i(x4, y4);
		glEnd();
	}
	glutSwapBuffers();
}

/*���������*/
void MousePlot(GLint button, GLint action, GLint xMouse, GLint yMouse)
{
	/*�����������º�*/
	if (button == GLUT_LEFT_BUTTON&&action == GLUT_DOWN)
	{
		/*����һ��ȷ���ľ���ֻ��Ҫȷ�������㼴��*/
		/*�����ǰû�л���ȷ���ľ��Σ������Ѿ�����һ������*/
		if (iPointNum == 0 || iPointNum == 2)
		{
			iPointNum = 1;		//��iPointNum����Ϊ1������֮ǰ��0��2

			/*ȷ����1*/
			x1 = xMouse;
			y1 = winHeight - yMouse;

			/*����ȷ����3,4*/
			x3 = x1;
			y4 = y1;
			y3 = y2;
			x4 = x2;
			/*ֻ�е�1ȷ����û����Ƥ��Ч��*/

		}
		/*�����ǰ�Ѿ�ȷ��һ���㣬��һ�����ȷ��*/
		else
		{
			iPointNum = 2;

			/*ȷ����2*/
			x2 =  xMouse;
			y2 = winHeight - yMouse;

			/*����ȷ����3,4*/
			x3 = x1;
			y4 = y1;
			y3 = y2;
			x4 = x2;

			glutPostRedisplay();
		}
	}

	/*������Ҽ����º����»�ͼ*/
	if (button == GLUT_RIGHT_BUTTON&&action == GLUT_DOWN)
	{
		iPointNum = 0;
		glutPostRedisplay();
	}
}

/*����ƶ�����*/
void PassiveMouseMove(GLint xMouse, GLint yMouse)
{
	/*ֻ�е�һ����ȷ������һ�����ȷ��ʱ*/
	/*�����ƶ��Ż����ֳ���Ƥ��Ч��*/
	if (iPointNum == 1)
	{
		/*ȷ����2*/
		x2 = xMouse;
		y2 = winHeight-yMouse;

		/*����ȷ����3,4*/
		x3 = x1;
		y4 = y1;
		y3 = y2;
		x4 = x2;

		glutPostRedisplay();
	}
}

int main(int argc, char *argv[])
{
	/*��ʼ��*/
	glutInit(&argc, argv);			
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	/*���ô��ڲ���������*/
	glutInitWindowSize(winWidth,winHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Example");

	/*�ص�������*/
	glutDisplayFunc(Display);
	glutReshapeFunc(ChangeSize);
	glutMouseFunc(MousePlot);
	glutPassiveMotionFunc(PassiveMouseMove);

	Initial();
	glutMainLoop();

	return 0;
}