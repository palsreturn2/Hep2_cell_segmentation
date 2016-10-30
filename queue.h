struct node
{
	void *data;
	int size;
	struct node *next;
};
typedef struct
{
	struct node *front,*rear;
} Queue;


void setNode(struct node *n,void *data,int size);
void initQueue(Queue *q);
int empty(Queue q);
void enqueue(Queue *q,void *data,int size);
struct node *dequeue(Queue *q);


		
	
	
