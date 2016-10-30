//This is the fast version of watershed algorithm..
#include <stdio.h>
#include <stdlib.h>
#include "point.h"
#include "image.h"
#include "show_image.h"
#include "heap.h"
#include "morphology.h"
#include "watershed.h"

//******************************************Comparison***********************************
int comparenode(void *p1,void *p2)
{
	Wnode *n1,*n2;
	n1=(Wnode *)p1;
	n2=(Wnode *)p2;
	if(n1->intensity>n2->intensity)
	return 1;
	else if(n1->intensity<n2->intensity)
	return -1;
	return 0;
}
//******************************************Impose minimum*******************************
void imimposemin(GrayImage *img,GrayImage *marker,GrayImage *result)
{
	int i,j;
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(marker->f[i][j]!=0)
			{
				result->f[i][j]=0;
			}
			else
			result->f[i][j]=img->f[i][j];
		}
	}
}
//******************************************Fast watershed*******************************
void watershed(GrayImage *img,GrayImage *result)
{
	GrayImage S,Sm,marker;
	Heap Sd;
	Wnode *n;
	int **SE=strel("111111111",3);
	int N[8][2]={{1,0},{0,1},{-1,0},{0,-1},{1,1},{-1,1},{1,-1},{-1,-1}};
	int *labels,nL,i,j,heapsize=0,ix,iy;
	initGrayImage(&marker,img->row,img->col,img->maxval);
	Sm=imcomplement(img);
	imregionalmax(&Sm,&marker);
	freeImage(&Sm);
	/*Sm=imcomplement(&marker);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			marker.f[i][j]=Sm.f[i][j];
		}
	}
	freeImage(&Sm);*/
	S=imlabel(&marker,&labels,&nL);
	Sm=imerode(&marker,SE,3);
	freeImage(&marker);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(Sm.f[i][j]!=img->maxval)
			heapsize++;
		}
	}
	initHeap(&Sd,sizeof(Wnode),heapsize);
	n=(Wnode *)malloc(sizeof(Wnode));
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(Sm.f[i][j]!=img->maxval)
			{
				n->p.x=i;
				n->p.y=j;
				n->intensity=img->f[i][j];
				insert2Heap(&Sd,n,sizeof(Wnode),comparenode);
			}
		}
	}
	free(n);
	while(!emptyH(&Sd))
	{
		n=(Wnode *)del4mHeap(&Sd,comparenode);
		for(i=0;i<8;i++)
		{
			ix=n->p.x+N[i][0];
			iy=n->p.y+N[i][1];
			if(ix>=0 && ix<img->row && iy>=0 && iy<img->col)
			{
				if(S.f[ix][iy]==0)
				S.f[ix][iy]=S.f[i][j];
			}
		}
		free(n);
	}
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			result->f[i][j]=S.f[i][j];
		}
	}
}
main(int argc,char **argv)
{
	GrayImage img,marker,temp;
	readPGM(argv[1],&img);
	readPGM(argv[2],&marker);
	initGrayImage(&temp,img.row,img.col,img.maxval);	
	watershed(&img,&temp);
	label2rgb(&temp);
}
	
