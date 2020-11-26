#include <cmath>
#include <gl\glut.h>
using namespace std;

/*确定一个点*/
void set_pixel(int x, int y)
{
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}

/* Bresenham画线算法主体部分*/
void Bresenham_line(int x0, int y0, int x1, int y1)
{
	int x, y, dx, dy, e;
	int i, temp;
	int change = 0;			// 如果直线斜率<=1, change=0; 否则change=1
	int Step_X = 1, Step_Y = 1;			//取值为1 或 -1 ,表示迭代过程中 x坐标和y坐标变化的方向
	dx = x1 - x0, dy = y1 - y0;

	if (dx<0) 
	{
		dx = -dx;  //dx始终为正
		Step_X = -1;//改变x的迭代方向为负,即每次迭代x减1
	}
	if (dy<0) 
	{
		dy = -dy;              //dy始终为正
		Step_Y = -1;  //改变y的迭代方向为负,即每次迭代y减1
	}

	if (dy>dx)		//如果斜率>1
	{      
		temp = dx; //交换dx和dy
		dx = dy;
		dy = temp;
		change = 1;   //设置斜率>1标志
	}

	e = -dx;
	x = x0;
	y = y0;

	for (i = 0; i <= dx; i++)
	{
		//画点
		set_pixel(x, y);
		if (change) 
			y = y + Step_Y; //如果斜率>1, 每次先递增(或递减)y,而后判断x的变化
		else         
			x = x + Step_X; //如果斜率<=1,每次先递增(或递减)x,而后判断y的变化
		e = e + 2 * dy;

		if (e >= 0)
		{
			if (change)       
				x = x + Step_X; //x的值比上一点有变化
			else       
				y = y + Step_Y; //y的值比上一点有变化
			e = e - 2 * dx;
		}
	}
}

/*初始化*/
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
	glMatrixMode(GL_MODELVIEW);//定义模型观察变换矩阵
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);
}

/*多次调用 Bresenham，画出多个方向的线*/
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	/*45度*/
	glColor3f(1.0, 0.0, 0.0);
	Bresenham_line(0, 0, 150, 150);
	/*135度*/
	glColor3f(1.0, 1.0, 0.0);
	Bresenham_line(0, 0, -150, 150);
	/*225度*/
	glColor3f(1.0, 0.0, 1.0);
	Bresenham_line(0, 0, 150, -150);
	/*315度*/
	glColor3f(0.0, 1.0, 1.0);
	Bresenham_line(0, 0, -150, -150);

	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	/*初始化*/
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	/*设置窗口并创建*/
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(300, 300);
	glutCreateWindow("Brensenham 画线算法演示");

	/*画线*/
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(Reshape);
	glutMainLoop();

	return 0;

}