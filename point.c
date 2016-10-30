#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "point.h"

void initPoint(Point2D *p)
{
	p->x=0;
	p->y=0;
}
void setPoint(Point2D *p,int x,int y)
{
	p->x=x;
	p->y=y;
}
int comparePoint(Point2D *p1,Point2D *p2)
{
	if(p1->x==p2->x && p1->y==p2->y)
	return 1;
	else 
	return 0;
}
double distEuclid(Point2D p1,Point2D p2)
{
	double d=(p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y);
	return sqrt(d);
}
int comparePts(void *p1,void *p2)
{
	Point2D *pt1,*pt2;
	pt1=(Point2D *)p1;
	pt2=(Point2D *)p2;
	if(pt1->x > pt2->x)
	return 1;
	else if(pt1->x < pt2->x)
	return -1;
	else
	{
		if(pt1->y > pt2->y)
		return 1;
		else if(pt1->y < pt2->y)
		return -1;
		else
		return 0;
	}
	return 0;
}
		
