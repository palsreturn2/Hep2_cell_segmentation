#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "image.h"
#include "colorimage.h"

#define pi 3.14159265359

void initColorImage(ColorImage *cimg,int row,int col,int maxval)
{
	int i,j;
	cimg->row=row;
	cimg->col=col;
	cimg->maxval=maxval;
	
	cimg->f[0]=(int **)malloc(sizeof(int *)*row);
	cimg->f[1]=(int **)malloc(sizeof(int *)*row);
	cimg->f[2]=(int **)malloc(sizeof(int *)*row);
	for(i=0;i<row;i++)
	{
		cimg->f[0][i]=(int *)malloc(sizeof(int)*col);
		cimg->f[1][i]=(int *)malloc(sizeof(int)*col);
		cimg->f[2][i]=(int *)malloc(sizeof(int)*col);
	}
}
void initHSIImage(HSIImage *cimg,int row,int col)
{
	int i,j;
	cimg->row=row;
	cimg->col=col;
	
	cimg->f[0]=(float **)malloc(sizeof(float *)*row);
	cimg->f[1]=(float **)malloc(sizeof(float *)*row);
	cimg->f[2]=(float **)malloc(sizeof(float *)*row);
	for(i=0;i<row;i++)
	{
		cimg->f[0][i]=(float *)malloc(sizeof(float)*col);
		cimg->f[1][i]=(float *)malloc(sizeof(float)*col);
		cimg->f[2][i]=(float *)malloc(sizeof(float)*col);
	}
}
void rgb2gray(ColorImage *cimg,GrayImage *img)
{
	int i,j;
	for(i=0;i<cimg->row;i++)
	{
		for(j=0;j<cimg->col;j++)
		img->f[i][j]=(cimg->f[0][i][j]+cimg->f[1][i][j]+cimg->f[2][i][j])/3;
	}
}
void rgb2HSI(ColorImage *cimg,HSIImage *hsi)
{
	int i,j,R,G,B,min;
	float temp,theta;
	for(i=0;i<cimg->row;i++)
	{
		for(j=0;j<cimg->col;j++)
		{
			hsi->f[2][i][j]=(cimg->f[0][i][j]+cimg->f[1][i][j]+cimg->f[2][i][j])/(3.0*cimg->maxval);
			R=cimg->f[0][i][j];
			G=cimg->f[1][i][j];
			B=cimg->f[2][i][j];
			
			temp=(((R-G)+(R-B))/2.0)/sqrt(pow(R-G,2)+(R-B)*(G-B));
			theta=acos(temp);
			if(B<=G)
			hsi->f[0][i][j]=(temp/(2.0*pi));
			else
			hsi->f[0][i][j]=1-(temp/(2.0*pi));
			
			min=R;
			if(min>G)
			min=G;
			if(min>B)
			min=B;
			hsi->f[1][i][j]=1-(3.0*min/(R+G+B));	
		}
	}			
}

