#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include "queue.h"
#include "image.h"
#include "colorimage.h"
#include "gengrowcut.h"
#include "show_image.h"
#include "point.h"
#include "morphology.h"


#define NEIGHBOURS 4
/*********************Copy Feature vector*************************************************************************/
void copyfv(float *fv1,float *fv2,int nfv)
{
	int i;
	for(i=0;i<nfv;i++)
	fv2[i]=fv1[i];
}
void cell2img(Cell *cell,GrayImage *img)
{
	int i,j;
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		img->f[i][j]=cell->f[i][j].label;
	}
}
/********************getSeedImage*********************************************************************************/
void initSeedPixel(HSIImage *img,GrayImage *fgm,GrayImage *bgm,Cell *cellOld)
{
	int i,j,*labels,nL,thresh;
	GrayImage temp,Io;
	initGrayImage(&Io,img->row,img->col,img->maxval);
	thresh=graythres(img);
	temp=imlabel(fgm,&labels,&nL);
	for(i=0;i<bgm->row;i++)
	{
		for(j=0;j<bgm->col;j++)
		{
			cellOld->f[i][j].fv=(float *)malloc(sizeof(cellOld->nfv));
			if(bgm->f[i][j]!=0)
			{
				cellOld->f[i][j].label=1;
				if(img->f[i][j]<thresh)
				cellOld->f[i][j].strength=1;
				else
				cellOld->f[i][j].strength=0.9;
			}
			else if(fgm->f[i][j]!=0)
			{
				cellOld->f[i][j].label=temp.f[i][j]+1;
				if(img->f[i][j]>=thresh)
				cellOld->f[i][j].strength=1;
				else
				cellOld->f[i][j].strength=0;
			}
			else
			{
				cellOld->f[i][j].label=0;
				cellOld->f[i][j].strength=0;
			}
			cellOld->f[i][j].fv[0]=img->f[0][i][j];
			cellOld->f[i][j].fv[1]=img->f[1][i][j];
			cellOld->f[i][j].fv[2]=img->f[2][i][j];
		}
	}	
	freeImage(&Io);
}
/*****************Another Function to store the neighbours in an image**************************************************/
void getNeighbour2(int ix,int iy,NewCell *N,Cell *cellOld)
{
	int K[NEIGHBOURS][2]={{-1,0},{0,-1},{0,1},{1,0}};
	int mx,my,i;
	for(i=0;i<NEIGHBOURS;i++)
	{
		mx=ix+K[i][0];
		my=iy+K[i][1];
		if(mx<0)
		mx=0;
		if(my<0)
		my=0;
		if(mx>=cellOld->row)
		mx=cellOld->row-1;
		if(my>=cellOld->col)
		my=cellOld->col-1;
		N[i].c.label=cellOld->f[mx][my].label;
		N[i].c.strength=cellOld->f[mx][my].strength;
		copyfv(cellOld->f[mx][my].fv,N[i].c.fv,cellOld->nfv);
		N[i].x=mx;
		N[i].y=my;
	}
}
/*******************Function to calculate distance b/w two feature Vectors*************************************/
float calDisMag(float * fv1,float *fv2,int nfv)
{
	float dI;
	int i;
	for(i=0;i<nfv;i++)
	{
		dI=dI+pow(fv1[i]-fv2[i],2);
	}
	dI=sqrt(dI);
	if(dI<0)
		return (-dI);
	return dI;
}
/******************Function to calculate g value****************************************************************/
float gFunction(float t,float gmax)
{
	float gval;
	gval=(1-((t)/gmax));
	return gval;
}
/**********************************************Synchronous growcut algorithm******************/
void syncgrowCut(Cell *cellOld,GrayImage *img)
{
	int i,j,gmax=0,k,dis,count=0,flag,ct=0;
	float *tempfv=(float*)malloc(sizeof(float)*cellOld->nfv);
	float gFinal;
	Queue cellQ,ncellQ;
	NewCell *N=(NewCell *)malloc(sizeof(NewCell)*NEIGHBOURS);
	for(i=0;i<NEIGHBOURS;i++)
	N[i].c.fv=(float *)malloc(sizeof(float)*cellOld->nfv);
	struct node *nod,*nod2;
	initQueue(&cellQ);
	NewCell pt,*pt2,*pt3;
	GrayImage coverImg;
	Cell cellNew;
	initGrayImage(&coverImg,cellOld->row,cellOld->col,1);
	initQueue(&cellQ);
	initQueue(&ncellQ);
	
	cellNew.f=(struct cell **)malloc(sizeof(struct cell *)*cellOld->row);
	for(i=0;i<cellOld->row;i++)
	cellNew.f[i]=(struct cell *)malloc(sizeof(struct cell) * cellOld->col);
	cellNew.row=img->row;
	cellNew.col=img->col;
	cellNew.nfv=cellOld->nfv;
	
	for(i=0;i<cellOld->nfv;i++)
	tempfv[i]=0;
	
	for(i=0;i<cellOld->row;i++)
	{
		for(j=0;j<cellOld->col;j++)
		{
			if(gmax<calDisMag(cellOld->f[i][j].fv,tempfv,cellOld->nfv))
			gmax=calDisMag(cellOld->f[i][j].fv,tempfv,cellOld->nfv);
		}
	}
	for(i=0;i<cellOld->row;i++)
	{
		for(j=0;j<cellOld->col;j++)
		{
			if(cellOld->f[i][j].strength>0)
			{
				pt.c=cellOld->f[i][j];
				pt.x=i;
				pt.y=j;
				enqueue(&cellQ,&pt,sizeof(NewCell));
			}
			cellNew.f[i][j].label=cellOld->f[i][j].label;
			cellNew.f[i][j].strength=cellOld->f[i][j].strength;
			cellNew.f[i][j].fv=(float *)malloc(sizeof(float)*cellOld->nfv);
			copyfv(cellOld->f[i][j].fv,cellNew.f[i][j].fv,cellOld->nfv);
			coverImg.f[i][j]=0;
		}
	}
	while(!empty(cellQ))
	{
		nod=dequeue(&cellQ);
		
		if(nod->size==sizeof(NewCell))
		pt2=(NewCell *)nod->data;
		
		getNeighbour2(pt2->x,pt2->y,N,cellOld);
		flag=0;	
		for(k=0;k<NEIGHBOURS;k++)
		{
			dis=calDisMag(cellOld->f[pt2->x][pt2->y].fv,N[k].c.fv,cellOld->nfv);
			gFinal=gFunction(dis,gmax)*N[k].c.strength;
			if(gFinal > cellOld->f[pt2->x][pt2->y].strength)
			{
				(cellNew.f[pt2->x][pt2->y]).label=N[k].c.label;
				(cellNew.f[pt2->x][pt2->y]).strength=gFinal;
			}
		}
		getNeighbour2(pt2->x,pt2->y,N,&cellNew);
		for(k=0;k<NEIGHBOURS;k++)
		{
			dis=calDisMag(cellOld->f[pt2->x][pt2->y].fv,N[k].c.fv,cellOld->nfv);
			gFinal=gFunction(dis,gmax)*cellNew.f[pt2->x][pt2->y].strength;
			if(gFinal > N[k].c.strength && coverImg.f[N[k].x][N[k].y]==0)
			{
				enqueue(&ncellQ,&N[k],sizeof(NewCell));
				coverImg.f[N[k].x][N[k].y]=1;
			}
		}
		if(empty(cellQ))
		{
			ct=0;
			while(!empty(ncellQ))
			{
				nod2=dequeue(&ncellQ);
				pt3=(NewCell *)nod2->data;
				enqueue(&cellQ,pt3,sizeof(NewCell));
				ct++;
				free(nod2);
				free(pt3);
			}
			printf("Iteration no=%d\n",ct++);
			for(i=0;i<cellOld->row;i++)
			{
				for(j=0;j<cellOld->col;j++)
				{
					cellOld->f[i][j].label=cellNew.f[i][j].label;
					cellOld->f[i][j].strength=cellNew.f[i][j].strength;
					coverImg.f[i][j]=0;
				}
			}

		}
		free(nod);
		free(pt2);
	}
	
}
