#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include <string.h>

void setNode(struct node *n,void *data,int size)
{
	memcpy(n->data,data,size);
	n->size=size;
	n->next=NULL;
}
void initQueue(Queue *q)
{
	q->front=q->rear=NULL;
}
int empty(Queue q)
{
	if(q.front==NULL && q.rear==NULL)
	return 1;
	return 0;
}
void enqueue(Queue *q,void *data,int size)
{
	struct node *n=(struct node *)malloc(sizeof(struct node));
	n->data=malloc(size);
	setNode(n,data,size);
	if(empty(*q))
	{
		q->front=q->rear=n;
		return;
	}
	q->rear->next=n;
	q->rear=n;
}
struct node *dequeue(Queue *q)
{
	struct node *tmp;
	if(empty(*q))
	{
		printf("Queue underflow\n");
		exit(-1);
	}
	if(q->front==q->rear)
	{
		tmp=q->front;
		q->front=q->rear=NULL;
		return tmp;
	}
	tmp=q->front;
	q->front=q->front->next;
	return tmp;
}
/****************************For Debugging purpose*********************************
main()
{
	Queue q;
	initQueue(&q);
	int i,*d;
	for(i=0;i<5;i++)
	{
		enqueue(&q,&i,sizeof(int));
	}
	while(!empty(q))
	{
		struct node *temp=dequeue(&q);
		d=(int *)temp->data;
		printf("%d\n",*d);	
		free(temp);
	}
}*/
