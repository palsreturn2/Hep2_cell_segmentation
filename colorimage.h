typedef struct
{
	int **f[3];
	int row;
	int col;
	int maxval;
} ColorImage;

typedef struct
{
	float **f[3];
	int row;
	int col;
} HSIImage;

void initColorImage(ColorImage *,int,int,int);
void rgb2gray(ColorImage *,GrayImage *);

