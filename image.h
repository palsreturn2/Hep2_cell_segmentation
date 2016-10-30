typedef struct
{
	int **f;
	int row;
	int col;
	int maxval;
} GrayImage;

typedef struct
{
	int **f;
	int row;
	int col;
} BinImage;

int graythres(GrayImage *);
GrayImage genRandomImg(int ,int ,int);
GrayImage getSeedImage(char *);
GrayImage getSeedImage3(GrayImage *,int);
GrayImage *zeros(int,int);
GrayImage im2bw(GrayImage,int);
GrayImage imcomplement(GrayImage *);
GrayImage splitImage(GrayImage *img,int x1,int y1,int x2,int y2);
void initGrayImage(GrayImage *img,int row,int col,int maxval);
void normalize(GrayImage *,GrayImage *);
void imhist(GrayImage *);
GrayImage imlabel(GrayImage *,int **,int *);
void freeImage(GrayImage *img);
void bwareaopen(GrayImage *,GrayImage *,int);
void superimpose(GrayImage *img,GrayImage *bin,int ns,GrayImage *output);
