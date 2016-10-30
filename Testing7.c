#include<stdio.h>
#include<stdlib.h>
#include <math.h>
#include "point.h"
#include "queue.h"
#include "image.h"
#include "colorimage.h"
#include "show_image.h"
#include "morphology.h"
#include "filtersAndGrads.h"
#include "texture.h"
#include "seedgen.h"
#include "growcut.h"
#include "textureOp.h"
#include "LBP.h"
#include "libtest.h"
#include "testResult.h"

#define STRUC_ELEM 30
void genmarker(GrayImage *img,int SEG,GrayImage *result)
{
	int i,j,m,n,min=img->maxval;
	for(i=0;i<img->row;i+=SEG)
	{
		for(j=0;j<img->col;j+=SEG)
		{
			min=img->maxval;
			for(m=i;m<=i+SEG;m++)
			{
				for(n=j;n<=j+SEG;n++)
				{
					if(m<img->row && n<img->col && img->f[m][n]<min)
					min=img->f[m][n];
				}
			}
			for(m=i;m<=i+SEG;m++)
			{
				for(n=j;n<=j+SEG;n++)
				{
					if(m<img->row && n<img->col)
					result->f[m][n]=min;
				}
			}
		}
	}
}
					
void genFgm(GrayImage *img,GrayImage *result)
{
	char *pat="001111100011111110111111111111111111111111111111111111111111111011111110001111100";
	int **SE=diskstrel(STRUC_ELEM),i,j,thresh;
	GrayImage tmp,tmp2,Io,Iobr,Iobrcbr,Iobrd,Ie,Ioc,fgm,I2;
	
	
	initGrayImage(&Ie,img->row,img->col,img->maxval);
	initGrayImage(&Iobr,img->row,img->col,img->maxval);
	initGrayImage(&Ioc,img->row,img->col,img->maxval);
	initGrayImage(&Iobrcbr,img->row,img->col,img->maxval);
	initGrayImage(&fgm,img->row,img->col,img->maxval);
	
	
	Ie=imerode(img,SE,2*STRUC_ELEM+1);
	//genmarker(img,70,&Ie);
	printf("Reconstruction started..\n");
	fastgdilate(&Ie,img,&Iobr);
	printf("Reconstruction end..\n");
	
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		fgm.f[i][j]=img->f[i][j]-Iobr.f[i][j]>0?img->maxval:0;
	}
	imopen(&fgm,diskstrel(5),11,&Iobrcbr);
	bwareaopen(&Iobrcbr,result,300);
	imshow(*result);
	freeImage(&Iobrcbr);
	freeImage(&Ie);
	freeImage(&fgm);
	freeImage(&Ioc);
}
void genBgm(GrayImage *img,GrayImage *fgm,GrayImage *bgm)
{
	GrayImage temp,te;
	int **SE=diskstrel(8),i,j;
	temp=histeq(img);
	
	temp=im2bw(temp,graythres(&temp));
	initGrayImage(&te,img->row,img->col,img->maxval);
	imopen(&temp,SE,2*8+1,&te);
	
	//convexHullObjects(&te,&temp);
	te=imdilate(&te,diskstrel(3),7);
	//freeImage(&temp);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(fgm->f[i][j]!=0 && te.f[i][j]==0)
			te.f[i][j]=img->maxval;
		}
	}
	temp=imdilate(&te,strel("111111111",3),3);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			bgm->f[i][j]=temp.f[i][j]-te.f[i][j];
		}
	}	
	freeImage(&temp);
	freeImage(&te);
}
void genBgm2(GrayImage *fgm,GrayImage *bgm)
{
	GrayImage temp,tmp2;
	int i,j;
	temp=imdilate(fgm,diskstrel(10),21);
	tmp2=imdilate(&temp,strel("111111111",3),3);
	for(i=0;i<fgm->row;i++)
	{
		for(j=0;j<fgm->col;j++)
		bgm->f[i][j]=tmp2.f[i][j]-temp.f[i][j];
	}
}
void compGtruth(GrayImage *img,GrayImage *gtruth)
{
	GrayImage result;
	initGrayImage(&result,img->row,img->col,img->maxval);
	int i,j;
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(img->f[i][j]!=0 && gtruth->f[i][j]!=0)
			result.f[i][j]=200;
			else if(img->f[i][j]!=0)
			result.f[i][j]=100;
			else if(gtruth->f[i][j]!=0)
			result.f[i][j]=50;
			else
			result.f[i][j]=0;
		}
	}
	label2rgb(&result);
	freeImage(&result);
}			

float testorigrowcut(GrayImage *img,GrayImage *fgm,GrayImage *bgm,GrayImage *gtruth)
{
	GrayImage result1,result2;
	initGrayImage(&result1,img->row,img->col,img->maxval);
	initGrayImage(&result2,img->row,img->col,img->maxval);
	Cell cellOld;
	int i,j;
	float acc=0;
	cellOld.f=(struct cell **)malloc(sizeof(struct cell *) * img->row);
	for(i=0;i<img->row;i++)
	cellOld.f[i]=(struct cell *)malloc(sizeof(struct cell) * img->col);
	cellOld.row=img->row;
	cellOld.col=img->col;
	//initSeedPixel(img,fgm,bgm,&cellOld);
	//printf("Starting growcut\n");
	//growCut(&cellOld,&result1);
	initSeedPixel(img,fgm,bgm,&cellOld);
	printf("Starting fast growcut\n");
	growCut(&cellOld,&result2);
	printf("Ending growcut\n");
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
				if(result2.f[i][j]==1)
				{
					result2.f[i][j]=0;
					result1.f[i][j]=0;
				}		
				else
				result1.f[i][j]=img->maxval;
		}
	}
	removecomp(&result1,gtruth,&result2);
	acc=sorencoeff(&result2,gtruth);
	compGtruth(&result2,gtruth);
	//label2rgb(&result2);
	//genFinalImage(img,fgm,&result2);
	//genFeatures(img,fgm,&result2);
	//imshow(result2);
	return acc;
}

float testgrowcut(GrayImage *img,GrayImage *fgm,GrayImage *bgm,GrayImage *gtruth)
{
	GrayImage result1,result2;
	initGrayImage(&result1,img->row,img->col,img->maxval);
	initGrayImage(&result2,img->row,img->col,img->maxval);
	Cell cellOld;
	int i,j;
	float acc=0;
	cellOld.f=(struct cell **)malloc(sizeof(struct cell *) * img->row);
	for(i=0;i<img->row;i++)
	cellOld.f[i]=(struct cell *)malloc(sizeof(struct cell) * img->col);
	cellOld.row=img->row;
	cellOld.col=img->col;
	//initSeedPixel(img,fgm,bgm,&cellOld);
	//printf("Starting growcut\n");
	//growCut(&cellOld,&result1);
	initSeedPixel(img,fgm,bgm,&cellOld);
	printf("Starting fast growcut\n");
	syncgrowCut(&cellOld,&result2);
	printf("Ending growcut\n");
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
				if(result2.f[i][j]==1)
				{
					result2.f[i][j]=0;
					result1.f[i][j]=0;
				}		
				else
				result1.f[i][j]=img->maxval;
		}
	}
	
	removecomp(&result1,gtruth,&result2);
	acc=sorencoeff(&result2,gtruth);
	compGtruth(&result2,gtruth);
	
	//genFinalImage(img,fgm,&result2);
	//genFeatures(img,fgm,&result2);
	//imshow(result2);
	return acc;
}

main(int argc,char *argv[])
{
	GrayImage img,fgm,bgm,gtruth,nl,result;
	float acc;
	int i,j,seed_count=0;
	FILE *fp=fopen("ComparisonRes.txt","a");
	readPGM(argv[1],&img);
	readPGM(argv[2],&gtruth);
	initGrayImage(&fgm,img.row,img.col,img.maxval);
	initGrayImage(&bgm,img.row,img.col,img.maxval);
	initGrayImage(&nl,img.row,img.col,img.maxval);
	genFgm(&img,&fgm);
	for(i=0;i<img.row;i++)
	{
		for(j=0;j<img.col;j++)
		{
			if(fgm.f[i][j]!=0)
				seed_count++;
		}
	}
	convexHullObjects(&fgm,&nl);
	nl=imdilate(&nl,diskstrel(5),11);
	boundaryOuter(&nl,&bgm);
	for(i=0;i<img.row;i++)
	{
		for(j=0;j<img.col;j++)
		{
			if(bgm.f[i][j]!=0)
				seed_count++;
		}
	}
	acc=testgrowcut(&img,&fgm,&bgm,&gtruth);
	acc=testorigrowcut(&img,&fgm,&bgm,&gtruth);
	//savePGM(img,argv[5]);
	fprintf(fp,"*****************************************************************\n");
	fprintf(fp,"Total number of seeds :- %d\n",seed_count);
	fprintf(fp,"Total number of pixels :- %d\n",(img.row*img.col));
	fprintf(fp,"*****************************************************************\n");
}
