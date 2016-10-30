#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"image.h"
#include"colorimage.h"
#include"show_image.h"
#include"Eqclass.h"
#include"LBP.h"

int findLBP(GrayImage *img,int ix,int jy)
{
	int val=img->f[ix][jy];
	int code=0;
	int i,j;
	int K[8][2]={{-1,-1},{-1,0},{-1,1},{0,-1},{1,1},{1,0},{1,-1},{-1,0}};
	int mx,my;
	for(i=0;i<8;i++)
	{
		mx=ix+K[i][0];
		my=jy+K[i][1];
		mx=mx%img->row;
		my=my%img->col;
		if(mx<0)
			mx=img->row+mx;
		if(my<0)
			my=img->col+my;
		if(val>=img->f[mx][my])
			code=code+pow(2,i);
	}
	return code;	
}

void resultLBP(GrayImage *img,GrayImage *result)
{
	int i,j;
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			result->f[i][j]=findLBP(img,i,j);
		}
	}
}
