#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include "Eqclass.h"
#include "image.h"
#include "colorimage.h"
#include "show_image.h"
#include "morphology.h"
#include "queue.h"
#include "point.h"
#include "heap.h"

int **strel(char *pattern,int size)
{
	int **SE;
	int i,j;
	SE=(int **)malloc(sizeof(int *)*size);
	for(i=0;i<size;i++)
	SE[i]=(int *)malloc(sizeof(int)*size);
	for(i=0;i<size;i++)
	{
		for(j=0;j<size;j++)
		{
			if(pattern[i*size+j]!='\0')
			SE[i][j]=pattern[i*size+j]-48;
			else
			return NULL;
		}
	}
	return SE;
}
int **diskstrel(int radius)
{
	int i,j,size;
	double s;
	int **SE;
	Point2D p,center;
	size=2*radius+1;
	setPoint(&center,size/2,size/2);
	SE=(int **)malloc(sizeof(int *)*size);
	for(i=0;i<size;i++)
	SE[i]=(int *)malloc(sizeof(int)*size);
	for(i=0;i<size;i++)
	{
		for(j=0;j<size;j++)
		{
			setPoint(&p,i,j);
			s=distEuclid(p,center);
			if(s<=radius)
			{
				SE[i][j]=1;
			}
			else
			{
				SE[i][j]=0;
			}
		}
	}
	return SE;
}
void freeSE(int ***matrix,int SE)
{
	int i;
	for(i=0;i<SE;i++)
	free(*((*matrix)+i));
	free(*matrix);
}
GrayImage imerode(GrayImage *originalImage,int **matrix,int SE)
{
	int i,j,s,t,min,k,p=0,q=0,flag=0;
	int **temp;
	GrayImage finalImage;
	k=SE/2;
	initGrayImage(&finalImage,originalImage->row,originalImage->col,originalImage->maxval);
	for(i=0;i<originalImage->row;i++)
	{
		for(j=0;j<originalImage->col;j++)
		{
			min=originalImage->maxval;
			flag=0;
			for(p=-k;p<=k;p++)
			{
				for(q=-k;q<=k;q++)
				{
					if(matrix[p+k][q+k]!=0)
					{
						s=i+p;
						t=j+q;
						if(s>=0 && s<originalImage->row && t>=0 && t<originalImage->col)
						{
							if(min>=originalImage->f[s][t])
							{
								min=originalImage->f[s][t];
								flag=1;
							}
						}
					}
				}
			}
			if(flag!=0)			
			finalImage.f[i][j]=min;
			else
			finalImage.f[i][j]=0;
		}
	}
	return finalImage;
}
GrayImage imdilate(GrayImage *originalImage,int **matrix,int SE)
{
	int i,j,s,t,max,p,q,k,flag;
	int **temp;
	GrayImage finalImage;
	initGrayImage(&finalImage,originalImage->row,originalImage->col,originalImage->maxval);
	k=SE/2;
	for(i=0;i<originalImage->row;i++)
	{
		for(j=0;j<originalImage->col;j++)
		{
			max=0;
			flag=0;
			for(p=-k;p<=k;p++)
			{
				for(q=-k;q<=k;q++)
				{
					if(matrix[p+k][q+k]!=0)
					{
						s=i+p;
						t=j+q;
						if(s>=0 && s<originalImage->row && t>=0 && t<originalImage->col)
						{
							if(max<=originalImage->f[s][t])
							{
								max=originalImage->f[s][t];
								flag=1;
							}
						}
					}
				}
			}
			if(flag!=0)			
			finalImage.f[i][j]=max;
			else
			finalImage.f[i][j]=0;
		}
	}
	return finalImage;
}
//****************************Hit or miss transform***********************//
GrayImage hitormiss(GrayImage *img,int **B,int SE,int match)
{
	int i,j,m,n,flag=0;
	GrayImage result;
	initGrayImage(&result,img->row,img->col,img->maxval);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			flag=0;
			for(m=-SE/2;m<=SE/2;m++)
			{
				for(n=-SE/2;n<=SE/2;n++)
				{
					if(i+m>=0 && i+m<img->row && j+n>=0 && j+n<img->col && B[m+SE/2][n+SE/2]<=1)
					{
						if(img->f[i+m][j+n]==(B[m+SE/2][n+SE/2]*img->maxval))
						flag++;
					}
				}
			}
			if(flag==match)
			result.f[i][j]=img->maxval;
			else
			result.f[i][j]=0;
		}
	}
	//imshow(result);
	return result;
}
//***************************Drawing line************************//
void drawLine(GrayImage *img,Point2D p1,Point2D p2) 
{ 
  int dx = abs(p2.x-p1.x), sx = p1.x<p2.x ? 1 : -1;
  int dy = abs(p2.y-p1.y), sy = p1.y<p2.y ? 1 : -1; 
  int err = (dx>dy ? dx : -dy)/2, e2;
 
  for(;;){
    img->f[p1.x][p1.y]=img->maxval;
    if (p1.x==p2.x && p1.y==p2.y) 
    break;
    e2 = err;
    if (e2 >-dx) { err -= dy; p1.x += sx; }
    if (e2 < dy) { err += dx; p1.y += sy; }
  }
}
		
//****************************Convex Hull************************//
int hasLeftTurn(Point2D p1,Point2D p2,Point2D p3)
{
	int line;
	line=(p2.y-p1.y)*(p3.x-p1.x)-(p2.x-p1.x)*(p3.y-p1.y);
	if(line>0)
	return 1;
	return 0;
	
}
void convexHull(GrayImage *img,GrayImage *result)
{
	Point2D *pts,*uhull,*lhull;
	int i,j,k,m,count=0;
	int **SE=strel("111111111",3);
	GrayImage temp;
	initGrayImage(&temp,img->row,img->col,img->maxval);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		temp.f[i][j]=0;
	}
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(img->f[i][j]!=0)
			count++;
		}
	}
	if(count<2)
	return;
	pts=(Point2D *)malloc(sizeof(Point2D)*count);
	uhull=(Point2D *)malloc(sizeof(Point2D)*count);
	lhull=(Point2D *)malloc(sizeof(Point2D)*count);
	k=0;
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(img->f[i][j]!=0)
			{
				setPoint(&pts[k],i,j);
				k++;
			}
		}
	}
	heapsort(pts,sizeof(Point2D),count,comparePts);
	uhull[0]=pts[0];
	uhull[1]=pts[1];
	k=2;
	for(i=2;i<count;i++)
	{
		uhull[k]=pts[i];
		while(k>=0 && k-1>=0 && k-2>=0 && hasLeftTurn(uhull[k-2],uhull[k-1],uhull[k]))
		{
			uhull[k-1]=uhull[k];
			k=k-1;
		}
		k++;
	}
	lhull[0]=pts[count-1];
	lhull[1]=pts[count-2];
	m=2;
	for(i=count-3;i>=0;i--)
	{
		lhull[m]=pts[i];
		while(m>=0 && m-1>=0 && m-2>=0 && hasLeftTurn(lhull[m-2],lhull[m-1],lhull[m]))
		{
			lhull[m-1]=lhull[m];
			m=m-1;
		}
		m++;
	}
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		result->f[i][j]=0;
	}
	for(i=0;i<k-1;i++)
	drawLine(&temp,uhull[i],uhull[i+1]);
	
	for(i=0;i<m-1;i++)
	drawLine(&temp,lhull[i],lhull[i+1]);
	drawLine(&temp,uhull[0],lhull[m-1]);
	drawLine(&temp,uhull[k-1],lhull[0]);
	temp=imdilate(&temp,SE,3);
	imfill(&temp,result);
	freeImage(&temp);
	temp=imcomplement(result);
	for(i=0;i<img->row;i++)
	for(j=0;j<img->col;j++)
	result->f[i][j]=temp.f[i][j];
	freeImage(&temp);
	free(uhull);
	free(lhull);
	free(pts);
}
void convexHullObjects(GrayImage *img,GrayImage *result)
{
	int *labels,n,i,j,k;
	GrayImage LI,temp,hull;
	initGrayImage(&temp,img->row,img->col,img->maxval);
	LI=imlabel(img,&labels,&n);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		result->f[i][j]=0;
	}
	for(k=0;k<n;k++)
	{
		for(i=0;i<img->row;i++)
		{
			for(j=0;j<img->col;j++)
			{
				if(LI.f[i][j]==labels[k])
				temp.f[i][j]=img->maxval;
				else
				temp.f[i][j]=0;
			}
		}
		initGrayImage(&hull,img->row,img->col,img->maxval);
		convexHull(&temp,&hull);
		for(i=0;i<img->row;i++)
		{
			for(j=0;j<img->col;j++)
			{
				if(hull.f[i][j]!=0)
				result->f[i][j]=hull.f[i][j];
			}
		}
		
		freeImage(&hull);
	}
	freeImage(&temp);
}
//***********Morphological opening operation******************//
void imopen(GrayImage *img,int **SE,int size,GrayImage *result)
{
	int i,j;
	GrayImage temp;
	temp=imerode(img,SE,size);
	temp=imdilate(&temp,SE,size);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			result->f[i][j]=temp.f[i][j];
		}
	}
	freeImage(&temp);
}
//**************Morphological closing operation****************//
void imclose(GrayImage *img,int **SE,int size,GrayImage *result)
{
	int i,j;
	GrayImage temp;
	temp=imdilate(img,SE,size);
	temp=imerode(&temp,SE,size);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			result->f[i][j]=temp.f[i][j];
		}
	}
	freeImage(&temp);
}
//**************Morphological top hat transform******************//			
void tophat(GrayImage *img,int **SE,int size,GrayImage *result)
{
	GrayImage temp;
	int i,j;
	temp=imerode(img,SE,size);
	temp=imdilate(&temp,SE,size);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			result->f[i][j]=img->f[i][j]-temp.f[i][j];
		}
	}
	freeImage(&temp);
}
//****************Geodesic Dilation******************************//
void gdilate(GrayImage *marker,GrayImage *mask,GrayImage *result)
{
	int **pat=strel("111111111",3);
	int i,j,flag;
	GrayImage temp;
	for(i=0;i<marker->row;i++)
	{
		for(j=0;j<marker->col;j++)
		result->f[i][j]=marker->f[i][j];
	}
	do
	{
		temp=imdilate(result,pat,3);
		flag=0;
		for(i=0;i<marker->row;i++)
		{
			for(j=0;j<marker->col;j++)
			{
				temp.f[i][j]=temp.f[i][j]>mask->f[i][j]?mask->f[i][j]:temp.f[i][j];
				if(result->f[i][j]!=temp.f[i][j])
				{
					flag=1;
				}
				result->f[i][j]=temp.f[i][j];
			}
		}
		freeImage(&temp);
	}
	while(flag!=0);
}		
//****************Hole filling************************//
void imfill(GrayImage *img,GrayImage *result)
{
	int i,j;
	GrayImage marker,mask;
	initGrayImage(&marker,img->row,img->col,img->maxval);
	initGrayImage(&mask,img->row,img->col,img->maxval);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(i==0 || j==0 || i==img->row-1 || j==img->col-1)
			marker.f[i][j]=img->maxval-img->f[i][j];
			else
			marker.f[i][j]=0;
		}
	}
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			mask.f[i][j]=img->maxval-img->f[i][j];
		}
	}
	fastgdilate(&marker,&mask,result);
}
//*******************Finding the regional maximum********************************//
void imregionalmax(GrayImage *mask,GrayImage *result)
{
	int i,j;
	GrayImage marker,tmp;
	initGrayImage(&marker,mask->row,mask->col,mask->maxval);
	initGrayImage(&tmp,mask->row,mask->col,mask->maxval);
	for(i=0;i<mask->row;i++)
	{
		for(j=0;j<mask->col;j++)
		{
			marker.f[i][j]=mask->f[i][j]-1;//>=0?mask->f[i][j]-1:0;
		}
	}
	fastgdilate(&marker,mask,result);
	for(i=0;i<mask->row;i++)
	{
		for(j=0;j<mask->col;j++)
		{
			result->f[i][j]=(mask->f[i][j]-result->f[i][j])==0?0:mask->maxval;
		}
	}
}
//*******************Fast hybrid algorithm for geodesic dilation of images**********//
void fastgdilate(GrayImage *marker,GrayImage *mask,GrayImage *result)
{
	GrayImage Id;
	struct node *pnode;
	int **SE=strel("111111111",3);
	int i,j,ix,iy,m,n,max,flag;
	Queue ptsQ;
	initQueue(&ptsQ);
	Point2D p;
	for(i=0;i<marker->row;i++)
	{
		for(j=0;j<marker->col;j++)
		{
			result->f[i][j]=marker->f[i][j];
		}
	}
	//**************************** Raster***********************
	for(i=0;i<mask->row;i++)
	{
		for(j=0;j<mask->col;j++)
		{
			max=0;
			for(m=-1;m<=1;m++)
			{
				for(n=-1;n<=1;n++)
				{
					ix=i+m;
					iy=j+n;
					if(ix>=0 && ix<marker->row && iy>=0 && iy<marker->col)
					{
						if(result->f[ix][iy]>max)
						{
							max=result->f[ix][iy];
							result->f[i][j]=max;
						}
					}
				}
			}
			result->f[i][j]=result->f[i][j]>mask->f[i][j]?mask->f[i][j]:result->f[i][j];
		}
	}
	//*******************************Anti Raster******************************
	for(i=mask->row-1;i>=0;i--)
	{
		for(j=mask->col-1;j>=0;j--)
		{
			max=0;
			for(m=-1;m<=1;m++)
			{
				for(n=-1;n<=1;n++)
				{
					ix=i+m;
					iy=j+n;
					if(ix>=0 && ix<marker->row && iy>=0 && iy<marker->col)
					{
						if(result->f[ix][iy]>max)
						{
							max=result->f[ix][iy];
							result->f[i][j]=max;
						}
					}
				}
			}
			result->f[i][j]=result->f[i][j]>mask->f[i][j]?mask->f[i][j]:result->f[i][j];
			flag=0;
			for(m=-1;m<=1;m++)
			{
				for(n=-1;n<=1;n++)
				{
					ix=i+m;
					iy=j+n;
					if(ix>=0 && ix<mask->row && iy>=0 && iy<mask->col && flag==0)
					{
						if(result->f[ix][iy]<result->f[i][j] && result->f[ix][iy]<mask->f[ix][iy])
						{
							setPoint(&p,i,j);
							enqueue(&ptsQ,&p,sizeof(Point2D));
							flag=1;
						}
					}
				}
			}
		}
	}
	while(!empty(ptsQ))
	{
		pnode=dequeue(&ptsQ);
		if(pnode->size!=sizeof(Point2D))
		{	
			printf("Not a point in the queue..Queue corrupted...\n");
			exit(-1);
		}
		p=*((Point2D *)pnode->data);
		for(m=-1;m<=1;m++)
		{
			for(n=-1;n<=1;n++)
			{
				ix=p.x+m;
				iy=p.y+n;
				if(ix>=0 && ix<marker->row && iy>=0 && iy<marker->col)
				{
					if(result->f[ix][iy]<result->f[p.x][p.y] && mask->f[ix][iy]!=result->f[ix][iy])
					{
						result->f[ix][iy]=result->f[ix][iy]>mask->f[ix][iy]?mask->f[ix][iy]:result->f[ix][iy];
						setPoint(&p,ix,iy);
						enqueue(&ptsQ,&p,sizeof(Point2D));
					}
				}
			}
		}
	}
}
void bwdist(GrayImage *img,GrayImage *result)
{
	Point2D *points,p;
	int i,j,count,nP=0,min;
	GrayImage temp;
	int **SE=strel("111111111",3);
	temp=imerode(img,SE,3);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		temp.f[i][j]=img->f[i][j]-temp.f[i][j];
	}
	imshow(temp);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(temp.f[i][j]!=0)
			nP++;
		}
	}
	printf("%d\n",nP);
	if(nP==0)
	{
		printf("Distance transform not possible\n");
		exit(-1);
	}
	points=(Point2D *)malloc(sizeof(Point2D)*nP);
	count=0;
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(temp.f[i][j]==img->maxval)
			{
				setPoint(&points[count],i,j);
				count++;
			}
		}
	}
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(img->f[i][j]!=0)
			result->f[i][j]=0;
			else
			{
				setPoint(&p,i,j);
				min=distEuclid(p,points[0]);
				for(count=0;count<nP;count++)
				min=distEuclid(p,points[count])<min?distEuclid(p,points[count]): min;
				result->f[i][j]=(int)min;
			}
		}
	}
}
//***************************Morphological Thining Algorithm****************************//
void skeleton(GrayImage *img,GrayImage *result)
{
	int **SE=strel("010111010",3);
	int i,j,flag;
	GrayImage EROS1,EROS2,Sn,OPEN;
	initGrayImage(&EROS1,img->row,img->col,img->maxval);
	//initGrayImage(&EROS2,img->row,img->col,img->maxval);
	initGrayImage(&Sn,img->row,img->col,img->maxval);
	printf("Skeletonizing innitiated\n");
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			EROS1.f[i][j]=img->f[i][j];
		}
	}
	while(1)
	{
		EROS2=imerode(&EROS1,SE,3);
		flag=0;
		for(i=0;i<img->row && !flag;i++)
		{
			for(j=0;j<img->col && !flag;j++)
			{
				if(EROS2.f[i][j]!=0)
				{
					flag=1;
					break; 
				}
			}
		}
		if(flag==0)
		{
			for(i=0;i<img->row;i++)
			{
				for(j=0;j<img->col;j++)
				{
					if(Sn.f[i][j]!=0 || EROS1.f[i][j]!=0)
					result->f[i][j]=img->maxval;
					else
					result->f[i][j]=0;
				}
			}
			return;
		}
		OPEN=imdilate(&EROS2,SE,3);
		for(i=0;i<img->row;i++)
		{
			for(j=0;j<img->col;j++)
			{
				if(Sn.f[i][j]!=0 || (EROS1.f[i][j]-OPEN.f[i][j])>0)
					Sn.f[i][j]=img->maxval;
				else
					Sn.f[i][j]=0;
				EROS1.f[i][j]=EROS2.f[i][j];
			}
		}
		imshow(Sn);
		getchar();
		freeImage(&OPEN);
		freeImage(&EROS2);
	}
	printf("Skeletonizing complete\n");
}
void boundaryOuter(GrayImage *img,GrayImage *result)
{
	int i,j;
	int **SE=strel("111111111",3);
	GrayImage temp;
	temp=imdilate(img,SE,3);
	for(i=0;i<img->row;i++)
	{
		for(j=0;j<img->col;j++)
		{
			if(temp.f[i][j]!=0 && img->f[i][j]==0)
			result->f[i][j]=img->maxval;
			else
			result->f[i][j]=0;
		}
	}
}
					
