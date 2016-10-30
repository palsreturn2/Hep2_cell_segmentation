#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "image.h"
#include "filtersAndGrads.h"

#define MOD(x) x>=0?x:-x
#define RADIAN 1.57
GrayImage imfilter(GrayImage *originalImage,double filter[3][3],int filterX,int filterY)
{
	int i,j,m,n;
	double s=0;
	GrayImage filImage,img;
	initGrayImage(&filImage,originalImage->row,originalImage->col,originalImage->maxval);
	for(i=0;i<originalImage->row;i++)
	{
		for(j=0;j<originalImage->col;j++)
		{
			s=0;
			for(m=-filterX/2;m<=filterX/2;m++)
			{
				for(n=-filterY/2;n<=filterY/2;n++)
				{
					if(m+i>=0 && m+i<originalImage->row && n+j>=0 && n+j<originalImage->col)
					s=s+((originalImage->f[m+i][n+j])*(filter[m+filterX/2][n+filterY/2]));
				}
			}
			filImage.f[i][j]=(int)s;
			//filImage.f[i][j]=((int)(s>255?255:s));
		}
	}
	filImage.maxval=originalImage->maxval;
	return filImage;
}
GrayImage imgradient(GrayImage *originalImage)
{
	int i,j;
	double filter[3][3]={{1,1,1},{1,-8,1},{1,1,1}};
	GrayImage I,final;
	I=imfilter(originalImage,filter,3,3);
	for(i=0;i<originalImage->row;i++)
	{
		for(j=0;j<originalImage->col;j++)
		{
			I.f[i][j]=originalImage->f[i][j]-I.f[i][j];
		}
	}
	normalize(&I,&final);
	return final;
}
void gradientSobel(GrayImage *originalImage,GrayImage *grad,GrayImage *angle)
{
	double filterx[3][3]={{-1,0,1},{-2,0,2},{-1,0,1}};
	double filtery[3][3]={{1,2,1},{0,0,0},{-1,-2,-1}};
	int i,j;	
	GrayImage Gx=imfilter(originalImage,filterx,3,3);
	GrayImage Gy=imfilter(originalImage,filtery,3,3);
	GrayImage g;
	//imshow(Gx);
	initGrayImage(&g,originalImage->row,originalImage->col,originalImage->maxval);
	initGrayImage(angle,originalImage->row,originalImage->col,originalImage->maxval);
	for(i=0;i<Gx.row;i++)
	{
		for(j=0;j<Gx.col;j++)
		{
			g.f[i][j]=sqrt((Gx.f[i][j]*Gx.f[i][j])+(Gy.f[i][j]*Gy.f[i][j]));
			if(Gx.f[i][j]!=0)
			angle->f[i][j]=(int)(atan(Gy.f[i][j]/Gx.f[i][j])*180/M_PI);
			else if(Gy.f[i][j]>=0)
			angle->f[i][j]=90;
			else
			angle->f[i][j]=-90;
		}
	}
	normalize(&g,grad);
}
void makeGaussFilter(double gKernel[3][3],int size)
{
	int x,y,i,j;
	//double **gKernel=(double **)malloc(sizeof(double *)*size);
	//for(i=0;i<size;i++)
	//gKernel[i]=(double *)malloc(sizeof(double)*size);
	double sigma = 1;
   double r, s = 2.0 * sigma * sigma;
 
   // sum is for normalization
   double sum = 0.0;
 
   for (x = -(size/2); x <= (size/2); x++)
   {
       for(y = -(size/2); y <= (size)/2; y++)
       {
           r = sqrt(x*x + y*y);
           gKernel[x + (size/2)][y + (size/2)] = (exp(-(r*r)/s))/(M_PI * s);
           sum += gKernel[x + (size/2)][y + (size/2)];
       }
   }
   // normalize the Kernel
   for(i = 0; i < size; ++i)
       for(j = 0; j < size; ++j)
           gKernel[i][j]=gKernel[i][j]/sum;
   
}
GrayImage filterGauss(GrayImage *img)
{
	double flt[3][3];
	int i;
	GrayImage temp;
	makeGaussFilter(flt,3);
	temp=imfilter(img,flt,3,3);
	return temp;
}
void sort(int *a,int n)
{
	int count[256];
	int i,j,c=0;
	for(i=0;i<256;i++)
	count[i]=0;
	for(i=0;i<n;i++)
	count[a[i]]++;
	for(i=0;i<256;i++)
	{
		while(count[i]!=0)
		{
			a[c]=i;
			count[i]--;
			c++;
		}
	}
	return;
}
void alphaTMFilter(GrayImage *img,float alpha,GrayImage *result)
{
	int seg=5;
	int i,j,ix,iy,m,n,an,s=0,c;
	int a[seg*seg];
	ix=0;
	iy=0;
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			c=0;
			for(m=-seg/2;m<=seg/2;m++)
			{
				for(n=-seg/2;n<=seg/2;n++)
				{
					ix=i+m;
					iy=j+n;
					if(ix>=0 && ix<img->row && iy>=0 && iy<img->col)
					{
						a[c]=img->f[ix][iy];
						c++;
					}
				}
			}
			sort(a,c);
			an=alpha*c;
			s=0;
			for(m=an;m<c-an;m++)
			s=s+a[m];
			if((c-(2*an))!=0)
			s=s/(c-(2*an));
			result->f[i][j]=(int)s;
		}
	}
}
void medfilt(GrayImage *img,GrayImage *result)
{
	alphaTMFilter(img,0.5,result);
}
void meanfilt(GrayImage *img,GrayImage *result)
{
	alphaTMFilter(img,0.0,result);
}
void comboSpatialFilter(GrayImage *img)
{
	GrayImage b,c,d,e,fi,g,angle,final;
	int i,j,l=0;
	double filter[3][3]={{1,1,1},{1,1,1},{1,1,1}};
	b=imgradient(img);
	//imshow(b);
	initGrayImage(&c,img->row,img->col,img->maxval);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			c.f[i][j]=img->f[i][j]+b.f[i][j];
		}
	}
	gradientSobel(img,&d,&angle);
	
	e=imfilter(&d,filter,3,3);
	initGrayImage(&g,img->row,img->col,img->maxval);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			g.f[i][j]=img->f[i][j]+((e.f[i][j]*c.f[i][j])/9);
			g.f[i][j]=g.f[i][j]>=255?255:g.f[i][j];
		}
	}
	normalize(&g,&final);
	imshow(final);
}
GrayImage histeq(GrayImage *img)
{
	int i,j,K,n=0;
	GrayImage final;
	initGrayImage(&final,img->row,img->col,img->maxval);
	int *hist,*sk;
	hist=(int *)malloc(sizeof(int) * (img->maxval+1));
	sk=(int *)malloc(sizeof(int) * (img->maxval+1));
	for(i=0;i<=img->maxval;i++)
	hist[i]=0;
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			hist[img->f[i][j]]++;	
		}
	}
	K=img->row*img->col;
	for(i=0;i<=img->maxval;i++)
	{
		n=n+hist[i];
		sk[i]=(img->maxval)*n/K;
	}
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			final.f[i][j]=sk[img->f[i][j]];
		}
	}
	free(hist);
	free(sk);
	return final;
}
GrayImage cannyedge(GrayImage *img)
{
	double gflt[3][3];
	int min,ix1,ix2,iy1,iy2,s,i,j,k;
	int N[8][2]={{-1,-1},{-1,0},{0,-1},{1,1},{1,-1},{-1,1},{1,0},{0,1}};
	makeGaussFilter(gflt,3);
	GrayImage final,f,grad,angle,gNH,gNL,g;
	f=imfilter(img,gflt,3,3);
	gradientSobel(&f,&grad,&angle);
	initGrayImage(&final,img->row,img->col,img->maxval);
	for(i=0;i<angle.row;i++)
	{
		for(j=0;j<angle.col;j++)
		{
			if(angle.f[i][j]>=-22 && angle.f[i][j]<=22)
			{
				iy1=iy2=j;
				ix1=i-1>=0?i-1:0;
				ix2=i+1<angle.row?i+1:angle.row-1;
			}
			else if(angle.f[i][j]<=-23 && angle.f[i][j]>=-67)
			{
				ix1=i+1<angle.row?i+1:angle.row-1;
				iy1=j-1>=0?j-1:0;
				ix2=i-1>=0?i-1:0;
				iy2=j+1<angle.col?j+1:angle.col-1;
			}
			else if(angle.f[i][j]>=23 && angle.f[i][j]<=67)
			{
				ix1=i+1<angle.row?i+1:angle.row-1;
				iy1=j+1<angle.col?j+1:angle.col-1;
				ix2=i-1>=0?i-1:0;
				iy2=j-1>=0?j-1:0;
			}
			else
			{
				ix1=ix2=i;
				iy1=j-1>=0?j-1:0;
				iy2=j+1<angle.col?j+1:angle.col-1;	
			}
			if(grad.f[ix1][iy1]>grad.f[i][j] || grad.f[ix2][iy2]>grad.f[i][j])
			final.f[i][j]=0;
			else
			final.f[i][j]=grad.f[i][j];
		}
	}
	gNH=im2bw(final,graythres(&final));
	gNL=im2bw(final,graythres(&final)/2);
	for(i=0;i<gNL.row;i++)
	{
		for(j=0;j<gNL.col;j++)
		{
			gNL.f[i][j]=gNL.f[i][j]-gNH.f[i][j];
		}
	}
	initGrayImage(&g,gNH.row,gNH.col,gNH.maxval);
	for(i=0;i<gNH.row;i++)
	{
		for(j=0;j<gNH.col;j++)
		{
			if(gNH.f[i][j]!=0)
			g.f[i][j]=gNH.f[i][j];
			for(k=0;k<8;k++)
			{
				ix1=i+N[k][0];
				iy1=j+N[k][1];
				if(ix1>=0 && ix1<g.row && iy1>=0 && iy1<g.col)
				{
					if(gNL.f[ix1][iy1]==1)
					g.f[ix1][iy1]=1;
				}
			}
		}
	}
	//imshow(g);
	return g;
}
void medfilt2(GrayImage *img,int size,GrayImage *result)
{
	int i,j,m,n,ix,iy,count;
	int *temp=(int *)malloc(sizeof(int)*size*size);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			count=0;
			for(m=-size/2;m<=size/2;m++)
			{
				for(n=-size/2;n<=size/2;n++)
				{
					ix=i+m;
					iy=j+n;
					if(ix>=0 && ix<img->row && iy>=0 && iy<img->col)
					{
						temp[count++]=img->f[i][j];
					}
				}
			}
			sort(temp,count);
			result->f[i][j]=temp[count/2];
		}
	}
	free(temp);
}
	
	
	
	
	
