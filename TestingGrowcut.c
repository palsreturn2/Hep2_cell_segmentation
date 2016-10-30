#include<stdio.h>
#include<stdlib.h>
#include <math.h>
#include "image.h"
#include "show_image.h"
#include "morphology.h"
#include "filtersAndGrads.h"
#include "texture.h"
#include "seedgen.h"
#include "growcut.h"

main(int argc,char **argv)
{
	GrayImage fgm,bgm,img,result,temp;
	readPGM(argv[1],&img);
	readPGM(argv[2],&fgm);
	readPGM(argv[3],&bgm);
	initGrayImage(&temp,img.row,img.col,img.maxval);
	printf("Transform started\n");
	bwdist(&fgm,&temp);
	label2rgb(&temp);
	exit(1);
	initGrayImage(&result,img.row,img.col,img.maxval);
	Cell cellOld;
	int i,j;
	cellOld.f=(struct cell **)malloc(sizeof(struct cell *) * img.row);
	for(i=0;i<img.row;i++)
	cellOld.f[i]=(struct cell *)malloc(sizeof(struct cell) * img.col);
	cellOld.row=img.row;
	cellOld.col=img.col;
	initSeedPixel(&img,&fgm,&bgm,&cellOld);
	growCut(&cellOld,&result);
	label2rgb(&result);
}
