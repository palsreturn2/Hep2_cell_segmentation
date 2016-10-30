#include<stdio.h>
#include<stdlib.h>
#include "image.h"
#define SE_VAL 5
void imerode(GrayImage *originalImage,GrayImage *finalImage,int SE[SE_VAL][SE_VAL])
{
	int i,j,k,l;
	int temp[SE_VAL][SE_VAL];
	finalImage->row=originalImage->row;
	finalImage->col=originalImage->col;
	finalImage->maxval=originalImage->maxval;
	finalImage->f=(int **)malloc(sizeof(int *)*originalImage->row);
	for(i=0;i<originalImage->row;i++)
		finalImage->f[i]=(int *)malloc(sizeof(int)*originalImage->col);
	for(i=0;i<originalImage->row;i++)
	{
		for(j=0;j<originalImage->col;j++)
		{
			for(s=0;s<SE_VAL;s++)
			{
				for(t=0;t<SE_VAL;t++)
				{
					temp[s][t]=originalImage->f[i+s][j+t];
				}
			}
			for(i=0;i<SE_VAL;i++)
			{
				for(j=0;j<SE_VAL;j++)
				{
					finalImage->f[i][j]=(temp>finalImage->f[i][j])?finalImage->f[i][j]:temp;
				}
			}
		}
	}
}
void imdilate(GrayImage *originalImage,GrayImage *finalImage,int SE[SE_VAL][SE_VAL])
{
	int i,j,k,l;
	int temp[SE_VAL][SE_VAL];
	finalImage->row=originalImage->row;
	finalImage->col=originalImage->col;
	finalImage->f=(int **)malloc(sizeof(int *)*originalImage->row);
	for(i=0;i<originalImage->row;i++)
		finalImage->f[i]=(int *)malloc(sizeof(int)*originalImage->col);
	for(i=0;i<originalImage->row;i++)
	{
		for(j=0;j<originalImage->col;j++)
		{
			for(s=0;s<SE_VAL;s++)
			{
				for(t=0;t<SE_VAL;t++)
				{
					temp[s][t]=originalImage->f[i-s][j-t];
				}
			}
			for(i=0;i<SE_VAL;i++)
			{
				for(j=0;j<SE_VAL;j++)
				{
					finalImage->f[i][j]=(temp<finalImage->f[i][j])?finalImage->f[i][j]:temp;
				}
			}

		}
	}
}
