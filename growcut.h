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
typedef struct
{
	struct cell c;
	int x;
	int y;
} NewCell;
/*******************************************Functions*******************************************************************/
void initSeedPixel(GrayImage *,GrayImage *,GrayImage *,Cell *);
void getNeighbour(int ix,int jy,struct cell *,Cell *);
void getNeighbour2(int ix,int jy,NewCell *,Cell *);
int calDisMag(struct cell cellOld_temp,struct cell N_temp);
float gFunction(int,int);
void cell2img(Cell *,GrayImage *);
void queue2img(Queue Q,GrayImage *img);
void growCut(Cell *,GrayImage *);
void fastgrowCut(Cell *,GrayImage *);
void fastergrowCut(Cell *,GrayImage *);
void syncgrowCut(Cell *,GrayImage *);

