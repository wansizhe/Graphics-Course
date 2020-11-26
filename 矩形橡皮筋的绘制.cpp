#include <GL\glut.h>
#include <Windows.h>

int x4, y4;		//矩形右上点
int x1=0, y1=0;		//矩形左上点
int x2=0, y2=0;		//矩形右下点
int x3, y3;		//矩形左下点

int first = 500;
int iPointNum = 0;			//已确定的点数
int winWidth = 400, winHeight = 300;		//窗口宽高

/*初始化*/
void Initial()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

/*设定窗口大小*/
void ChangeSize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, winWidth, 0.0, winHeight);
}

/*画出矩形*/
void Display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0f, 0.0f, 0.0f);

	/*如果已经选择了一个点*/
	if (iPointNum >= 1)
	{
		/*按照如下所示的点的顺序画出回路*/
		glBegin(GL_LINE_LOOP);
		glVertex2i(x1, y1);
		glVertex2i(x3, y3);
		glVertex2i(x2, y2);
		glVertex2i(x4, y4);
		glEnd();
	}
	glutSwapBuffers();
}

/*鼠标点击控制*/
void MousePlot(GLint button, GLint action, GLint xMouse, GLint yMouse)
{
	/*当鼠标左键按下后*/
	if (button == GLUT_LEFT_BUTTON&&action == GLUT_DOWN)
	{
		/*画出一个确定的矩形只需要确定两个点即可*/
		/*如果当前没有画出确定的矩形，或者已经画出一个矩形*/
		if (iPointNum == 0 || iPointNum == 2)
		{
			iPointNum = 1;		//将iPointNum重置为1，不论之前是0或2

			/*确定点1*/
			x1 = xMouse;
			y1 = winHeight - yMouse;

			/*进而确定点3,4*/
			x3 = x1;
			y4 = y1;
			y3 = y2;
			x4 = x2;
			/*只有点1确定，没有橡皮筋效果*/

		}
		/*如果当前已经确定一个点，另一个点待确定*/
		else
		{
			iPointNum = 2;

			/*确定点2*/
			x2 =  xMouse;
			y2 = winHeight - yMouse;

			/*进而确定点3,4*/
			x3 = x1;
			y4 = y1;
			y3 = y2;
			x4 = x2;

			glutPostRedisplay();
		}
	}

	/*当鼠标右键按下后，重新画图*/
	if (button == GLUT_RIGHT_BUTTON&&action == GLUT_DOWN)
	{
		iPointNum = 0;
		glutPostRedisplay();
	}
}

/*鼠标移动控制*/
void PassiveMouseMove(GLint xMouse, GLint yMouse)
{
	/*只有当一个点确定，另一个点待确定时*/
	/*鼠标的移动才会体现出橡皮筋效果*/
	if (iPointNum == 1)
	{
		/*确定点2*/
		x2 = xMouse;
		y2 = winHeight-yMouse;

		/*进而确定点3,4*/
		x3 = x1;
		y4 = y1;
		y3 = y2;
		x4 = x2;

		glutPostRedisplay();
	}
}

int main(int argc, char *argv[])
{
	/*初始化*/
	glutInit(&argc, argv);			
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	/*设置窗口并创建窗口*/
	glutInitWindowSize(winWidth,winHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Example");

	/*回调各函数*/
	glutDisplayFunc(Display);
	glutReshapeFunc(ChangeSize);
	glutMouseFunc(MousePlot);
	glutPassiveMotionFunc(PassiveMouseMove);

	Initial();
	glutMainLoop();

	return 0;
}