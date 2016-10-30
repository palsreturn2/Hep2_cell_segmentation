struct cell
{
	int label;
	float strength;
	int intensity;
};
typedef struct
{
	struct cell **f;
	int row,col;
} Cell;

void getImage(char *,char *,GrayImage *,GrayImage *);
void initSeedPixel(GrayImage *,GrayImage *,Cell *);
void getNeighbour(int ix,int jy,struct cell *,Cell *);
float calDisMag(struct cell cellOld_temp,struct cell N_temp);
float gFunction(float,int);
void growCut(Cell *,Cell *,GrayImage *);
void getSegment(Cell *,GrayImage *,GrayImage *);
void finalImage();
