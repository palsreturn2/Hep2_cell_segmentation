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
#include "gengrowcut.h"
#include "textureOp.h"
#include "LBP.h"
#include "libtest.h"
#include "testResult.h"

void genFgm(GrayImage *img,GrayImage *result)
{
	char *pat="001111100011111110111111111111111111111111111111111111111111111011111110001111100";
	int **SE=diskstrel(STRUC_ELEM),i,j;
	GrayImage tmp,tmp2,Io,Iobr,Iobrcbr,Iobrd,Ie,Ioc,fgm;
	
	
	initGrayImage(&Io,img->row,img->col,img->maxval);
	initGrayImage(&Iobr,img->row,img->col,img->maxval);
	initGrayImage(&Ioc,img->row,img->col,img->maxval);
	initGrayImage(&Iobrcbr,img->row,img->col,img->maxval);
	initGrayImage(&fgm,img->row,img->col,img->maxval);
	
	
	Ie=imerode(img,SE,2*STRUC_ELEM+1);
	printf("Reconstruction started..\n");
	fastgdilate(&Ie,img,&Iobr);
	printf("Reconstruction end..\n");
	
	freeImage(&Io);
	freeImage(&Ioc);
	
	Iobrd=imdilate(&Iobr,SE,2*STRUC_ELEM+1);
	
	tmp=imcomplement(&Iobrd);
	tmp2=imcomplement(&Iobr);
	
	printf("Reconstruction started..\n");
	fastgdilate(&tmp,&tmp2,&Iobrcbr);
	printf("Reconstruction end..\n");	
	Iobrcbr=imcomplement(&Iobrcbr);
	
	printf("Finding regional maximum\n");
	imregionalmax(&Iobrcbr,&fgm);
	
	
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			result->f[i][j]=fgm.f[i][j];
		}
	}
}
void genFinalImage(GrayImage *img,GrayImage *fgm,GrayImage *simg)
{
	GrayImage temp,temp2;
	int *labels,nl,**SE=strel("111111111",3),i,j,k;
	temp=imlabel(fgm,&labels,&nl);
	for(k=0;k<nl;k++)
	{
		for(i=0;i<img->row;i++)
		{
			for(j=0;j<img->col;j++)
			{
				if(simg->f[i][j]==(labels[k]+1))
				{
					temp.f[i][j]=img->maxval;
				}
				else
				{
					temp.f[i][j]=0;
				}
			}
		}
		temp2=imerode(&temp,SE,3);
		for(i=0;i<img->row;i++)
		{
			for(j=0;j<img->col;j++)
			{
				if((temp.f[i][j]-temp2.f[i][j])!=0)
				img->f[i][j]=img->maxval;
			}
		}
		freeImage(&temp2);
	}
	imshow(*img);
}
float testgrowcut(HSIImage *img,GrayImage *fgm,GrayImage *bgm,GrayImage *gtruth)
{
	Cell cellOld;
	GrayImage result2;
	initGrayImage(&result2,img->row,img->col,img->maxval);
	int i,j;
	float acc;
	cellOld.f=(struct cell **)malloc(sizeof(struct cell *) * img->row);
	for(i=0;i<img->row;i++)
	cellOld.f[i]=(struct cell *)malloc(sizeof(struct cell) * img->col);
	cellOld.row=img->row;
	cellOld.col=img->col;
	cellOld.nfv=3;
	
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
				}		
				else
				result2.f[i][j]=img->maxval;
		}
	}
	acc=sorencoeff(result2,gtruth);
	//label2rgb(&result2);
	//genFinalImage(img,fgm,&result2);
	return acc;
}

main(int argc,char **argv)
{
	GrayImage img,fgm,bgm,regmax,temp,gtruth;
	ColorImage cimg;
	float acc;
	HSIImage hsi;
	readPPM(argv[1],&cimg);
	cimshow(cimg);
	initHSIImage(&hsi,cimg.row,cimg->col);
	initGrayImage(&img,cimg.row,cimg->col,cimg->maxval);
	rgb2gray(&cimg,&img);
	rgb2HSI(&cimg,&hsi);
	exit(1);
	genFgm(&img,&fgm);
	//readPGM(argv[2],&fgm);
	imclose(&fgm,diskstrel(3),7,&nl);
	bwareaopen(&nl,&fgm,100);
	//imshow(fgm);
	//fgm.row=300;
	//fgm.col=300;
	convexHullObjects(&fgm,&nl);
	boundaryOuter(&nl,&bgm);
	//imshow(bgm);
	//printf("Foreground marker generated\n");
	//genBgm(&img,&fgm,&bgm);
	//nl=imerode(&fgm,diskstrel(10),21);
	//nl=imcomplement(&fgm);	
	//skeleton(&fgm,&bgm);
	//imshow(bgm);
	//getchar();
	acc=testgrowcut(&hsi,&fgm,&bgm,&gtruth);

	printf("*****************************************************************\n");
	printf("Accuracy of %s is %f\n",argv[1],acc);
	printf("*****************************************************************\n");
}

