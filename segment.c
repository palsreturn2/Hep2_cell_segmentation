#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"
#include "colorimage.h"
#include "filtersAndGrads.h"
#include "morphology.h"
#include "segment.h"
#include "show_image.h"
#include "point.h"


#define SE_VAL 7
int findPerimeter(GrayImage *img)
{
	int i,j,count=0;
	int *T;
	GrayImage result;
	char *pat="111111111";
	int **se=strel(pat,3);
	result=imerode(img,se,3);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			result.f[i][j]=img->f[i][j]-result.f[i][j];
			if(result.f[i][j]!=0)
				count++;
		}
	}
	return count;
}
void boundary(GrayImage *img,GrayImage *result)
{
	int **se=strel("111111111",3);
	GrayImage temp;
	int i,j;
	initGrayImage(&temp,img->row,img->col,img->maxval);
	temp=imerode(img,se,3);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			result->f[i][j]=img->f[i][j]-temp.f[i][j];
		}
	}
}
	
int findArea(GrayImage *img)
{
	int i,j,count=0;
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(img->f[i][j]!=0)
			count++;
		}
	}
	return count;
}
void divideSegment(GrayImage *original,GrayImage *img,GrayImage *final,int iter)
{
	char *pat="1111111111111111111111111111111111111111111111111111111111111111";
	static int imageNo=0;
	int t,r,i,j,per,n=0,area;
	char *saveloc="/home/robocop/workplace/unsup_growcut/images/13/final";
	char filename[100];
	filename[0]='\0';
	strcat(filename,saveloc);
	t=imageNo;
	for(i=0;i<100 && filename[i]!='\0';i++);
	while(t!=0)
	{
		r=t%10;
		filename[i]=(char)(r+48);
		i++;
		t=t/10;
	}
	filename[i]='\0';
	strcat(filename,".pgm");
	int **SE=strel(pat,8);
	
	GrayImage temp,L,tp2;
	//per=findPerimeter(img);
	area=findArea(img);
	if(area<AREATHRES)
	{
		while(iter!=0)
		{
			temp=imdilate(img,SE,8);
			for(i=0;i<img->row;i++)
			{
				for(j=0;j<img->col;j++)
				{
					img->f[i][j]=temp.f[i][j];
				}
			}
			freeImage(&temp);
			iter--;
		}
		initGrayImage(&tp2,img->row,img->col,img->maxval);
		for(i=0;i<img->row;i++)
		{
			for(j=0;j<img->col;j++)
			{
				if(img->f[i][j]!=0)
				{
					final->f[i][j]=original->f[i][j];
					tp2.f[i][j]=original->f[i][j];
				}
				else
				tp2.f[i][j]=0;
			}
		}
		savePGM(tp2,filename);
		freeImage(&tp2);
		imageNo=imageNo+1;
		return;
	}
	
	int *labels;
	temp=imerode(img,SE,8);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			img->f[i][j]=temp.f[i][j];
		}
	}
	freeImage(&temp);
	L=imlabel(img,&labels,&n);
	
	if(n==1)
	divideSegment(original,&temp,final,iter+1);
	else
	{
		GrayImage T;
		while((n--)>=1)
		{
			initGrayImage(&T,img->row,img->col,img->maxval);
			for(i=0;i<img->row;i++)
			{
				for(j=0;j<img->col;j++)
				{
					T.f[i][j]=(L.f[i][j]==labels[n])?img->maxval:0;
				}
			}
			divideSegment(original,&T,final,iter+1);
			//freeImage(&T);
			
		}
	}
	free(labels);
	return;
}		
	
GrayImage iterSegment(GrayImage *original,GrayImage *img)
{
	GrayImage temp,temp2,LI,final;
	int n=0,i,j,iter;
	int *labels;
	initGrayImage(&final,img->row,img->col,img->maxval);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		final.f[i][j]=0;
	}
	LI=imlabel(img,&labels,&n);
	label2rgb(&LI);
	//exit(1);
	while((n--)>=1)
	{
		initGrayImage(&temp,img->row,img->col,img->maxval);
		for(i=0;i<img->row;i++)
		{
			for(j=0;j<img->col;j++)
			{
				temp.f[i][j]=(LI.f[i][j]==(labels[n]))?img->maxval:0;
			}
		}
		//imshow(temp);
		printf("Iteration no=%d\n",n);
		if(findPerimeter(&temp)>100)
		{
			initGrayImage(&temp2,img->row,img->col,img->maxval);
			//imshow(temp2);
			for(i=0;i<img->row;i++)
			{
				for(j=0;j<img->col;j++)
				temp2.f[i][j]=0;
			}
			iter=0;
			divideSegment(original,&temp,&temp2,iter);
			for(i=0;i<img->row;i++)
			{
				for(j=0;j<img->col;j++)
				if(temp2.f[i][j]!=0)
				final.f[i][j]=temp2.f[i][j];
			}
			freeImage(&temp2);
		}
		//if(n%10==0)
		//imshow(final);
		freeImage(&temp);
	}
	free(labels);
	return final;
}
GrayImage boundaryImage(GrayImage *img)
{
	char *se="111111111";
	int **SE=strel(se,3);
	int i,j;
	GrayImage temp,final;
	initGrayImage(&final,img->row,img->col,img->maxval);
	temp=imerode(img,SE,3);	
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			final.f[i][j]=img->f[i][j]-temp.f[i][j];
		}
	}
	return final;
}
double **transformation(GrayImage *img)
{
	double **T;
	int *pts[2];
	int i,j,per,count=0;
	GrayImage bound;
	T=(double **)malloc(sizeof(double *)*img->row);
	for(i=0;i<img->row;i++)
	T[i]=(double *)malloc(sizeof(double)*img->col);
	bound=boundaryImage(img);
	per=findPerimeter(img);
	pts[0]=(int *)malloc(sizeof(int)*per);
	pts[1]=(int *)malloc(sizeof(int)*per);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(bound.f[i][j]!=1)
			{
				pts[0][count]=i;
				pts[1][count]=j;
			}
		}
	}
}	
void segment(GrayImage *img)
{
	int *labels,n=0;
	int i,j,l,count;
	GrayImage L=imlabel(img,&labels,&n);
	GrayImage temp,chull,comp;
	Point2D *p1,*p2;
	initGrayImage(&temp,img->row,img->col,img->maxval);
	for(l=0;l<n;l++)
	{
		for(i=0;i<img->row;i++)
		{
			for(j=0;j<img->col;j++)
			{
				if(L.f[i][j]==labels[l])
				comp.f[i][j]=img->maxval;
				else
				comp.f[i][j]=0;
			}
		}
		//chull=convexHull(&comp);
		boundary(&comp,&temp);
		p1=(Point2D *)malloc(sizeof(Point2D)*findPerimeter(&comp));
		count=0;
		for(i=0;i<img->row;i++)
		{
			for(j=0;j<img->col;j++)
			{
				if(temp.f[i][j]!=0);
				//setPoint(&p1[count++],i,j);
			}
		}
		boundary(&chull,&temp);
		p2=(Point2D *)malloc(sizeof(Point2D)*findPerimeter(&chull));
		count=0;
		for(i=0;i<img->row;i++)
		{
			for(j=0;j<img->col;j++)
			{
				if(temp.f[i][j]!=0);
				//setPoint(&p2[count++],i,j);
			}
		}
	}
}
void convexan(GrayImage *img)
{
	int **pat1=strel("112102112",3);
	GrayImage temp;
	temp=hitormiss(img,pat1,3,6);
	imshow(temp);
}
	
