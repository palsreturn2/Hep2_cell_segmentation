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

void genFgm(GrayImage *img,GrayImage *result)
{
	char *pat="001111100011111110111111111111111111111111111111111111111111111011111110001111100";
	int **SE=diskstrel(STRUC_ELEM),i,j,thresh;
	GrayImage tmp,tmp2,Io,Iobr,Iobrcbr,Iobrd,Ie,Ioc,fgm,I2;
	
	
	initGrayImage(&Io,img->row,img->col,img->maxval);
	initGrayImage(&Iobr,img->row,img->col,img->maxval);
	initGrayImage(&Ioc,img->row,img->col,img->maxval);
	initGrayImage(&Iobrcbr,img->row,img->col,img->maxval);
	initGrayImage(&fgm,img->row,img->col,img->maxval);
	
	
	Ie=imerode(img,SE,2*STRUC_ELEM+1);
	printf("Reconstruction started..\n");
	fastgdilate(&Ie,img,&Iobr);
	printf("Reconstruction end..\n");
	
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		fgm.f[i][j]=img->f[i][j]-Iobr.f[i][j]>0?img->maxval:0;
	}
	imopen(&fgm,diskstrel(5),7,&Iobrcbr);
	bwareaopen(&Iobrcbr,result,300);
	return;
	
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
	
	thresh=graythres(img);
	freeImage(&tmp);
	tmp=im2bw(*img,thresh);
	
	initGrayImage(&I2,img->row,img->col,img->maxval);
	
	imopen(&tmp,diskstrel(5),11,&I2);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(I2.f[i][j]!=0 || fgm.f[i][j]!=0)
			result->f[i][j]=img->maxval;
			else
			result->f[i][j]=0;
		}
	}
	imshow(*result);
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
GrayImage imcrop(GrayImage *img,GrayImage *obj)
{
	Point2D *pts;
	GrayImage temp;
	Point2D leftx,rightx,topy,downy;
	int i,j,npts=0,c=0;
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(obj->f[i][j]!=0)
			npts++;
		}
	}
	pts=(Point2D *)malloc(sizeof(Point2D)*npts);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(obj->f[i][j]!=0)
			{
				setPoint(pts+c,i,j);
				c++;
			}
		}
	}
	leftx=rightx=topy=downy=pts[0];
	for(i=0;i<npts;i++)
	{
		if(leftx.x>pts[i].x)
		leftx=pts[i];
		if(rightx.x<pts[i].x)
		rightx=pts[i];
		if(topy.y>pts[i].y)
		topy=pts[i];
		if(downy.y<pts[i].y)
		downy=pts[i];
	}
	initGrayImage(&temp,rightx.x-leftx.x+1,downy.y-topy.y+1,img->maxval);
	for(i=leftx.x;i<=rightx.x;i++)
	{
		for(j=topy.y;j<=downy.y;j++)
		{
			temp.f[i-leftx.x][j-topy.y]=img->f[i][j];
		}
	}
	return temp;
}
void textureTest(GrayImage *img,int k,FILE *fp)
{
	int **matrix;
	int i;
	//fp=fopen("texture.ods","a");
	float con,uni,homo,en,cor,dm,sumAverage,sumEn,sumVar,diffEn,diffVar;
	float count=0;
	float **promatrix;
	matrix=(int **)malloc(sizeof(int *)*LEVEL);
	for(i=0;i<LEVEL;i++)
	{
		matrix[i]=(int *)malloc(sizeof(int )*LEVEL);
	}
	
	promatrix=(float  **)malloc(sizeof(float *)*LEVEL);
	for(i=0;i<LEVEL;i++)
	{
		promatrix[i]=(float *)malloc(sizeof(float )*LEVEL);
	}

	computeProb(img,promatrix,1,0);
	con=constrast(promatrix);
	uni=uniformity(promatrix);
	homo=homogeneity(promatrix);
	en=entropy(promatrix);
	cor=correlation(promatrix);
	dm=diffMoment(promatrix);
	sumAverage=sumAvg(promatrix);
	sumEn=sumEntropy(promatrix);
	sumVar=sumVariance(promatrix);
	diffEn=diffEntropy(promatrix);
	diffVar=diffVariance(promatrix);
	fprintf(fp,"%d\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n",k,con,uni,homo,en,cor,dm,sumAverage,sumEn,sumVar,diffEn,diffVar);
}
void testLBP(GrayImage *img,int k,FILE *fp)
{
	GrayImage result;
	initGrayImage(&result,img->row,img->col,img->maxval);
	resultLBP(img,&result);
	textureTest(&result,k,fp);
}	

void genFeatures(GrayImage *img,GrayImage *fgm,GrayImage *simg)
{
	GrayImage temp,cropped,LBPresult;
	initGrayImage(&LBPresult,img->row,img->col,img->maxval);
	int *labels,nl,i,j,k;
	int *arr;
	arr=(int *)malloc(sizeof(int)*9);	
	temp=imlabel(fgm,&labels,&nl);
	FILE *fp=fopen("featuresLBP.ods","w");
	fprintf(fp,"%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n","Label","Constrast","uniformity","homogeneity","entropy","correlation","diffMoment","sumAverage","sumEntropy","sumVariance","diffEntropy","diffVariance");

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
		cropped=imcrop(img,&temp);
		//Enter your function
		if(temp.row>1 && temp.col>1)
		{	
			printf("calculating LBP for %d\n",k);
			testLBP(&cropped,k,fp);
		}
			
		//End
	}
	fclose(fp);
	//Enter your function
	//End
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
	//label2rgb(&result2);
	//genFinalImage(img,fgm,&result2);
	//genFeatures(img,fgm,&result2);
	//imshow(result2);
	return acc;
}

main(int argc,char **argv)
{
	GrayImage img,fgm,bgm,gtruth,nl,result;
	float acc;
	readPGM(argv[1],&img);
	readPGM(argv[2],&gtruth);
	initGrayImage(&fgm,img.row,img.col,img.maxval);
	initGrayImage(&bgm,img.row,img.col,img.maxval);
	initGrayImage(&nl,img.row,img.col,img.maxval);
	genFgm(&img,&fgm);
	//readPGM(argv[2],&fgm);
	//imclose(&fgm,diskstrel(3),7,&nl);
	//bwareaopen(&nl,&fgm,100);
	//fgm.row=300;
	//fgm.col=300;
	convexHullObjects(&fgm,&nl);
	nl=imdilate(&nl,diskstrel(5),11);
	boundaryOuter(&nl,&bgm);
	//printf("Foreground marker generated\n");
	//genBgm(&img,&fgm,&bgm);
	//nl=imerode(&fgm,diskstrel(10),21);
	//nl=imcomplement(&fgm);	
	//skeleton(&fgm,&bgm);
	//imshow(bgm);
	//getchar();
	//readPGM(argv[3],&bgm);
	acc=testgrowcut(&img,&fgm,&bgm,&gtruth);
	//savePGM(img,argv[5]);
	printf("*****************************************************************\n");
	printf("Accuracy of %s is %f\n",argv[1],acc);
	printf("*****************************************************************\n");
}
