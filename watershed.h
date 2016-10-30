typedef struct 
{
	Point2D p;
	int intensity;
} Wnode;
int comparenode(void *p1,void *p2);
void imimposemin(GrayImage *img,GrayImage *marker,GrayImage *result);
void watershed(GrayImage *img,GrayImage *result);
