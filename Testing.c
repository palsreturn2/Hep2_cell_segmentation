#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "image.h"
#include "show_image.h"
#include "morphology.h"
#include "filtersAndGrads.h"
#include "texture.h"
#include "seedgen.h"
#include "ungrowcut.h"

//************************************The reconstruction function****************************//
void testReconstruct(GrayImage *mask,GrayImage *marker,GrayImage *final)
{
	GrayImage temp,temp2;
	int i,j,flag=1;
	initGrayImage(&temp,mask->row,mask->col,mask->maxval);
	int **SE=strel("111111111",3);
	for(i=0;i<marker->row;i++)
	{
		for(j=0;j<marker->col;j++)
		{
			temp.f[i][j]=marker->f[i][j];		//Forming image to be dilated
		}
	}
	while(flag)
	{
		temp2=imdilate(&temp,SE,3);		//dilation operation
		for(i=0;i<marker->row;i++)
		{
			for(j=0;j<marker->col;j++)
			{
				if(temp2.f[i][j]!=0 && mask->f[i][j]==0)		//Checking whether the dilated image exceeded the mask boundary
				{
					flag=0;
				}
				temp.f[i][j]=temp2.f[i][j];
			}
		}
		freeImage(&temp2);
	}
	for(i=0;i<marker->row;i++)
	{
		for(j=0;j<marker->col;j++)
		{
			final->f[i][j]=temp.f[i][j];
		}
	}
	printf("Reconstruction done..\n");
}
void textureStatTest(GrayImage *img,GrayImage *final,int n)
{
	int i,j,window=3,count=0,i1,i2,k,m,c=0;
	GrayImage result;
	initGrayImage(&result,img->row,img->col,img->maxval+1);
	double *pz=(double *)malloc(sizeof(double)*(img->maxval+1));
	int *hist=(int *)malloc(sizeof(int)*(img->maxval+1));
	for(i=0;i<img->maxval+1;i++)
	hist[i]=0;
	int *temp;
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			hist[img->f[i][j]]++;
		}
	}
	for(i=0;i<img->maxval+1;i++)
	{
		pz[i]=1.0*hist[i]/(img->row*img->col);
		//printf("%lf\n",pz[i]);
	}
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			count=0;
			for(k=-window/2;k<=window/2;k++)
			{
				for(m=-window/2;m<=window/2;m++)
				{
					i1=i+k;
					i2=j+m;
					if(i1>=0 && i1<img->row && i2>=0 && i2<img->col)
					{
						 count++;
					}
				}
			}
			temp=(int *)malloc(sizeof(int)*count);
			c=0;
			for(k=-window/2;k<=window/2;k++)
			{
				for(m=-window/2;m<=window/2;m++)
				{
					i1=i+k;
					i2=j+m;
					if(i1>=0 && i1<img->row && i2>=0 && i2<img->col)
					{
						 temp[c++]=img->f[i1][i2];
					}
				}
			}
		result.f[i][j]=(int)texture_stat(temp,count,pz,img->maxval+1,n);
		free(temp);
		}
	}
	free(hist);
	free(pz);
	normalize(&result,final);
	freeImage(&result);
	imshow(*final);
}
//***********************************Function for threshold testing************************
void thresholdtest(GrayImage *img)
{
	GrayImage temp,eI,BI,LI,CI,FI,temp2;
	char *pat="001111100011111110111111111111111111111111111111111111111111111011111110001111100";
	int **SE=strel(pat,9);
	int nL,*labels,i,j,k,count=0;
	temp=histeq(img);	
	BI=im2bw(temp,graythres(&temp));
	eI=imerode(&BI,SE,9);
	eI=imerode(&eI,SE,9);
	eI=imdilate(&eI,SE,9);
	eI=imdilate(&eI,SE,9);
	LI=imlabel(&eI,&labels,&nL);		//Labeling the component image
	label2rgb(&LI);
	freeImage(&eI);
	freeImage(&temp);
	initGrayImage(&FI,img->row,img->col,img->maxval);
	initGrayImage(&eI,img->row,img->col,img->maxval);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		FI.f[i][j]=0;
	}
	for(k=0;k<nL;k++)
	{
		count=0;
		initGrayImage(&CI,img->row,img->col,img->maxval);
		for(i=0;i<img->row;i++)				//Picking up the components and doing and operation with the original Image
		{
			for(j=0;j<img->col;j++)
			{
				if(LI.f[i][j]==labels[k])
				{
					count++;
					CI.f[i][j]=img->f[i][j];
					eI.f[i][j]=img->maxval;
				}
				else
				{
					CI.f[i][j]=0;
					eI.f[i][j]=0;
				}
			}
		}
		initGrayImage(&temp,1,count,img->maxval);	//image that contains gray values of pixels where the component lies
		count=0;
		for(i=0;i<img->row;i++)
		{
			for(j=0;j<img->col;j++)
			{
				if(LI.f[i][j]==labels[k])
				temp.f[0][count++]=img->f[i][j];//Putting intensity values of the component pixels 
			}
		}
		temp2=im2bw(CI,graythres(&temp));
		freeImage(&temp);
		initGrayImage(&temp,img->row,img->col,img->maxval);
		testReconstruct(&eI,&temp2,&temp);	//Reconstructing the image
		for(i=0;i<img->row;i++)
		{
			for(j=0;j<img->col;j++)
			{
				if(temp.f[i][j]!=0)
				FI.f[i][j]=img->maxval;
			}
		}
		freeImage(&temp2);
	}
	initGrayImage(&temp,img->row,img->col,img->maxval);
	bwareaopen(&FI,&temp,20);
	imshow(temp);
	freeImage(&temp);
	exit(1);
}
	
void test(GrayImage *img)
{
	int T1,T2,T3,i;
	GrayImage i1,i2,i3,I;
	char *pat="001111100011111110111111111111111111111111111111111111111111111011111110001111100";
	int **SE=strel(pat,9);
	I=*(img);
	/*T1=graythres(&I);	
	T2=T1/2;
	T3=((I.maxval)-T2)/2;
	i1=im2bw(I,T1);
	i2=im2bw(I,T2);
	i3=im2bw(I,T3);
	
	
	savePGM(i1,"test1.pgm");
	savePGM(i2,"test2.pgm");
	savePGM(i3,"test3.pgm");*/
	for(i=1;i<img->maxval;i+=2)
	{
		i1=im2bw(I,i);
		printf("Threshold=%d\n",i);
		imshow(i1);
		freeImage(&i1);
		getchar();
	}
}
void testwatershed(GrayImage *img)
{
	int **SE=strel("1111111111111111111111111",5);
	GrayImage temp,temp2,Iobr,Io,Ioc,Iobrd,Iobrcbr;
	initGrayImage(&Io,img->row,img->col,img->maxval);
	initGrayImage(&Ioc,img->row,img->col,img->maxval);
	imopen(img,SE,5,&Io);
	imclose(&Io,SE,5,&Ioc);
	temp=imerode(img,SE,5);
	initGrayImage(&Iobr,img->row,img->col,img->maxval);
	gdilate(&temp,img,&Iobr);
	Iobrd=imdilate(&Iobr,SE,5);
	freeImage(&temp);
	temp=imcomplement(&Iobrd);
	temp2=imcomplement(&Iobr);
	initGrayImage(&Iobrcbr,img->row,img->col,img->maxval);
	gdilate(&temp,&temp2,&Iobrcbr);
	freeImage(&temp);
	freeImage(&temp2);
	temp=imcomplement(&Iobrcbr);
	imshow(temp);
	exit(1);
}
main(int argc,char **argv)
{
	GrayImage img,temp,originalImage,seedImage,finalImage;
	Cell cellOld,cellNew;
	int i,j,*labels,nL,T;
	readPGM(argv[1],&img);
	T=graythres(&img);
	printf("Threshold=%d\n",T);
	imshow(im2bw(img,T));
	exit(1);
	
	testwatershed(&img);
	thresholdtest(&img);
	initGrayImage(&temp,img.row,img.col,img.maxval);
	textureStatTest(&img,&temp,2);	
	exit(1);
	imshow(temp);
	initGrayImage(&originalImage,img.row,img.col,img.maxval);
	for(i=0;i<img.row;i++)
	{
		for(j=0;j<img.col;j++)
		originalImage.f[i][j]=temp.f[i][j];
	}
	freeImage(&temp);
	originalImage.row=500;
	originalImage.col=500;
	seedImage=genRandomImg(500,originalImage.row,originalImage.col);
	initSeedPixel(&seedImage,&originalImage,&cellOld);
	growCut(&cellOld,&cellNew,&originalImage);
	getSegment(&cellOld,&finalImage,&originalImage);
}
