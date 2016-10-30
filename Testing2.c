#include<stdio.h>
#include<stdlib.h>
#include <math.h>
#include "point.h"
#include "image.h"
#include "show_image.h"
#include "morphology.h"
#include "filtersAndGrads.h"
#include "texture.h"
#include "seedgen.h"
#include "growcut.h"

void genFgm(GrayImage *img,GrayImage *result)
{
	char *pat="001111100011111110111111111111111111111111111111111111111111111011111110001111100";
	int **SE=diskstrel(20),i,j;
	GrayImage tmp,tmp2,Io,Iobr,Iobrcbr,Iobrd,Ie,Ioc,fgm;
	
	
	initGrayImage(&Io,img->row,img->col,img->maxval);
	initGrayImage(&Iobr,img->row,img->col,img->maxval);
	initGrayImage(&Ioc,img->row,img->col,img->maxval);
	initGrayImage(&Iobrcbr,img->row,img->col,img->maxval);
	initGrayImage(&fgm,img->row,img->col,img->maxval);
	
		
	Ie=imerode(img,SE,41);
	
	printf("Reconstruction started..\n");
	fastgdilate(&Ie,img,&Iobr);
	printf("Reconstruction end..\n");
	
	
	freeImage(&Io);
	freeImage(&Ioc);
	
	
	Iobrd=imdilate(&Iobr,SE,41);
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
void genBgm(GrayImage *img,GrayImage *fgm,GrayImage *bgm)
{
	GrayImage temp,te;
	int **SE=diskstrel(8),i,j;
	temp=histeq(img);
	temp=im2bw(temp,graythres(&temp));
	initGrayImage(&te,img->row,img->col,img->maxval);
	imopen(&temp,SE,2*8+1,&te);
	//imfill(&te,&temp);
	temp=te;
	te=imdilate(&te,diskstrel(3),7);
	freeImage(&temp);
	temp=imdilate(&te,strel("111111111",3),3);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(fgm->f[i][j]!=0 && te.f[i][j]==0)
			fgm->f[i][j]=0;
		}
	}
	
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			bgm->f[i][j]=temp.f[i][j]-te.f[i][j];
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
GrayImage imcrop(GrayImage *img,GrayImage *obj)
{
	Point2D *pts;
	GrayImage temp;
	Point2D left,right,top,down;
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
	left=right=top=down=pts[0];
	for(i=0;i<npts;i++)
	{
		if(left.y>pts[i].y)
		left=pts[i];
		if(right.y<pts[i].y)
		right=pts[i];
		if(top.x>pts[i].x)
		top=pts[i];
		if(down.x<pts[i].x)
		down=pts[i];
	}
	initGrayImage(&temp,down.x-top.x+1,right.y-left.y+1,img->maxval);
	for(i=top.x;i<=down.x;i++)
	{
		for(j=left.y;j<right.y;j++)
		{
			temp.f[i-top.x][j-left.y]=img->f[i][j];
		}
	}
	return temp;
}
void genFeatures(GrayImage *img,GrayImage *simg,GrayImage *fgm)
{
	GrayImage temp,cropped;
	int *labels,nl,i,j,k;
	temp=imlabel(fgm,&labels,&nl);
	freeImage(&temp);
	initGrayImage(&temp,img->row,img->col,img->maxval);
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
		imshow(cropped);
		freeImage(&cropped);
		getchar();
	}
}			
void testgrowcut(GrayImage *img,GrayImage *fgm,GrayImage *bgm)
{
	GrayImage result;
	initGrayImage(&result,img->row,img->col,img->maxval);
	Cell cellOld;
	int i,j;
	cellOld.f=(struct cell **)malloc(sizeof(struct cell *) * img->row);
	for(i=0;i<img->row;i++)
	cellOld.f[i]=(struct cell *)malloc(sizeof(struct cell) * img->col);
	cellOld.row=img->row;
	cellOld.col=img->col;
	initSeedPixel(img,fgm,bgm,&cellOld);
	fastgrowCut(&cellOld,&result);
	label2rgb(&result);
	//genFinalImage(img,fgm,&result);
	//genFeatures(img,&result,fgm);
}

main(int argc,char **argv)
{
	GrayImage img,fgm,bgm,nI,temp;
	readPGM(argv[1],&img);
	//img.row=300;
	//img.col=300;
	initGrayImage(&fgm,img.row,img.col,img.maxval);
	//initGrayImage(&temp,img.row,img.col,img.maxval);
	initGrayImage(&bgm,img.row,img.col,img.maxval);
	//initGrayImage(&nI,img.row,img.col,img.maxval);
	//genFgm(&img,&temp);
	readPGM(argv[2],&temp);
	//fgm.row=300;
	//fgm.col=300;
	imopen(&temp,diskstrel(4),9,&fgm);
	genBgm(&img,&fgm,&bgm);
	imshow(fgm);
	testgrowcut(&img,&fgm,&bgm);
}
