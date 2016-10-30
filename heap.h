typedef struct
{
	void *data;
	int sizeData;
	int elems;
	int maxelems;
} Heap;

int emptyH(Heap *h);
void initHeap(Heap *h,int sD,int sH);
void shiftup(Heap *h,int compare(void *,void *));
void shiftdown(Heap *h,int compare(void *,void *));
void insert2Heap(Heap *h,void *el,int size,int compare(void *,void *));
void *del4mHeap(Heap *h,int compare(void *,void *));


