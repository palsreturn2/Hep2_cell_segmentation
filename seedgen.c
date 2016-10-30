#include<stdio.h>
#include<stdlib.h>
#include"image.h"
#include <math.h>
#include "morphology.h"
#include "seedgen.h"
#define SE_MATRIX {{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1}}

#define SE_VAL 7
#define SEEDTHR 10
GrayImage genRandomImg(int seed,int rows,int cols)
{
	GrayImage img;
	int i,j;
	
	img.f=(int **)malloc(sizeof(int *)*rows);
	for(i=0;i<rows;i++)
	img.f[i]=(int *)malloc(sizeof(int)*cols);
	img.row=rows;
	img.col=cols;
	img.maxval=seed;
	for(i=0;i<rows;i++)
	{
		for(j=0;j<cols;j++)
		img.f[i][j]=0;
	}
	
	for(i=0;i<seed;i++)
	{
		img.f[rand()%rows][rand()%cols]=i+1	;
		srand(i);
	}
	return img;
}
GrayImage getSeedImage(char *file)
{
	GrayImage img;
	int i,j;
	readPGM(file,&img);
	for(i=0;i<img.row;i++)
	{
		for(j=0;j<img.col;j++)
		{
			if(img.f[i][j]==0)
			img.f[i][j]=0;
			else
			img.f[i][j]=1;
		}
	}
	printf("%d\n",img.maxval);
	
	return img;
}
/*GrayImage getSeedImage2(char *file)
{
	GrayImage img,bwimg,result;
	int level;
	int se[SE_VAL][SE_VAL]=SE_MATRIX;
	int *T=&se[0][0];
	readPGM(file,&img);
	level=graythres(&img);
	bwimg=im2bw(img,level);
	result=imerode(&bwimg,&T,SE_VAL);
	return bwimg;
}*/
GrayImage getSeedImage3(GrayImage *originalImage,int segment)
{
	int i,j,m,n,ix,iy,posx,posy,l,count;
	GrayImage seedImage;
	count=0;
	seedImage.row=originalImage->row;
	seedImage.col=originalImage->col;
	seedImage.f=(int **)malloc(sizeof(int *)*originalImage->row);
	for(i=0;i<originalImage->row;i++)
	seedImage.f[i]=(int *)malloc(sizeof(int)*originalImage->col);
	
	for(i=0;i<originalImage->row;i++)
	{
		for(j=0;j<originalImage->col;j++)
		seedImage.f[i][j]=0;
	}
	
	ix=0;
	iy=0;
	for(i=segment;i<originalImage->row;i+=segment)
	{		
		for(j=segment;j<originalImage->col;j+=segment)
		{			
			l=0;
			for(m=ix;m<i;m++)
			{
				for(n=iy;n<j;n++)
				{
					if(l<originalImage->f[m][n])
					{
						l=originalImage->f[m][n];
						posx=m;
						posy=n;
					}
				}
			}
			if(posx<originalImage->row && posy<originalImage->col)
			seedImage.f[posx][posy]=l;
			iy=j;
		}
		ix=i;
	}
	seedImage.maxval=originalImage->maxval;
	return seedImage;
}
	
	
	
/*main(int argc,char **argv)
{
	GrayImage img;
	img=getSeedImage2(argv[1]);
	imshow(img);
}*/
	


