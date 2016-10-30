#include <stdio.h>
#include <stdlib.h>
#include "image.h"
#include "colorimage.h"
#include "show_image.h"

void removecomp(GrayImage *img, GrayImage *gtruth,GrayImage *result)
{
	int i,j,max=0,*labels,nL,k,flag=0;
	GrayImage tmp;
	initGrayImage(&tmp,img->row,img->col,img->maxval);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		result->f[i][j]=0;
	}
	for(i=0;i<img->row;i++)
	{	
		for(j=0;j<img->col;j++)
		{
			if(img->f[i][j]>0)
			tmp.f[i][j]=img->maxval;
			else
			tmp.f[i][j]=0;
		}
	}
	GrayImage LI=imlabel(&tmp,&labels,&nL);
	
	for(k=0;k<nL;k++)
	{
		flag=0;
		for(i=0;i<img->row;i++)
		{
			for(j=0;j<img->col && flag==0;j++)
			{
				if(LI.f[i][j]==labels[k] && gtruth->f[i][j]!=0)
				{
					flag=1;	
				}
			}
		}
		if(flag==1)
		{
			for(i=0;i<img->row;i++)
			{
				for(j=0;j<img->col;j++)
				{
					if(LI.f[i][j]==labels[k])
					result->f[i][j]=img->maxval;
				}
			}
		}
	}
}
float sorencoeff(GrayImage *result,GrayImage *gtruth)
{
	int i,j,c=0,a=0,b=0;
	float dcoeff;
	for(i=0;i<result->row;i++)
	{
		for(j=0;j<result->col;j++)
		{
			if(result->f[i][j]!=0 && gtruth->f[i][j]!=0)
			c++;
			if(result->f[i][j]!=0)
			a++;
			if(gtruth->f[i][j]!=0)
			b++;
		}
	}
	dcoeff=2.0*c/(float)(a+b);
	return dcoeff;
}
			
		
