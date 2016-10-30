#include <stdio.h>
#include <stdlib.h>
#include "image.h"
#include "colorimage.h"
#include "show_image.h"
#include "libtest.h"

void debugImage(GrayImage *img)
{
	int row,col,length,breadth,i,j;
	printf("View image portion as matrix\n\n");
	printf("Enter the image portion as matrix\n\n\n");
	printf("Enter the x co-ordinate(%d-%d)\n",0,img->row-1);
	scanf("%d",&row);
	printf("Enter the y co-ordinate(%d-%d)\n",0,img->col-1);
	scanf("%d",&col);
	printf("Enter the length of the rectangle\n");
	scanf("%d",&length);
	printf("Enter the breadth of the rectangle\n");
	scanf("%d",&breadth);
	if(row+breadth>img->row || col+length>img->col || length>100 || breadth>100)
	{
		printf("Not possible... Please choose a shorter window...\n");
		return;
	}
	
	for(i=0;i<breadth;i++)
	{
		for(j=0;j<length;j++)
		{
			printf("%d\t",img->f[row+i][col+j]);
		}
		printf("\n\n");
	}
	return;
}
void createImage(GrayImage *img)
{
	char *filename=(char *)malloc(sizeof(char) * MAX_FILENAME_LENGTH);
	int i,ch;
	printf("This is a portal for creating test images..\n");
	printf("Press enter to continue\n");
	getchar();
	while(1)
	{
		printf("Components\n1) Square(type 1)\n2) Rectangle(type 2)\n3) Show Image(type 3)\n4) Save(type 4)\n5) Exit(type 5)\nChoose an option from above..\n");
		scanf("%d",&ch);
		switch(ch)
		{
			case 1:insSquare(img);
				break;
				
			case 2:insRect(img);
				break;
				
			case 3:imshow(*img);
				break;
				
			case 4:
			printf("Please enter a filename\n");
			for(i=0;i<MAX_FILENAME_LENGTH && (filename[i]=getchar())!='\n';i++);
			filename[i]='\0';
			savePGM(*img,filename);
				break;
			
			case 5:
			
			printf("Exiting..\n");
			return;
			
				break;
			default:printf("Invalid option\n\n\n");
		}
	}
}
void insSquare(GrayImage *img)
{
	int row,col,size,i,j;
	printf("Enter the x co-ordinate(%d-%d)\n",0,img->row-1);
	scanf("%d",&row);
	printf("Enter the y co-ordinate(%d-%d)\n",0,img->col-1);
	scanf("%d",&col);
	printf("Enter the size of the square\n");
	scanf("%d",&size);
	
	if(row+size>img->row || col+size>img->col)
	{
		printf("Not possible\n");
		return;
	}
	
	for(i=0;i<size;i++)
	{
		for(j=0;j<size;j++)
		{
			img->f[row+i][col+j]=img->maxval;
		}
	}
}
void insRect(GrayImage *img)
{
	int row,col,length,breadth,i,j;
	printf("Enter the x co-ordinate(%d-%d)\n",0,img->row-1);
	scanf("%d",&row);
	printf("Enter the y co-ordinate(%d-%d)\n",0,img->col-1);
	scanf("%d",&col);
	printf("Enter the length of the rectangle\n");
	scanf("%d",&length);
	printf("Enter the breadth of the rectangle\n");
	scanf("%d",&breadth);
	
	if(row+breadth>img->row || col+length>img->col)
	{
		printf("Not possible\n");
		return;
	}
	
	for(i=0;i<breadth;i++)
	{
		for(j=0;j<length;j++)
		{
			img->f[row+i][col+j]=img->maxval;
		}
	}
}
/*main()
{
	GrayImage img;
	initGrayImage(&img,4,4,255);
	createImage(&img);
	debugImage(&img);
}*/		
	

