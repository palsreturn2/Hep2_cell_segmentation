#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "image.h"
#include "filtersAndGrads.h"
#include "morphology.h"
#include "segment.h"
#include "show_image.h"
#include "ungrowcut.h"
#include "seedgen.h"

#define SE_VAL 9
#define NEIGHBOURS 4
#define THR 0.70
#define SEED 500


/*************************************Functions*******************************************************************/

void getImage(char *file1,char *file2,GrayImage *seedImage,GrayImage *originalImage)
{
	int i,j,T;
	/*char *pat="001111100011111110111111111111111111111111111111111111111111111011111110001111100";
	int **SE=strel(pat,9);
	GrayImage e,e2,d,g,temp;
	readPGM(file1,&d);
	//imshow(e);
	initGrayImage(&temp,d.row,d.col,d.maxval);
	imshow(d);
	g=histeq(&d);
	
	//imshow(g);
	//printf("press enter to continue..\n");
	//while(getchar()!='\n');
	//imhist(&g);
	T=graythres(&g);
	e=im2bw(g,T);
	(*originalImage)=imerode(&e,SE,SE_VAL);
	(*originalImage)=imerode(originalImage,SE,SE_VAL);
	(*originalImage)=imerode(originalImage,SE,SE_VAL);
	(*originalImage)=imdilate(originalImage,SE,SE_VAL);
	(*originalImage)=imdilate(originalImage,SE,SE_VAL);
	//imfill(originalImage,&temp);
	imshow(*originalImage);
	exit(1);
	for(i=0;i<originalImage->row;i++)
	{
		for(j=0;j<originalImage->col;j++)
		{
			if(originalImage->f[i][j]==originalImage->maxval)
			originalImage->f[i][j]=d.f[i][j];
			else
			originalImage->f[i][j]=0;
		}
	}
	imshow(*originalImage);
	exit(1);
	convexan(originalImage);
	
	segment(originalImage);
	exit(1);
	e=iterSegment(&d,originalImage);
	for(i=0;i<e.row;i++)
	{
		for(j=0;j<e.col;j++)
		{
			if(e.f[i][j]!=0)
			e.f[i][j]=d.f[i][j];
		}
	}
	imshow(e);*/
	readPGM(file1,originalImage);
	originalImage->row=500;
	originalImage->col=500;
	imshow(*originalImage);
	(*seedImage)=genRandomImg(SEED,originalImage->row,originalImage->col);
	//(*seedImage)=getSeedImage3(originalImage,10);
	//imshow(*seedImage);
	
	return;
}
/*********************Function to initialise the seed cells******************************************************/
void initSeedPixel(GrayImage *seedImage,GrayImage *originalImage,Cell *cellOld)
{
	int i,j;
	cellOld->f=(struct cell **)malloc(sizeof(struct cell *)*seedImage->row);
	for(i=0;i<seedImage->row;i++)
	cellOld->f[i]=(struct cell *)malloc(sizeof(struct cell)*seedImage->col);		//allocating space to the cellold matrix
	
	cellOld->row=seedImage->row;
	cellOld->col=seedImage->col;
	
	for(i=0;i<seedImage->row;i++)
	{
		for(j=0;j<seedImage->col;j++)
		{
			if(seedImage->f[i][j]==0)
			{
				cellOld->f[i][j].label=0;//for undefined cells label is zero
				cellOld->f[i][j].strength=0;
				cellOld->f[i][j].intensity=originalImage->f[i][j];
			}
			else 
			{
				cellOld->f[i][j].label=seedImage->f[i][j];
				cellOld->f[i][j].strength=1;
				cellOld->f[i][j].intensity=originalImage->f[i][j];
			}			
		}
	}
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
		mx=mx%cellOld->row;
		my=my%cellOld->col;
		if(mx<0)
		mx=cellOld->row+mx;
		if(my<0)
		my=cellOld->col+my;
		N[i].label=cellOld->f[mx][my].label;
		N[i].strength=cellOld->f[mx][my].strength;
		N[i].intensity=cellOld->f[mx][my].intensity;
	}
}
/*******************Function to calculate distance b/w two feature Vectors*************************************/
float calDisMag(struct cell cellOld_temp,struct cell N_temp)
{
	float dI;
	dI=cellOld_temp.intensity-N_temp.intensity;
	if(dI<0)
		return (-dI);
	return dI;
}
/******************Function to calculate g value**************************************************************/
float gFunction(float t,int gmax)
{
	float gval;
	int i,j;
	
	gval=(1-(t/gmax));
	return gval;
}
/******************Function to implement the growCut algorithm************************************************/
void growCut(Cell *cellOld,Cell *cellNew,GrayImage *originalImage)
{
	int count;
	GrayImage finalImage;
	struct cell N[NEIGHBOURS];
	struct eqclass *class=NULL;
	float gFinal,dis;
	int i,j,k,temp,gmax=0,m,temp1,temp2;
	int flag=0;
	printf("in growCut...\n");
	
	for(i=0;i<SEED;i++)
	insertEq(&class,i);
	
	for(i=0;i<originalImage->row;i++)
	{
		for(j=0;j<originalImage->col;j++)
		gmax=(originalImage->f[i][j]<gmax)?gmax:originalImage->f[i][j];
	}
	count=1;
	cellNew->f=(struct cell **)malloc(sizeof(struct cell *)*originalImage->row);
	
	for(i=0;i<originalImage->row;i++)
	cellNew->f[i]=(struct cell *)malloc(sizeof(struct cell)*originalImage->col);
	
	while(count>0)
	{
		printf("Number of label updates:%d\n",count);
		count=0;
		for(i=0;i<cellOld->row;i++)
		{
			for(j=0;j<cellOld->col;j++)
			{
				cellNew->f[i][j].label=cellOld->f[i][j].label;
				cellNew->f[i][j].strength=cellOld->f[i][j].strength;
				cellNew->f[i][j].intensity=cellOld->f[i][j].intensity;		
				getNeighbour(i,j,N,cellOld);				
				for(k=0;k<NEIGHBOURS;k++)
				{
					dis=calDisMag(cellOld->f[i][j],N[k]);
					gFinal=gFunction(dis,gmax);					
					if((gFinal*(N[k].strength))>THR && cellOld->f[i][j].label!=N[k].label)
					{						
						cellNew->f[i][j].label=N[k].label;
						cellNew->f[i][j].strength=(gFinal*N[k].strength);
						count++;
						if(cellOld->f[i][j].label!=0)
						{			
						//if(cellNew->f[i][j].label!=0)
							mergeEq(&class,cellOld->f[i][j].label,cellNew->f[i][j].label);
						//else
							//cellNew->f[i][j].label=cellOld->f[i][j].label;	
						}
					}
				}		
			}
		}
		for(i=0;i<cellOld->row;i++)
		{
			for(j=0;j<cellOld->col;j++)
			{
				cellOld->f[i][j].label=searchEq(class,cellNew->f[i][j].label);
				cellOld->f[i][j].strength=cellNew->f[i][j].strength;
				cellOld->f[i][j].intensity=cellNew->f[i][j].intensity;
			}
		}
		//getSegment(cellOld,&finalImage,originalImage);
	}
	//printEq(class);
}
/********************Function to get final image segment******************************************************/
void getSegment(Cell *cellOld,GrayImage *finalImage,GrayImage *originalImage)
{
	int i,j;
	finalImage->f=(int **)malloc(sizeof(int *)*cellOld->row);
	for(i=0;i<cellOld->row;i++)
	finalImage->f[i]=(int *)malloc(sizeof(int)*cellOld->col);
	finalImage->row=cellOld->row;
	finalImage->col=cellOld->col;
	finalImage->maxval=255;
	for(i=0;i<cellOld->row;i++)
	{
		for(j=0;j<cellOld->col;j++)
		{
			//if(cellOld->f[i][j].label==1)
			finalImage->f[i][j]=(cellOld->f[i][j].label);
			//printf("%d\n",finalMatrix[i][j]);
		}
	}
	//imshow(*finalImage);
	label2rgb(finalImage);
}



