#include "image.h"
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include <math.h>
#include "Eqclass.h"
#include "point.h"

#define PI 22.0/7.0

int graythres(GrayImage *img)
{
	int i,j;
	int sum=0,sumB=0,wb=0,wf=0;
	double mB,mF,max=0.0,between=0.0,threshold1=0.0,threshold2=0.0;
	int hist[256];
	//FILE *fp=fopen("hist.xls","w+");
	for(i=0;i<256;i++)
	hist[i]=0;
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			hist[img->f[i][j]]++;
		}
	}
	for(i=1;i<256;i++)
	{
		sum+=((i)*hist[i]);
		//fprintf(fp,"%d %d\n",i,hist[i]);
	}
	//fclose(fp);
	for(i=0;i<256;i++)
	{
		wb+=hist[i];
		if(wb==0)
			continue;
		wf=(img->row*img->col)-wb;
		if(wf==0)
			break;
		sumB+=(i*hist[i]);
		mB=sumB/wb;
		mF=(sum-sumB)/wf;
		between = sqrt(wb) * sqrt(wf) * sqrt(pow((mB - mF),2));
        if ( between >= max ) 
        {
            threshold1 = i;
            if ( between > max ) 
            {
                threshold2 = i;
            }
            max = between;            
        }
    }
    return ( threshold1 + threshold2 ) / 2.0;
}
void initGrayImage(GrayImage *img,int row,int col,int maxval)
{
	int i;
	img->f=(int **)malloc(sizeof(int *)*row);
	if(img->f==NULL)
	{
		printf("Mem allocation failed\n");
		exit(1);
	}
	for(i=0;i<row;i++)
	{
		img->f[i]=(int *)malloc(sizeof(int)*col);
		if(img->f[i]==NULL)
		{	
			printf("Mem allocation failed\n");
			//exit(1);
		}
	}
	img->row=row;
	img->col=col;
	img->maxval=maxval;
}
GrayImage *zeros(int m,int n)
{
	int i,j;
	GrayImage *img=(GrayImage *)malloc(sizeof(GrayImage));
	img->row=m;
	img->col=n;
	img->maxval=255;
	img->f=(int **)malloc(sizeof(int *)*m);
	for(i=0;i<m;i++)
	img->f[i]=(int *)malloc(sizeof(int)*n);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		img->f[i][j]=0;
	}
	return img;
}	
GrayImage im2bw(GrayImage img,int level)
{
	int i,j;
	GrayImage *I=zeros(img.row,img.col);
	for(i=0;i<img.row;i++)
	{
		for(j=0;j<img.col;j++)
		{
			if(img.f[i][j]>level)
				I->f[i][j]=255;
		}
	}
	return *I;
}
GrayImage imcomplement(GrayImage *oImage)
{
	int i,j;
	GrayImage cImage;
	initGrayImage(&cImage,oImage->row,oImage->col,oImage->maxval);
	for(i=0;i<oImage->row;i++)
	{
		for(j=0;j<oImage->col;j++)
		{
			cImage.f[i][j]=oImage->maxval-oImage->f[i][j];
		}
	}
	return cImage;
}
void getMarker(GrayImage *img,GrayImage *result)
{
	int i,j;
	result->row=img->row;
	result->col=img->col;
	result->maxval=img->maxval;
	result->f=(int **)malloc(sizeof(int *)*img->row);
	for(i=0;i<img->row;i++)	
	result->f[i]=(int *)malloc(sizeof(int)*img->col);
	for(i=0;i<img->row;i++)
	{
		result->f[i][0]=img->maxval-img->f[i][0];
		result->f[i][img->col-1]=img->maxval-img->f[i][img->col-1];
	}
	for(i=1;i<img->col-1;i++)
	{
		result->f[0][i]=img->maxval-img->f[0][i];
		result->f[img->row-1][i]=img->maxval-img->f[img->row-1][i];
	}
	for(i=1;i<img->row-1;i++)
	{
		for(j=1;j<img->col-1;j++)
		{
			result->f[i][j]=0;
		}
	}
}

void normalize(GrayImage *img,GrayImage *enImage)
{
	int i,j,min,max=0;
	min=img->f[0][0];
	initGrayImage(enImage,img->row,img->col,img->maxval);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(img->f[i][j]<min)
			min=img->f[i][j];
			if(img->f[i][j]>max)
			max=img->f[i][j];
		}
	}
	for(i=0;i<enImage->row;i++)
	{
		for(j=0;j<enImage->col;j++)
		{
			enImage->f[i][j]=(int)((((float)(img->f[i][j]-min))/(max-min))*img->maxval);
			if(enImage->f[i][j]>img->maxval)
			{
				printf("Normalization error.. Value given..%d..min value %d...max value..%d\n",img->f[i][j],min,max);
				exit(1);
			}
		}
	}
}
GrayImage splitImage(GrayImage *img,int x1,int y1,int x2,int y2)
{
	GrayImage result;
	initGrayImage(&result,x2-x1,y2-y1,img->maxval);
	int i,j;
	for(i=x1;i<x2;i++)
	{
		for(j=y1;j<y2;j++)
		{
			result.f[i-x1][j-y1]=img->f[i][j];
		}
	}
	return result;
}
void freeImage(GrayImage *img)
{
	int i;
	for(i=0;i<img->row;i++)
	free(img->f[i]);
	free(img->f);
}
void imhist(GrayImage *img)
{
	int hist[256];
	int i,j;
	FILE *fp=fopen("hist.dat","w");
	for(i=0;i<256;i++)
	hist[i]=0;
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			hist[img->f[i][j]]++;
		}
	}
	for(i=0;i<256;i++)
	fprintf(fp,"%d %d\n",i,hist[i]);
	fclose(fp);
}
GrayImage imlabel(GrayImage *img,int **L,int *noOfLables)
{
	GrayImage labels;
	initGrayImage(&labels,img->row,img->col,img->maxval);
	struct eqclass *linked=NULL,*ptr;
	int N[2][2]={{-1,0},{0,-1}};
	int NB[2][2];
	int neighbours=0,nextlabel=1,i,j,k,ix,iy,count;
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(img->f[i][j]==0)
			labels.f[i][j]=0;
		}
	}
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{	
			if(img->f[i][j]!=0)
			{
				neighbours=0;
				for(k=0;k<2;k++)
				{
					ix=i+N[k][0];
					iy=j+N[k][1];
					if(ix>=0 && ix<img->row && iy>=0 && iy<img->col)
					{
						if(img->f[i][j]==img->f[ix][iy])
						{
							NB[neighbours][0]=ix;
							NB[neighbours][1]=iy;
							neighbours++;
						}
					}
				}
				if(neighbours==0)
				{
					insertEq(&linked,nextlabel);
					labels.f[i][j]=nextlabel;
					nextlabel++;
				}
				else if(neighbours==1)
				{
					labels.f[i][j]=labels.f[NB[0][0]][NB[0][1]];
				}
				else
				{
					if(labels.f[NB[0][0]][NB[0][1]]>labels.f[NB[1][0]][NB[1][1]])
					{
						mergeEq(&linked,labels.f[NB[1][0]][NB[1][1]],labels.f[NB[0][0]][NB[0][1]]);
						labels.f[i][j]=labels.f[NB[1][0]][NB[1][1]];
					}
					else
					{
						mergeEq(&linked,labels.f[NB[0][0]][NB[0][1]],labels.f[NB[1][0]][NB[1][1]]);
						labels.f[i][j]=labels.f[NB[0][0]][NB[0][1]];
					}
				}
			}
		}
	}
	//printEq(linked);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(img->f[i][j]!=0)
			{
				labels.f[i][j]=searchEq(linked,labels.f[i][j]);
			}
		}
	}
	(*noOfLables)=countClasses(linked);
	(*L)=(int *)malloc(sizeof(int)*(*noOfLables));
	ptr=linked;
	count=0;
	while(ptr!=NULL)
	{
		(*L)[count++]=ptr->min;
		ptr=ptr->next;
	}
	return labels;
}
void bwareaopen(GrayImage *img,GrayImage *result,int npixels)
{
	GrayImage temp;
	int *labels,nlabels;
	int i,j,k,count=0;
	temp=imlabel(img,&labels,&nlabels);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		result->f[i][j]=0;
	}
	for(k=0;k<nlabels;k++)
	{
		count=0;
		for(i=0;i<img->row;i++)
		{
			for(j=0;j<img->col;j++)
			{
				if(temp.f[i][j]==labels[k])
				count++;
			}
		}
		if(count>npixels)
		{
			for(i=0;i<img->row;i++)
			{
				for(j=0;j<img->col;j++)
				{
					if(temp.f[i][j]==labels[k])
					result->f[i][j]=img->maxval;
				}
			}
		}
	}
}
void superimpose(GrayImage *img,GrayImage *bin,int ns,GrayImage *output)
{
	int i,j,flag,k;
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			flag=0;
			for(k=0;k<ns && flag==0;k++)
			{
				if(bin[k].f[i][j]!=0)
				{
					output->f[i][j]=img->maxval;
					flag=1;
				}
			}
			if(flag==0)
			output->f[i][j]=img->f[i][j];
		}
	}
}
			
					

