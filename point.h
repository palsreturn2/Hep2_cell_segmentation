typedef struct
{
	int x;
	int y;
} Point2D;

void initPoint(Point2D *p);
void setPoint(Point2D *p,int x,int y);
int comparePoint(Point2D *p1,Point2D *p2);
double distEuclid(Point2D p1,Point2D p2);
int comparePts(void *p1,void *p2);
