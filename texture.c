#include <stdio.h>
#include "texture.h"
#include <math.h>
#include <stdlib.h>

#define mod(x) x>=0?x:-x

//*******************Textural approximation using statistical approach******************//
double texture_stat(int *data,int sizeData,double *pz,int sizeZ,int order)
{
	int i;
	double muz=0.0,mean=0.0;
	for(i=0;i<sizeData;i++)
	{
		if(data[i]<sizeZ)
		{
			mean=mean+data[i]*pz[data[i]];
		}
		else
		{
			printf("texture stat\n");
			exit(1);
		}
	}
	//printf("%lf\n",mean);
	for(i=0;i<sizeData;i++)
	{
		muz=muz+pow((data[i]-mean),order)*pz[data[i]];
	}
	return muz;
}
	
