#include <GL/glut.h>
#include <math.h>
#include<iostream>
using namespace std;

#define ScanRange 1000


void Bresonham(int x0, int y0, int x1, int y1)				//Bresonham画线算法
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
	e = -dx, x = x0, y = y0;
	for (i = 0; i <= dx; i++)
	{
		//画点
		glVertex3i(x, y, 0);
		if (change)  y = y + Step_Y;
		else	  	x = x + Step_X;
		e = e + 2 * dy;
		if (e >= 0)
		{
			if (change)	x = x + Step_X;
			else        y = y + Step_Y;
			e = e - 2 * dx;
		}
	}
	glEnd();
}

struct Edge {
	float x;		//与扫描线交点x坐标
	float dx;		//扫描线移动时，交点x坐标增量
	float dmax;		//最高点与扫描线的距离
	Edge* next;
};

struct Point {
	GLint x;		//x坐标
	GLint y;		//y坐标
	int dist;		//与扫描线间的距离
};

double k = -1;		//扫描线斜率
double b = 0;		//斜率初始截距
Point* vertexs; //顶点数组
int N;			//顶点个数
int interval = 4;   //影线密集程度

void GetDist_Points(Point* points, int numofpoints, double k)		//计算每个顶点与扫描线的距离
{
	int i;

	/*调整影线初始截距*/
	while (1)
	{
		bool flag = true;
		for (int j = 0; j < numofpoints; ++j)
		{
			double t = points[j].y - (points[j].x*k + b);
			if (t < 0)
				flag = false;
		}
		if (flag)
			break;
		else
			b -= 10;
	}
	for (i = 0; i < numofpoints; ++i)
		points[i].dist = abs(k*points[i].x - points[i].y + b) / sqrt(1 + k*k);
}

void InsertEdge(Edge* list, Edge* edge)				//按交点x坐标升序插入边节点
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

int GetNeighbor_dist(int index, Point* points, int numofpoints)		//求下标为index的顶点的下一个不平行的顶点的dist值
{
	int j = index + 1;
	if ((index + 1) > (numofpoints - 1))
		j = 0;
	while (points[index].dist == points[j].dist)
	{
		if ((j + 1) > (numofpoints - 1))
			j = 0;
		else
			j++;
	}
	return points[j].dist;
}

void Print(Edge* edge)
{
	Edge* p = edge;
	while (p)
	{
		cout << "x:" << p->x << "   " << "dx:" << p->dx << "   " << "dmax:" << p->dmax << endl;
		p = p->next;
	}
}

void ProduceEdge(Point lower, Point higher, int neighbor_dist, Edge* Net)		//根据两个顶点和较高点另一个相邻点的dist生成一个边节点
{
	float k1 = (higher.y - lower.y) / float(higher.x - lower.x);		//边的斜率 

	Edge* edge = new Edge;
	edge->next = NULL;
	edge->x = lower.x;
	edge->dx = sqrt(1 + k*k) / (k1 - k);			//扫描线每移动一个单位，与该边交点x坐标的增量
	if (higher.dist < neighbor_dist)
		edge->dmax = higher.dist - 1;
	else
		edge->dmax = higher.dist;
	InsertEdge(&Net[lower.dist], edge);			//根据该边最低点距离扫描线的距离，插入到Net新边表中
	Print((&Net[lower.dist])->next);
}

void BuildNet(Edge* Net, Point* points, int numofpoints)
{

	Point v1 = points[numofpoints - 1];
	int previous_dist = points[numofpoints - 2].dist;

	for (int i = 0; i < numofpoints; ++i)
	{
		Point v2 = points[i];
		if (v1.dist != v2.dist)
		{
			if (v2.dist > v1.dist)		//v2更高
				ProduceEdge(v1, v2, GetNeighbor_dist(i, points, numofpoints), Net);
			else                        //v1更高
				ProduceEdge(v2, v1, previous_dist, Net);
		}
		previous_dist = v1.dist;
		v1 = v2;
	}
}

void Insert2AET(int scan, Edge* Net, Edge* AET)		//将Net[scan]中的边节点全部插入AET表中
{
	Edge *p = Net[scan].next;
	Edge *q;
	while (p != NULL)
	{
		q = p->next;
		InsertEdge(AET, p);
		p = q;
	}
}

void UpdateAET(Edge* AET, int scan)				//更新AET表，将dmax<=scan的边节点删除
{
	Edge* p = AET->next, *q = AET;
	while (p)
	{
		if (scan >= p->dmax)
		{
			Edge* t = p;
			p = p->next;
			q->next = p;
			delete t;
		}
		else
		{
			p->x += p->dx;
			q = p;
			p = p->next;
		}
	}
}

void ResortAET(Edge* AET)				//对AET表中的边节点按x升序重新排列
{
	Edge* p = AET->next, *q;
	AET->next = NULL;
	while (p)
	{
		q = p->next;
		InsertEdge(AET, p);
		p = q;
	}
}

void Fill(Edge* AET, int scan)				//一次扫描填充
{
	Edge *p, *q;
	p = AET->next;
	while (p)
	{
		q = p->next;
		int x0 = p->x;
		int x1 = q->x;
		int y0 = k*x0 + scan*sqrt(float(k*k + 1)) + b;			//根据扫描线直线方程求y坐标
		int y1 = k*x1 + scan*sqrt(float(k*k + 1)) + b;

		Bresonham(x0, y0, x1, y1);
		p = q->next;
	}
}

void Scanfill(Point* points, int numofpoints)		//填充
{

	GetDist_Points(points, numofpoints, k);
	/*for (int i = 0; i < N; ++i)
	{
	cout << "第" << i + 1 << "个顶点到扫描线的距离：" << points[i].dist << endl;
	}*/
	Edge* Net = new Edge[ScanRange];
	for (int i = 0; i < ScanRange; ++i)
		Net[i].next = NULL;

	BuildNet(Net, points, numofpoints);
	Edge* AET = new Edge;
	AET->next = NULL;

	int scan = 0;
	int count = 0;
	for (; scan < ScanRange; scan++)
	{
		Insert2AET(scan, Net, AET);
		if (AET->next != NULL)
		{
			if (count % interval == 0)			//可调节影线密集程度
				Fill(AET, scan);
			UpdateAET(AET, scan);
			ResortAET(AET);
		}
		count++;
	}
}

void Init(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glColor3f(0.0f, 0.0f, 0.0f);
	glPointSize(2.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 640.0, 0.0, 480.0);
}

void myDisplay(void)
{

	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i<N; i++)
		glVertex2i(vertexs[i].x, vertexs[i].y);
	glEnd();
	Scanfill(vertexs, N);
	glutSwapBuffers();
	glFlush();

}

int main(int argc, char *argv[])
{
	cout << "请输入扫描线斜率：";
	cin >> k;
	cout << "请输入顶点个数：";
	cin >> N;
	vertexs = new Point[N];
	cout << "输入顶点坐标：" << endl;
	for (int i = 0; i < N; i++)
	{
		cin >> vertexs[i].x;
		cin >> vertexs[i].y;

		cout << "第" << i + 1 << "个顶点坐标为(" << vertexs[i].x << "," << vertexs[i].y << ")" << endl;
	}

	cout << "输入影线间隔:";
	cin >> interval;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 480);
	glutCreateWindow("影线填充封闭多边形");
	glutDisplayFunc(myDisplay);
	Init();
	glutMainLoop();
	return 0;
}