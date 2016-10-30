#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"image.h"
#include "colorimage.h"
#include"show_image.h"
#include"textureOp.h"
#include"Eqclass.h"


/**************************************Function to compute probabilty matrix******************************/
void computeProb(GrayImage* img,float **promatrix,int d,int angle)
{
	int i,j;
	int x,y;
	int **matrix;
	int count=0;
	matrix=(int **)malloc(sizeof(int *)*LEVEL);
	for(i=0;i<256;i++)
	{
		matrix[i]=(int *)malloc(sizeof(int )*LEVEL);
	}
	if(angle==0)
	{
		x=0;
		y=1;
	}
	if(angle==45)
	{
		x=-1;
		y=1;
	}
	if(angle==90)
	{
		x=-1;
		y=0;
	}
	if(angle==135)
	{
		x=0;
		y=-1;
	}
	if(angle==180)
	{
		x=0;
		y=-1;
	}
	if(angle==225)
	{
		x=1;
		y=-1;
	}
	if(angle==270)
	{
		x=1;
		y=0;
	}
	if(angle==315)
	{
		x=1;
		y=1;
	}
	for(i=0;i<LEVEL;i++)
	{
		for(j=0;j<LEVEL;j++)
		{
			matrix[i][j]=0;
		}
	}
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if((i+(d*x)) < img->row && (i+(d*x))>=0 && (j+(d*y)) < img->col && (j+(d*y))>=0)
			matrix[img->f[i][j]][img->f[i+(d*x)][j+(d*y)]]++;
			else
			matrix[img->f[i][j]][img->f[i][j]]++;
		}
	}
	for(i=0;i<LEVEL;i++)
	{
		for(j=0;j<LEVEL;j++)
		{
			if(matrix[i][j]!=0)
				count=count+matrix[i][j];
		}
	}
	if(count==0)
	{
		printf("Not possible to find probability %d %d\n",img->row,img->col);
		
		exit(1);
	}
	for(i=0;i<LEVEL;i++)
	{
		for(j=0;j<LEVEL;j++)
		{
			promatrix[i][j]=(matrix[i][j]/(float)count);	
		}
	}
}
/**************************************Function to compute constrast**************************************/
float constrast(float **promatrix)
{
	float sum=0;
	int i,j;
	for(i=0;i<LEVEL;i++)
	{
		for(j=0;j<LEVEL;j++)
		{
			sum=sum+((i-j)*(i-j)*promatrix[i][j]);
		}
	}
	return sum;
}
/**************************************Function to compute uniformity*************************************/
float uniformity(float **promatrix)
{
	float sum=0;
	int i,j;
	for(i=0;i<LEVEL;i++)
	{
		for(j=0;j<LEVEL;j++)
		{
			sum=sum+(promatrix[i][j])*(promatrix[i][j]);
		}
	}
	return sum;
}
/**************************************Function to compute homogeneity*************************************/
float homogeneity(float **promatrix)
{
	float sum=0.0;
	int i,j,x,y;
	for(i=0;i<LEVEL;i++)
	{
		for(j=0;j<LEVEL;j++)
		{
			x=i-j;
			y=mod(x);
			sum=sum+(promatrix[i][j]/(1+y));
		}
	}
	return sum;
}
/*************************************Function to compute entropy******************************************/
float entropy(float **promatrix)
{
	float sum=0,val;
	int i,j;
	for(i=0;i<LEVEL;i++)
	{
		for(j=0;j<LEVEL;j++)
		{
			if(promatrix[i][j]==0)
				val=0;
			else
				val=(logf(promatrix[i][j]));
			sum=sum+promatrix[i][j]*(val/(logf(2)));
		}
	}
	return (-sum);
}
/************************************Function to compute correlation****************************************/
float correlation(float **promatrix)
{
	float mr=0,mc=0,stdr,stdc,variancer=0,variancec=0,product;
	float sum1[LEVEL],sum2[LEVEL],result=0;
	int i,j;
	for(i=0;i<LEVEL;i++)
	{
		sum1[i]=0;
		sum2[i]=0;
	}
	for(i=0;i<LEVEL;i++)
	{
		for(j=0;j<LEVEL;j++)
		{
			sum1[i]=sum1[i]+promatrix[i][j];
			sum2[j]=sum2[j]+promatrix[j][i];
		}
	}
	for(i=0;i<LEVEL;i++)
	{
		mr=mr+i*sum1[i];
		mc=mc+i*sum2[i];
	}
	for(i=0;i<LEVEL;i++)
	{
		variancer=variancer+(i-mr)*(i-mr)*sum1[i];
		variancec=variancec+(i-mc)*(i-mc)*sum2[i];
	}
	stdr=sqrt(variancer);
	stdc=sqrt(variancec);
	product=stdc*stdr;
	if(product==0)
	{
		printf("Overflow in correlation...%f %f\n",stdc,stdr);
		exit(1);
	}
	for(i=0;i<LEVEL;i++)
	{
		for(j=0;j<LEVEL;j++)
		{
			result=result+(((i-mr)*(j-mc)*promatrix[i][j])/product);
		}
	}
	return result;
}
/****************************************Inverse Difference moment******************************************/
float diffMoment(float **promatrix)
{
	float result=0;
	int i,j;
	for(i=0;i<LEVEL;i++)
	{
		for(j=0;j<LEVEL;j++)
		{
			result=result+(promatrix[i][j]/(1+(i-j)*(i-j)));
		}
	}
	return result;
}
/****************************************Calculate Probability Sum*****************************************/
void calProSum(float **promatrix,float *sum)
{
	int i,j,k;
	
	for(i=0;i<LEVEL;i++)
	{
		for(j=0;j<LEVEL;j++)
		{
			k=i+j;
			sum[k]=sum[k]+promatrix[i][j];
		}
	}
}
float sumAvg(float **promatrix)
{
	int i,j,k;
	float result=0.0;
	float *sum=(float *)malloc(sizeof(float)*2*LEVEL);
	calProSum(promatrix,sum);
	for(i=0;i<=(2*LEVEL)-2;i++)
	{
		result=result+i*sum[i];
	}
	return result;
}
float sumEntropy(float **promatrix)
{
	int i;
	float result=0,val;
	float *sum=(float *)malloc(sizeof(float)*2*LEVEL);
	calProSum(promatrix,sum);
	for(i=0;i<=(2*LEVEL)-2;i++)
	{
		if(sum[i]==0)
				val=0;
		else
			val=(logf(sum[i]));
		result=result+sum[i]*(val/(logf(2)));		
	}
	return (-result);	
}
float sumVariance(float **promatrix)
{
	int i;
	float result=0,sumEn;
	float *sum=(float *)malloc(sizeof(float)*2*LEVEL);
	calProSum(promatrix,sum);
	sumEn=sumEntropy(promatrix);
	for(i=0;i<=(2*LEVEL)-2;i++)
	{
		result=result+(i-sumEn)*(i-sumEn)*sum[i];
	}
	return result;
}
void calProDiff(float **promatrix,float *diff)
{
	int i,j,k;
	for(i=0;i<LEVEL;i++)
	{
		for(j=0;j<LEVEL;j++)
		{
			k=mod(i-j);
			diff[k]=diff[k]+promatrix[i][j];
		}
	}
}
float diffEntropy(float **promatrix)
{
	float *diff=(float *)malloc(sizeof(float)*LEVEL);
	calProDiff(promatrix,diff);
	int i;
	float result=0,val;
	for(i=0;i<=(LEVEL-1);i++)
	{
		if(diff[i]==0)
				val=0;
		else
			val=(logf(diff[i]));
		result=result+diff[i]*(val/(logf(2)));
	}
	return (-result);
}
float diffVariance(float **promatrix)
{
	float *diff=(float *)malloc(sizeof(float)*LEVEL);
	calProDiff(promatrix,diff);
	int i;
	float result=0;
	for(i=0;i<=(LEVEL+1);i++)
	{
		result=result+i*i*diff[i];
	}	
	return result;
}

/*main(int argc,char **argv)
{
	int i,j;
	int **co_matrix;
	float **promatrix;
	GrayImage original;
	readPGM(argv[1],&original);
	FILE *fp;
	fp=fopen("temp.txt","w");

	float con,uni,homo,en,cor;
	float count=0;
	co_matrix=(int **)malloc(sizeof(int *)*LEVEL);
	promatrix=(float **)malloc(sizeof(float *)*LEVEL);
	for(i=0;i<LEVEL;i++)
	{
		co_matrix[i]=(int *)malloc(sizeof(int)*LEVEL);
		promatrix[i]=(float *)malloc(sizeof(float)*LEVEL);
	}
	comatrix(&original,co_matrix,&count);
	printf("%f\n",count);
	computeProb(co_matrix,promatrix,count);
	for(i=0;i<LEVEL;i++)
	{
		for(j=0;j<LEVEL;j++)
		{
			fprintf(fp,"%f ",promatrix[i][j]);
		}
		fprintf(fp,"\n");
	}
	
	con=constrast(promatrix);
	uni=uniformity(promatrix);
	homo=homogeneity(promatrix);
	en=entropy(promatrix);
	cor=correlation(promatrix);
	printf("Constrast:%lf\nuniformity=%lf\nhomogeneity=%lf\nentropy=%lf\ncorrelation=%lf\n",con,uni,homo,en,cor);
	
}*/
