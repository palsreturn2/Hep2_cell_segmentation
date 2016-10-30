#include<stdio.h>
#include<stdlib.h>
#define MAX 1400
void matrixGen(int matrix[MAX][MAX])//generates a matrix of random numbers
{
	int i,j;
	FILE* fp;
	fp=fopen("random.txt","w");
	for(i=0;i<MAX;i++)
	{
		for(j=0;j<MAX;j++)
		{
			matrix[i][j]=rand()%100;
			fprintf(fp,"%d ",matrix[i][j]);
		}
		fprintf(fp,"\n");
	}
}

int findMax(int matrix[MAX][MAX])//find max of the numbers stored in a matrix
{
	int i,j;
	int max=matrix[0][0];
	for(i=0;i<MAX;i++)
	{
		for(j=0;j<MAX;j++)
		{
			if(max<matrix[i][j])
			{
				max=matrix[i][j];
			}
		}
	}
	return max;
}


