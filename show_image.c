#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"
#include "colorimage.h"
#include "show_image.h"


void readPGM(char *fname,GrayImage *img)
{
	FILE *ifp;
	int i,j,type,width,height,maxVal;
	int	**data;
	int ch_int,lo;
	char ch,hi;
	ifp=fopen(fname,"rb");//opening the file
	if(ifp==NULL)
	{
		printf("Error: Unable to open file %s\n\n", fname);
		exit(1);
	}
	printf("Reading the image file:%s\n",fname);
	
	/**checking the type of pgm file***/
	ch=fgetc(ifp);
	if(ch!='P')
	{
		printf("Not a pgm file...\n");
		exit(1);
	}
	ch=fgetc(ifp);
	type=ch-48;

	if(type==2)
	{
		printf("It is a P2 file...\n");
	}
	if(type==5)
	{
		printf("It is a P5 file...\n");
	}
	/*************************************/
	
	ch=fgetc(ifp);
	ch=fgetc(ifp);

	if(ch=='#')
	{
		//printf("Comment");
		while(fgetc(ifp)!='\n');//skiping the comment
		//exit(1);
	}
	fseek(ifp,-1,SEEK_CUR);
	fscanf(ifp,"%d",&width);
	
	ch=fgetc(ifp);
	
	fscanf(ifp,"%d",&height);
	
	ch=fgetc(ifp);
	
	data=(int **)malloc(height*sizeof(int *));
	for(i=0;i<height;i++)
	data[i]=(int *)malloc(width*sizeof(int));
	
	fscanf(ifp,"%d",&maxVal);
	ch=fgetc(ifp);
	if(maxVal>=256)
	{
		printf("Cannot read...program needs to be changed..\n");
		exit(1);
	}
	if(type==2)//for P2 image file
	{
		for(i=0;i<height;i++)
		{
			for(j=0;j<width;j++)
			{
				fscanf(ifp,"%d",&ch_int);
				if(ch_int<=maxVal)
				data[i][j]=ch_int;
				else
				{
					printf("Error in reading\n");
					exit(1);
				}
			}						
		}
	}
	else if(type==5)//for P5 image file
	{
      for (i=0;i<height;i++)
      {
           	for (j=0;j<width;j++) 
           	{
              	lo = fgetc(ifp);
              	data[i][j] = (lo<0)?255+lo:lo;	
           	}
		}
	}
	fclose(ifp);
	img->f=data;
	img->row=height;
	img->col=width;
	img->maxval=maxVal;
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(img->f[i][j]<0)
			{
				printf("Image wrongly read...negative intensity values\n");
				exit(1);
			}
		}
	}
}
void savePGM(GrayImage img,char *out_file)
{
	FILE* ofp;
	int i,j,temp;
	char lo,hi;
	ofp=fopen(out_file,"wb");//opening the file
	if(ofp==NULL)
	{
		printf("Error: Unable to open file...");
		exit(1);
	}
	//printf("Writing on the image file:%s\n\n",out_file);
/********Writing header*****************/
	fprintf(ofp,"P5\n");
	fprintf(ofp,"%d %d\n",img.col,img.row);
	fprintf(ofp,"%d\n",img.maxval);
/**************************************/
    if (img.maxval > 255)
    {
        for (i = 0; i < img.row; ++i) 
        {
        	for (j = 0; j < img.col; ++j) 
        	{
            	hi = HI(img.f[i][j]);
            	lo = LO(img.f[i][j]);
            	fputc(hi, ofp);
            	fputc(lo, ofp);
			}
 
		}
	}
	else 
	{
        for (i = 0; i < img.row; ++i)
        {
        	for (j = 0; j < img.col; ++j) 
        	{
                lo = LO(img.f[i][j]);
                fputc(lo, ofp);
            }
        }
    }
	fclose(ofp);
}
void imshow(GrayImage img )
{
	savePGM(img,"temp.pgm");
	system("eog temp.pgm &");
}
/*int main(int argc,char **argv)
{
	GrayImage img;
	int m,n,row,col,maxVal,format;
	readPGM(argv[1],&img);
	imshow(img);
	return 0;
}*/
void label2rgb(GrayImage *img)
{
	int i,j;
	FILE *fp;
	static unsigned char color[3];
	fp=fopen("temp.ppm","wb");
	fprintf(fp, "P6\n%d %d\n%d\n",img->col,img->row,img->maxval);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
				if(img->f[i][j]==0)
				{
					color[0] = 255; /* red */
		  	color[1] = 255;  /* green */
   		color[2] = 255;  /* blue */
   		fwrite(color, 1, 3, fp);
   	}
   	else
				{
					color[0] = img->f[i][j]; /* red */
		  	color[1] = img->f[i][j]+ img->f[i][j];  /* green */
   		color[2] = (img->f[i][j]*img->f[i][j]);  /* blue */
   		fwrite(color, 1, 3, fp);
   	}
		}
	}
	system("eog temp.ppm &");
}	
/**************************************Reading protable color image**************************/
void readPPM(char *str,ColorImage *cimg)
{
	FILE *fp;
	fp=fopen(str,"r");
	char ch;
	int width,height,maxval,i,j,lo,type;
	if(fp==NULL)
	{
		printf("Error:Cannot open file\n");
		exit(1);
	}
	
	ch=fgetc(fp);
	if(ch!='P')
	printf("It is not a PPM file\n");
	ch=fgetc(fp);
	type=ch-48;
	if(type==6)
	printf("It is a P6 file\n");
	
	ch=fgetc(fp);
	ch=fgetc(fp);

	if(ch=='#')
	{
		//printf("Comment");
		while(fgetc(fp)!='\n');//skiping the comment
		//exit(1);
	}
	fseek(fp,-1,SEEK_CUR);
	fscanf(fp,"%d",&width);
	
	ch=fgetc(fp);
	
	fscanf(fp,"%d",&height);
	
	ch=fgetc(fp);
	
	fscanf(fp,"%d",&maxval);
	
	initColorImage(cimg,height,width,maxval);
	for(i=0;i<height;i++)
   {
     	for (j=0;j<width;j++) 
     	{
     		lo=fgetc(fp);
        	cimg->f[0][i][j]=(lo<0)?255+lo:lo;
        	lo=fgetc(fp);
        	cimg->f[1][i][j]=(lo<0)?255+lo:lo;
        	lo=fgetc(fp);
        	cimg->f[2][i][j]=(lo<0)?255+lo:lo;
     	}
	}
	fclose(fp);
}
/****************************************Writing Portable Color image***********************/
void savePPM(ColorImage *cimg,char *str)
{
	FILE *fp=fopen(str,"w");
	fprintf(fp,"P6\n");
	fprintf(fp,"%d %d\n",cimg->col,cimg->row);
	fprintf(fp,"%d\n",cimg->maxval);
	char color[3];
	int i,j;
	for(i=0;i<cimg->row;i++)
	{
		for(j=0;j<cimg->col;j++)
		{
			color[0] = cimg->f[0][i][j]; /* red */
		  	color[1] = cimg->f[1][i][j];  /* green */
   		color[2] = cimg->f[2][i][j];  /* blue */
   		fwrite(color, sizeof(char), 3, fp);
   	}
   }
   fclose(fp);
}
/*************************************Show ppm image****************************************/
void cimshow(ColorImage cimg)
{
	savePPM(&cimg,"temp.ppm");
	system("eog temp.ppm &");
}
/**************************************Read pbm file****************************************/
void readPBM(char *str,GrayImage *img)
{
	FILE *fp;
	int lo,i,j,width,height;
	char ch;
	fp=fopen(str,"r");
	ch=fgetc(fp);
	if(ch!='P')
	{
		printf("It is not a pbm file\n");
		exit(1);
	}
	ch=fgetc(fp);
	if(ch=='4')
	{
		printf("It is a P4 file\n");
	}
	fgetc(fp);
	ch=fgetc(fp);

	if(ch=='#')
	{
		//printf("Comment");
		while(fgetc(fp)!='\n');//skiping the comment
		//exit(1);
	}
	fseek(fp,-1,SEEK_CUR);
	fscanf(fp,"%d",&width);
	fgetc(fp);
	fscanf(fp,"%d",&height);
	fgetc(fp);
	initGrayImage(img,height,width,255);
	for(i=0;i<height;i++)
   {
     	for (j=0;j<width;j+=8)
     	{
     		lo=fgetc(fp);
     		if(j>=width)
     		break;
     		lo=(lo<0)?255+lo:lo;
        	img->f[i][j]=(lo&128!=0)?255:0;
        	img->f[i][j+1]=(lo&64!=0)?255:0;
        	img->f[i][j+2]=(lo&32!=0)?255:0;
        	img->f[i][j+3]=(lo&16!=0)?255:0;
        	img->f[i][j+4]=(lo&8!=0)?255:0;
        	img->f[i][j+5]=(lo&4!=0)?255:0;
        	img->f[i][j+6]=(lo&2!=0)?255:0;
        	img->f[i][j+7]=(lo&1!=0)?255:0;
     	}
	}
	fclose(fp);
}
