struct cell
{
	int label;
	float strength;
	float *fv;
};
typedef struct
{
	struct cell **f;
	int row,col;
	int nfv;
} Cell;
typedef struct
{
	struct cell c;
	int x;
	int y;
} NewCell;
/*******************************************Functions*******************************************************************/
void initSeedPixel(GrayImage *,GrayImage *,GrayImage *,Cell *);
void getNeighbour2(int ix,int jy,NewCell *,Cell *);
float calDisMag(float *,float *,int);
float gFunction(float,float);
void cell2img(Cell *,GrayImage *);
void queue2img(Queue Q,GrayImage *img);
void syncgrowCut(Cell *,GrayImage *);

/*******************************************MACROS***********************************************************************/

