#include <iostream>
#include <GL\glut.h>
using namespace std;

#define ScanMAX 1000
int slope; //斜率
int yinter = 0; //y轴截距，默认初始为0

/*Bresonham画线算法*/
void Bresonham_Line(int x0, int y0, int x1, int y1)				
{
	int x, y, dx, dy, e;
	int i, temp;
	int change = 0;
	int Step_X = 1, Step_Y = 1;
	dx = x1 - x0, dy = y1 - y0;
	glBegin(GL_POINTS);
	if (dx<0) {
		dx = -dx;
		Step_X = -1;
	}
	if (dy<0) {
		dy = -dy;
		Step_Y = -1;
	}
	if (dy>dx) {
		temp = dx;
		dx = dy;
		dy = temp;
		change = 1;
	}
	e = -dx;
	x = x0;
	y = y0;
	for (i = 0; i <= dx; i++)
	{
		//画点
		glVertex3i(x, y, 0);
		if (change)  
			y = y + Step_Y;
		else	  	
			x = x + Step_X;
		e = e + 2 * dy;
		if (e >= 0)
		{
			if (change)	
				x = x + Step_X;
			else        
				y = y + Step_Y;
			e = e - 2 * dx;
		}
	}
	glEnd();
}

/*用于构建新边表和活性边表*/
struct Edge
{
	float x;          /* 当前扫描线与边的交点的x值  */
	float dx;         /* 从当前扫描线到下一扫描线之间的x的增量 */
	int dmax;      /*  计算点到y=kx的距离 */
	struct Edge *next;
};
/*点结构*/
struct Point
{
	GLint x;
	GLint y;
	int d;	//点到y=kx的距离，k为影线线的斜率
};

/*计算所有点到y=kx的距离*/
void CalcutalePointsD(int k, Point *points, int count)	//计算每一点的间距
{
	//初始化y轴截距
	while (1)
	{
		bool flag = true;
		for (int j = 0; j < count; ++j)
		{
			double t = points[j].y - (points[j].x*k + yinter);
			if (t < 0)
				flag = false;
		}
		if (flag)
			break;
		else
			yinter -= 10;
	}

	//计算点到y=kx的距离
	double n = 1 + k*k;
	float m = sqrt(n);

	for (int i = 0; i<count; i++)
		points[i].d = abs(k*points[i].x-points[i].y +yinter) / m;
}

/*按交点的x升序对边进行插入排序*/
void InsertSortEdge(Edge *list, Edge *edge)
{
	Edge *p, *q = list;
	p = q->next;
	while (p)
	{
		if (edge->x<p->x)
			p = NULL;
		else
		{
			q = p;
			p = p->next;
		}
	}
	edge->next = q->next;
	q->next = edge;
}

/*
	按照Point表的记录顺序，访问当前点之后的，
	满足以下条件的点的d值（到y=kx的距离）：
	1. 该点与当前点的连线不平行于影线
	2. 该点在Point表中排在当前点之后，且离当前点的序号最接近
*/
int  NextPointD(int k, int count, Point *points)  
{
	int j;
	if ((k + 1)>(count - 1))	//当前已经到最后一点，则循环回到第一点
		j = 0;
	else		//还未到最后一点，就访问正常的下一点
		j = k + 1;
	while (points[k].d == points[j].d)		//排除连线与影线平行的点
	{
		if ((j + 1)>(count - 1)) 
			j = 0;	//如果已经到达最后一点，则循环回到第一个点
		else
			j++;
	}
	return(points[j].d);
}

/*生成有序边表的一条边 */
void CreateNewEdge(Point lower, Point upper, int dComp, Edge *edges)
{
	float m;
	//float k = -1;	//影线斜率
	int db = 1;	//影线增长单位

	m = (float)(upper.y - lower.y) / (float)(upper.x - lower.x);	//计算边的斜率

	Edge* edge = new Edge;
	edge->next = NULL;

	//通过一定的数学计算会发现
	//影线增加一个单位，x的变化值是sqrt(1+k^2)/(m-k)
	edge->dx = db*sqrt(1 + slope*slope) / (m - slope);

	//边结构中保存的x值是边中相对于影线的最低点
	edge->x = lower.x;

	//为防止凹多边形填充错误而做的处理
	//判断upper点是不是真的局部最高点
	if (upper.d < dComp)
		edge->dmax = upper.d -1;//该点既不是局部高点也不是局部低点
	else
		edge->dmax = upper.d;//局部高点
	InsertSortEdge(&edges[lower.d], edge);//tips
}

/*建立新边表*/
void CreateNewEdgesList(int count, Point *points, Edge *edges)
{
	//Edge *edge;
	Point v1, v2;	//v2表示序号较大的点，v1表示序号较小的点，但是初始化时v2是第一个点而v1是最后一个点
	int i, dPrev = points[count - 2].d;		//dPrev表示当前新边的前一个点的d值，因此被初始化为最后一个点的上一个点，也就是倒数第二个点
	v1 = points[count - 1];	//v1被初始化为最后一个点，即为第一个点的上一个点

	/*
		填充边表需要三个在Point表中连续的点，
		其中两个点连线构成新边，
		另一个点是新边端点中序号较小的端点的前一个点
		它用来辅助填充边结构体的dmax值，
		针对性处理凹多边形
	*/
	for (i = 0; i<count; i++)
	{
		v2 = points[i];
		//如果两点到y=kx距离不相同，
		//说明这两点的联系不是平行于扫描线的
		//那么就可以加入新边
		if (v1.d != v2.d)	
		{
			//edge = new Edge;
			if ((v1.d < v2.d))
				//如果v2到影线的距离更远
				//那么参考值就要在v2的一侧找
				//调用NextPointD可以找到需要的点的d值
				CreateNewEdge(v1, v2, NextPointD(i, count, points), edges);      
			else
				//如果v1到影线的距离更远
				//那么参考值就要在v1的一侧找
				//就是已经保存了的v1的上一个点的d值
				CreateNewEdge(v2, v1, dPrev,  edges);
		}
		//向前推进建立新边
		dPrev = v1.d;
		v1 = v2;
	}
}

/*建立活性边表*/
void CreateActiveEdgesList(int scan, Edge * active, Edge *edges)
{
	Edge *p, *q;
	p = edges[scan].next;
	while (p)
	{
		q = p->next;
		InsertSortEdge(active, p);
		p = q;
	}
}

/*对当前扫描线填充*/
void FillScanLine(int scan, Edge * active)
{
	Edge *p1, *p2;

	p1 = active->next;
	while (p1)
	{
		p2 = p1->next;
		int x1 = p1->x;
		int x2 = p2->x;
		int y1 = slope * x1 + scan*sqrt(float(1 + slope*slope))+yinter;
		int y2 = slope * x2 + scan*sqrt(float(1 + slope*slope))+yinter;
		//确定影线两端点后，调用bresonham画线算法
		Bresonham_Line(x1, y1, x2, y2);
		p1 = p2->next;
	}
}

/*为下一条扫描线进行更新活性边表*/
void  UpdateActiveList(int scan, Edge *active)
{
	Edge *q = active, *p = active->next;
	Edge *t;
	while (p)
	{
		//tips
		if (scan >= p->dmax)
		{
			/*删除不再相交的边*/
			t = p;
			p = p->next;
			q->next = p;
			delete t;
		}
		else
		{
			//继续推进扫描线
			p->x = p->x + p->dx;
			q = p;
			p = p->next;
		}
	}
}

/*重排活性边表*/
void ResortActiveList(Edge *active)
{
	Edge *q;
	Edge *p = active->next;
	active->next = NULL;
	while (p)
	{
		q = p->next;
		InsertSortEdge(active, p);
		p = q;
	}
}

void ScanLineFilling(int count, Point *points)
{
	int db = 0;
	// 初始化点集
	CalcutalePointsD(slope, points, count); //-1为斜率
	Edge edges[ScanMAX], *active;
	int i, scan;

	// 构造新边表
	for (i = 0; i<ScanMAX; i++)
		edges[i].next = NULL;
	CreateNewEdgesList(count, points, edges);

	active = new Edge;
	active->next = NULL;

	for (scan = 0; scan<ScanMAX; scan++)
	{
		//构造活性边表
		CreateActiveEdgesList(scan, active, edges);
		if (active->next)
		{
			if (db % 10 == 0)//10个间距画一条线。
				FillScanLine(scan, active);
			//更新活性边表
			UpdateActiveList(scan, active);
			// 重排活性边表
			ResortActiveList(active);
		}
		db++;
	}
}

int pointnum;
Point *p;
bool select = false;
void Initial()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glColor3f(0.0f, 0.0f, 0.0f);
	glPointSize(2.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 640.0, 0.0, 480.0);
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	if (select == true)
	{
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i<pointnum; i++)
			glVertex2i(p[i].x, p[i].y);
		glEnd();
		ScanLineFilling(pointnum, p);
	}
	glutSwapBuffers();
	glFlush();

}

void MousePlot(int button, int state, int x, int y)  //鼠标控制部分，通过点击来记录点
{
	static int number = 0;

	if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			p[number].x = x;
			p[number].y = 480 - y;

			glBegin(GL_POINTS);
			glVertex2i(x, 480 - y);
			glEnd();
			glFlush();
			cout << p[number].x << "  " << p[number].y << endl;
			number++;

			if (number == pointnum)
			{
				number = 0;
				select = true;
				glutPostRedisplay();
			}
		}
		else if (button == GLUT_RIGHT_BUTTON)
		{
			glClear(GL_COLOR_BUFFER_BIT);
			glFlush();
		}
	}
}


int main(int argc, char *argv[])
{
	cout << "请输入斜率" << endl;
	cin >> slope;
	cout << "请输入点数" << endl;
	cin >> pointnum;
	p = new Point [pointnum];

	/* cout << "输入顶点坐标：" << endl;
	for (int i = 0; i < pointnum; i++)
	{
		cin >> p[i].x;
		cin >> p[i].y;

		cout << "第" << i + 1 << "个顶点坐标为(" << p[i].x << "," << p[i].y << ")" << endl;
	}             */                                                                                     

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 480);
	glutCreateWindow("OpenGL");
	glutMouseFunc(MousePlot);
	glutDisplayFunc(Display);
	Initial();
	glutMainLoop();
	return 0;
}