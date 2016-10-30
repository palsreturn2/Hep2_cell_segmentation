#include "heap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
void initHeap(Heap *h,int sD,int sH)
{
	h->sizeData=sD;
	h->elems=0;
	h->maxelems=sH;
	h->data=malloc(sD*sH);
}
int emptyH(Heap *h)
{
	if(h->elems==0)
	return 1;
	return 0;
}
int full(Heap *h)
{
	if(h->elems>=h->maxelems)
	return 1;
	return 0;
}
void shiftup(Heap *h,int compare(void *,void *))
{
	int root,flag;
	void *temp,*n1,*n2;
	root=h->elems;
	
	temp=malloc(h->sizeData);
	while(root>1)
	{
		n1=h->data+((root-1)*h->sizeData);
		root=root/2;
		n2=h->data+((root-1)*h->sizeData);
		flag=compare(n1,n2);
		if(flag<0)
		{
			memcpy(temp,n1,h->sizeData);
			memcpy(n1,n2,h->sizeData);
			memcpy(n2,temp,h->sizeData);
		}
		else
		break;
	}
	free(temp);
	return;
}
void shiftdown(Heap *h,int compare(void *,void *))
{
	void *n1,*n2,*n3,*t;
	int root=1,child,swap;
	
	t=malloc(h->sizeData);
	while((2*root)<=h->elems)
	{
		child=(2*root);
		swap=root;
		
		n1=h->data+((swap-1)*h->sizeData);
		n2=h->data+((child-1)*h->sizeData);
		n3=h->data+((child)*h->sizeData);
		if(compare(n1,n2)>0)
		swap=child;
		
		if(child+1<=h->elems && compare(n1,n3)>0)
		{
			if(compare(n2,n3)<0)
			swap=child;
			else
			swap=child+1;
		}
		
		if(swap!=root)
		{
			memcpy(t,h->data+((swap-1)*h->sizeData),h->sizeData);
			memcpy(h->data+((swap-1)*h->sizeData),h->data+((root-1)*h->sizeData),h->sizeData);
			memcpy(h->data+((root-1)*h->sizeData),t,h->sizeData);
			root=swap;
		}
		else
		break;
	}
	free(t);
}
				
void insert2Heap(Heap *h,void *el,int size,int compare(void *,void *))
{
	if(size!=h->sizeData || full(h))
	{
		printf("Size of element not equal to element size of heap\n");
		exit(1);
	}
	void *temp=h->data+(h->sizeData*h->elems);
	memcpy(temp,el,h->sizeData);
	h->elems++;
	shiftup(h,compare);
}
void *del4mHeap(Heap *h,int compare(void *,void *))
{
	void *temp=malloc(h->sizeData);
	if(h->elems<=0)
	{
		printf("Deletion from heap not possible\n");
		exit(1);
	}
	memcpy(temp,h->data,h->sizeData);
	memcpy(h->data,h->data+((h->elems-1)*h->sizeData),h->sizeData);
	memcpy(h->data+((h->elems-1)*h->sizeData),temp,h->sizeData);
	h->elems--;
	shiftdown(h,compare);
	return temp;
}
void heapsort(void *arr,int elsize,int size,int compare(void *,void *))
{
	Heap h;
	void *element;
	initHeap(&h,elsize,size);
	int i;
	for(i=0;i<size;i++)
	{
		insert2Heap(&h,arr+i*elsize,elsize,compare);
	}
	i=0;
	while(!emptyH(&h))
	{
		element=del4mHeap(&h,compare);
		memcpy((arr+i*elsize),element,elsize);
		i++;
	}
}		
	
/*int compareInts(void *a,void *b)
{
	int *n1,*n2;
	n1=(int *)a;
	n2=(int *)b;
	if((*n1)>(*n2))
	return 1;
	else if((*n1)<(*n2))
	return -1;
	return 0;
}
main()
{
	Heap H;
	int n,*t,i;
	int arr[]={5,4,3,7,100};
	heapsort(arr,sizeof(int),5,compareInts);
	for(i=0;i<5;i++)
	printf("%d ",arr[i]);
	printf("\n");
}*/
				
	
	
