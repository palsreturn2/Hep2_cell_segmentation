#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include "queue.h"
#include "image.h"
#include "colorimage.h"
#include "growcut.h"
#include "show_image.h"
#include "point.h"
#include "morphology.h"
#include "filtersAndGrads.h"
#include "LBP.h"

#define NEIGHBOURS 4
/********************getSeedImage*********************************************************************************/
void initSeedPixel(GrayImage *img,GrayImage *fgm,GrayImage *bgm,Cell *cellOld)
{
	int i,j,*labels,nL=0,thresh;
	GrayImage temp;
	thresh=graythres(img);
	temp=imlabel(fgm,&labels,&nL);
	for(i=0;i<bgm->row;i++)
	{
		for(j=0;j<bgm->col;j++)
		{
			if(bgm->f[i][j]!=0)
			{
				cellOld->f[i][j].label=1;
				cellOld->f[i][j].strength=1;
				cellOld->f[i][j].intensity=img->f[i][j];
				nL++;
			}
			else if(fgm->f[i][j]!=0)
			{
				cellOld->f[i][j].label=temp.f[i][j]+1;
				cellOld->f[i][j].strength=1;
				cellOld->f[i][j].intensity=img->f[i][j];
				nL++;
			}
			else
			{
				cellOld->f[i][j].label=0;
				cellOld->f[i][j].strength=0;
				cellOld->f[i][j].intensity=img->f[i][j];
			}
		}
	}
	printf("%d\n",nL);
}
/*******************Function to store the neighbours in an array**************************************************/
void getNeighbour(int ix,int iy,struct cell *N,Cell *cellOld)
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
		N[i].label=cellOld->f[mx][my].label;
		N[i].strength=cellOld->f[mx][my].strength;
		N[i].intensity=cellOld->f[mx][my].intensity;
	}
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
		N[i].c.intensity=cellOld->f[mx][my].intensity;
		N[i].x=mx;
		N[i].y=my;
	}
}
/*******************Function to calculate distance b/w two feature Vectors*************************************/
int calDisMag(struct cell cellOld_temp,struct cell N_temp)
{
	int dI;
	dI=cellOld_temp.intensity-N_temp.intensity;
	if(dI<0)
		return (-dI);
	return dI;
}
/******************Function to calculate g value****************************************************************/
float gFunction(int t,int gmax)
{
	float gval;
	gval=(1-((float)(t)/gmax));
	return gval;
}
/******************Function to cell to image****************************************************************/
void cell2img(Cell *cell,GrayImage *img)
{
	int i,j;
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		img->f[i][j]=cell->f[i][j].label;
	}
}
/******************Function to implement the growCut algorithm**************************************************/
void growCut(Cell *cellOld,GrayImage *img)
{
	int ui,count=1,countold,gmax,dis;
	float gFinal;
	FILE *fp=fopen("ComparisonRes.txt","a");
	int i,j,k,flag,search_count=0;
	struct cell *N=(struct cell *)malloc(sizeof(struct cell)*NEIGHBOURS);
	Cell cellNew;
	cellNew.row=cellOld->row;
	cellNew.col=cellOld->col;
	cellNew.f=(struct cell **)malloc(sizeof(struct cell) * cellOld->row);
	for(i=0;i<cellOld->row;i++)
	cellNew.f[i]=(struct cell *)malloc(sizeof(struct cell) * cellOld->col);
	cellNew.row=img->row;
	cellNew.col=img->col;
	
	gmax=0;
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(gmax<cellOld->f[i][j].intensity)
			gmax=cellOld->f[i][j].intensity;
		}
	}
	flag=1;
	while(count>0)
	{
		//printf("No of label updates=%d\n",flag);
		count=0;
		for(i=0;i<cellOld->row;i++)
		{
			for(j=0;j<cellOld->col;j++)
			{
				(cellNew.f[i][j]).label=cellOld->f[i][j].label;
				(cellNew.f[i][j]).strength=cellOld->f[i][j].strength;
				(cellNew.f[i][j]).intensity=cellOld->f[i][j].intensity;			
			
				getNeighbour(i,j,N,cellOld);
				for(k=0;k<NEIGHBOURS;k++)
				{
					dis=calDisMag(cellOld->f[i][j],N[k]);
					gFinal=gFunction(dis,gmax)*N[k].strength;
					search_count++;
					if((gFinal)>cellOld->f[i][j].strength && (gFinal)>cellNew.f[i][j].strength)
					{
						count++;
						(cellNew.f[i][j]).label=N[k].label;
						(cellNew.f[i][j]).strength=gFinal;
						
					}
				}
				flag++;
			}
		}
		for(i=0;i<img->row;i++)
		{
			for(j=0;j<img->col;j++)
			{
				if(cellOld->f[i][j].label!=cellNew.f[i][j].label || cellNew.f[i][j].strength!=cellOld->f[i][j].strength)
				{
					cellOld->f[i][j].label=(cellNew.f[i][j]).label;
					cellOld->f[i][j].strength=(cellNew.f[i][j]).strength;
					cellOld->f[i][j].intensity=(cellNew.f[i][j]).intensity;
				}
			}
		}
	}
	cell2img(cellOld,img);
	fprintf(fp,"SEARCH COUNT of ORIGINAL GROWCUT is: %d\n",search_count);
}
/******************Function to implement the Fast growCut algorithm**************************************************/
void fastgrowCut(Cell *cellOld,GrayImage *img)
{
	int ui,count=1,countold,gmax,dis;
	float gFinal;
	int i,j,k,flag;
	struct cell *N=(struct cell *)malloc(sizeof(struct cell)*NEIGHBOURS);
	Cell cellNew;
	cellNew.row=cellOld->row;
	cellNew.col=cellOld->col;
	cellNew.f=(struct cell **)malloc(sizeof(struct cell) * cellOld->row);
	for(i=0;i<cellOld->row;i++)
	cellNew.f[i]=(struct cell *)malloc(sizeof(struct cell) * cellOld->col);
	cellNew.row=img->row;
	cellNew.col=img->col;
	
	gmax=0;
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(gmax<cellOld->f[i][j].intensity)
			gmax=cellOld->f[i][j].intensity;
		}
	}
	flag=1;
	countold=0;
	while(count>0)
	{
		flag=0;
		printf("Iteration no=%d\n",count);
		count=0;
		countold++;
		for(i=0;i<cellOld->row;i++)
		{
			for(j=0;j<cellOld->col;j++)
			{
				getNeighbour(i,j,N,cellOld);
				(cellNew.f[i][j]).label=cellOld->f[i][j].label;
				(cellNew.f[i][j]).strength=cellOld->f[i][j].strength;
				(cellNew.f[i][j]).intensity=cellOld->f[i][j].intensity;
				for(k=0;k<NEIGHBOURS;k++)
				{
					dis=calDisMag(cellOld->f[i][j],N[k]);
					gFinal=gFunction(dis,gmax)*N[k].strength;
					if(gFinal>cellOld->f[i][j].strength)
					{
						(cellOld->f[i][j]).label=N[k].label;
						(cellOld->f[i][j]).strength=gFinal;
						count++;
					}
				}
			}
		}	
		printf("Iteration no=%d\n",count);
		if(count==0)
		break;
		count=0;
		for(i=cellOld->row-1;i>=0;i--)
		{
			for(j=cellOld->col-1;j>=0;j--)
			{
				getNeighbour(i,j,N,cellOld);
				(cellNew.f[i][j]).label=cellOld->f[i][j].label;
				(cellNew.f[i][j]).strength=cellOld->f[i][j].strength;
				(cellNew.f[i][j]).intensity=cellOld->f[i][j].intensity;
				for(k=0;k<NEIGHBOURS;k++)
				{
					dis=calDisMag(cellOld->f[i][j],N[k]);
					gFinal=gFunction(dis,gmax)*N[k].strength;
					if(gFinal>cellOld->f[i][j].strength)
					{
						(cellOld->f[i][j]).label=N[k].label;
						(cellOld->f[i][j]).strength=gFinal;
						count++;
					}
				}
			}
		}		
	}
	cell2img(cellOld,img);
}
void queue2img(Queue Q,GrayImage *img)
{
	struct node *ptr=Q.front;
	NewCell *cl;
	int i,j,count=0;
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		img->f[i][j]=0;
	}
	while(ptr!=NULL)
	{
		cl=(NewCell *)ptr->data;
		img->f[cl->x][cl->y]=img->maxval;
		ptr=ptr->next;
		count++;
	}
	printf("No of cells=%d\n",count);
	imshow(*img);
}
/******************Function to implement the Faster growCut algorithm**************************************************/
void fastergrowCut(Cell *cellOld,GrayImage *img)	 
{
	Queue cellQ;
	GrayImage coverImg;
	struct node *nod;
	initQueue(&cellQ);
	NewCell pt,*pt2;
	NewCell *N=(NewCell *)malloc(sizeof(NewCell)*NEIGHBOURS);
	initGrayImage(&coverImg,img->row,img->col,img->maxval);
	initQueue(&cellQ);
	int i,j,gmax=0,k,dis,count=0,flag,ct=0;
	float gFinal;
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(gmax<cellOld->f[i][j].intensity)
			gmax=cellOld->f[i][j].intensity;
		}
	}
	for(i=0;i<cellOld->row;i++)
	{
		for(j=0;j<cellOld->col;j++)
		{
			if(cellOld->f[i][j].strength==1)
			{
				pt.c=cellOld->f[i][j];
				coverImg.f[i][j]=img->maxval;
				pt.x=i;
				pt.y=j;
				enqueue(&cellQ,&pt,sizeof(NewCell));
			}
			else
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
			dis=calDisMag(cellOld->f[pt2->x][pt2->y],N[k].c);
			gFinal=gFunction(dis,gmax)*N[k].c.strength;
			if(gFinal > cellOld->f[pt2->x][pt2->y].strength)
			{
				(cellOld->f[pt2->x][pt2->y]).label=N[k].c.label;
				(cellOld->f[pt2->x][pt2->y]).strength=gFinal;
				ct++;
				coverImg.f[pt2->x][pt2->y]=0;
			}
		}
		for(k=0;k<NEIGHBOURS;k++)
		{
			dis=calDisMag(cellOld->f[pt2->x][pt2->y],N[k].c);
			gFinal=gFunction(dis,gmax)*cellOld->f[pt2->x][pt2->y].strength;
			if(gFinal > N[k].c.strength && coverImg.f[N[k].x][N[k].y]==0)
			{
				enqueue(&cellQ,&N[k],sizeof(NewCell));
				coverImg.f[N[k].x][N[k].y]=img->maxval;
			}
		}
		free(nod);
		free(pt2);
	}
	cell2img(cellOld,img);
}
/**********************************************Synchronous growcut algorithm******************/
void syncgrowCut(Cell *cellOld,GrayImage *img)
{
	int i,j,gmax=0,k,dis,count=0,flag,ct=0,search_count=0;
	float gFinal;
	FILE *fp=fopen("ComparisonRes.txt","a");
	Queue cellQ,ncellQ;
	NewCell *N=(NewCell *)malloc(sizeof(NewCell)*NEIGHBOURS);
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
	
	for(i=0;i<cellOld->row;i++)
	{
		for(j=0;j<cellOld->col;j++)
		{
			if(gmax<cellOld->f[i][j].intensity)
			gmax=cellOld->f[i][j].intensity;
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
			cellNew.f[i][j].intensity=cellOld->f[i][j].intensity;
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
			dis=calDisMag(cellOld->f[pt2->x][pt2->y],N[k].c);
			gFinal=gFunction(dis,gmax)*N[k].c.strength;
			/*****ADDITION of RIJULA*****************/
				search_count++;
			if(gFinal > cellOld->f[pt2->x][pt2->y].strength)
			{
				(cellNew.f[pt2->x][pt2->y]).label=N[k].c.label;
				(cellNew.f[pt2->x][pt2->y]).strength=gFinal;
			}
		}
		getNeighbour2(pt2->x,pt2->y,N,&cellNew);
		for(k=0;k<NEIGHBOURS;k++)
		{
			dis=calDisMag(cellOld->f[pt2->x][pt2->y],N[k].c);
			gFinal=gFunction(dis,gmax)*cellNew.f[pt2->x][pt2->y].strength;
			/*****ADDITION of RIJULA*****************/
				search_count++;
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
		//	printf("Iteration no=%d\n",ct++);
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
	cell2img(cellOld,img);
	fprintf(fp,"SEARCH COUNT of fastgrowCut is :%d\n",search_count);
}
	
	
	
