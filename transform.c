#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Eqclass.h"
#include "image.h"
#include "show_image.h"


GrayImage hough(GrayImage *img)
{
	int i,j,pho,theta,p;
	GrayImage A;
	pho=((int)sqrt(img->row*img->row+img->col*img->col))+1;
	initGrayImage(&A,2*pho+1,181,img->maxval);
	for(i=0;i<A.row;i++)
	{
		for(j=0;j<A.col;j++)
		A.f[i][j]=0;
	}
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(img->f[i][j]!=0)
			{
				for(theta=-90;theta<=90;theta++)
				{
					p=(int)(i*cos(theta) + j*sin(theta));
					A.f[pho+p][theta+90]++;
				}
			}
		}
	}
	return A;
}
main(int argc,char **argv)
{
	GrayImage img,temp;
	readPGM(argv[1],&img);
	temp=hough(&img);
	imshow(temp);
}
