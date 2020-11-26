#include <iostream>
#include <GL\glut.h>
using namespace std;

#define ScanMAX 1000
int slope; //б��
int yinter = 0; //y��ؾ࣬Ĭ�ϳ�ʼΪ0

/*Bresonham�����㷨*/
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
		//����
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

/*���ڹ����±߱�ͻ��Ա߱�*/
struct Edge
{
	float x;          /* ��ǰɨ������ߵĽ����xֵ  */
	float dx;         /* �ӵ�ǰɨ���ߵ���һɨ����֮���x������ */
	int dmax;      /*  ����㵽y=kx�ľ��� */
	struct Edge *next;
};
/*��ṹ*/
struct Point
{
	GLint x;
	GLint y;
	int d;	//�㵽y=kx�ľ��룬kΪӰ���ߵ�б��
};

/*�������е㵽y=kx�ľ���*/
void CalcutalePointsD(int k, Point *points, int count)	//����ÿһ��ļ��
{
	//��ʼ��y��ؾ�
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

	//����㵽y=kx�ľ���
	double n = 1 + k*k;
	float m = sqrt(n);

	for (int i = 0; i<count; i++)
		points[i].d = abs(k*points[i].x-points[i].y +yinter) / m;
}

/*�������x����Ա߽��в�������*/
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
	����Point��ļ�¼˳�򣬷��ʵ�ǰ��֮��ģ�
	�������������ĵ��dֵ����y=kx�ľ��룩��
	1. �õ��뵱ǰ������߲�ƽ����Ӱ��
	2. �õ���Point�������ڵ�ǰ��֮�����뵱ǰ��������ӽ�
*/
int  NextPointD(int k, int count, Point *points)  
{
	int j;
	if ((k + 1)>(count - 1))	//��ǰ�Ѿ������һ�㣬��ѭ���ص���һ��
		j = 0;
	else		//��δ�����һ�㣬�ͷ�����������һ��
		j = k + 1;
	while (points[k].d == points[j].d)		//�ų�������Ӱ��ƽ�еĵ�
	{
		if ((j + 1)>(count - 1)) 
			j = 0;	//����Ѿ��������һ�㣬��ѭ���ص���һ����
		else
			j++;
	}
	return(points[j].d);
}

/*��������߱��һ���� */
void CreateNewEdge(Point lower, Point upper, int dComp, Edge *edges)
{
	float m;
	//float k = -1;	//Ӱ��б��
	int db = 1;	//Ӱ��������λ

	m = (float)(upper.y - lower.y) / (float)(upper.x - lower.x);	//����ߵ�б��

	Edge* edge = new Edge;
	edge->next = NULL;

	//ͨ��һ������ѧ����ᷢ��
	//Ӱ������һ����λ��x�ı仯ֵ��sqrt(1+k^2)/(m-k)
	edge->dx = db*sqrt(1 + slope*slope) / (m - slope);

	//�߽ṹ�б����xֵ�Ǳ��������Ӱ�ߵ���͵�
	edge->x = lower.x;

	//Ϊ��ֹ�����������������Ĵ���
	//�ж�upper���ǲ�����ľֲ���ߵ�
	if (upper.d < dComp)
		edge->dmax = upper.d -1;//�õ�Ȳ��Ǿֲ��ߵ�Ҳ���Ǿֲ��͵�
	else
		edge->dmax = upper.d;//�ֲ��ߵ�
	InsertSortEdge(&edges[lower.d], edge);//tips
}

/*�����±߱�*/
void CreateNewEdgesList(int count, Point *points, Edge *edges)
{
	//Edge *edge;
	Point v1, v2;	//v2��ʾ��Žϴ�ĵ㣬v1��ʾ��Ž�С�ĵ㣬���ǳ�ʼ��ʱv2�ǵ�һ�����v1�����һ����
	int i, dPrev = points[count - 2].d;		//dPrev��ʾ��ǰ�±ߵ�ǰһ�����dֵ����˱���ʼ��Ϊ���һ�������һ���㣬Ҳ���ǵ����ڶ�����
	v1 = points[count - 1];	//v1����ʼ��Ϊ���һ���㣬��Ϊ��һ�������һ����

	/*
		���߱���Ҫ������Point���������ĵ㣬
		�������������߹����±ߣ�
		��һ�������±߶˵�����Ž�С�Ķ˵��ǰһ����
		�������������߽ṹ���dmaxֵ��
		����Դ��������
	*/
	for (i = 0; i<count; i++)
	{
		v2 = points[i];
		//������㵽y=kx���벻��ͬ��
		//˵�����������ϵ����ƽ����ɨ���ߵ�
		//��ô�Ϳ��Լ����±�
		if (v1.d != v2.d)	
		{
			//edge = new Edge;
			if ((v1.d < v2.d))
				//���v2��Ӱ�ߵľ����Զ
				//��ô�ο�ֵ��Ҫ��v2��һ����
				//����NextPointD�����ҵ���Ҫ�ĵ��dֵ
				CreateNewEdge(v1, v2, NextPointD(i, count, points), edges);      
			else
				//���v1��Ӱ�ߵľ����Զ
				//��ô�ο�ֵ��Ҫ��v1��һ����
				//�����Ѿ������˵�v1����һ�����dֵ
				CreateNewEdge(v2, v1, dPrev,  edges);
		}
		//��ǰ�ƽ������±�
		dPrev = v1.d;
		v1 = v2;
	}
}

/*�������Ա߱�*/
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

/*�Ե�ǰɨ�������*/
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
		//ȷ��Ӱ�����˵�󣬵���bresonham�����㷨
		Bresonham_Line(x1, y1, x2, y2);
		p1 = p2->next;
	}
}

/*Ϊ��һ��ɨ���߽��и��»��Ա߱�*/
void  UpdateActiveList(int scan, Edge *active)
{
	Edge *q = active, *p = active->next;
	Edge *t;
	while (p)
	{
		//tips
		if (scan >= p->dmax)
		{
			/*ɾ�������ཻ�ı�*/
			t = p;
			p = p->next;
			q->next = p;
			delete t;
		}
		else
		{
			//�����ƽ�ɨ����
			p->x = p->x + p->dx;
			q = p;
			p = p->next;
		}
	}
}

/*���Ż��Ա߱�*/
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
	// ��ʼ���㼯
	CalcutalePointsD(slope, points, count); //-1Ϊб��
	Edge edges[ScanMAX], *active;
	int i, scan;

	// �����±߱�
	for (i = 0; i<ScanMAX; i++)
		edges[i].next = NULL;
	CreateNewEdgesList(count, points, edges);

	active = new Edge;
	active->next = NULL;

	for (scan = 0; scan<ScanMAX; scan++)
	{
		//������Ա߱�
		CreateActiveEdgesList(scan, active, edges);
		if (active->next)
		{
			if (db % 10 == 0)//10����໭һ���ߡ�
				FillScanLine(scan, active);
			//���»��Ա߱�
			UpdateActiveList(scan, active);
			// ���Ż��Ա߱�
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

void MousePlot(int button, int state, int x, int y)  //�����Ʋ��֣�ͨ���������¼��
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
	cout << "������б��" << endl;
	cin >> slope;
	cout << "���������" << endl;
	cin >> pointnum;
	p = new Point [pointnum];

	/* cout << "���붥�����꣺" << endl;
	for (int i = 0; i < pointnum; i++)
	{
		cin >> p[i].x;
		cin >> p[i].y;

		cout << "��" << i + 1 << "����������Ϊ(" << p[i].x << "," << p[i].y << ")" << endl;
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