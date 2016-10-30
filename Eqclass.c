#include <stdio.h>
#include <stdlib.h>
#include "Eqclass.h"

#define SEED 256

void initset(Set *s)
{
	s->elem=NULL;
	s->cardinality=0;
}
int search(Set *s,int n)
{
	if(s->cardinality==0)
	return 0;
	int i;
	for(i=0;i<s->cardinality;i++)
	{
		if(s->elem[i]==n)
		return 1;
	}
	return 0;
}
void sortseed(int *a,int n)
{
	int count[SEED];
	int i,j,c=0;
	for(i=0;i<SEED;i++)
	count[i]=0;
	for(i=0;i<n;i++)
	count[a[i]]++;
	for(i=0;i<SEED;i++)
	{
		while(count[i]!=0)
		{
			a[c]=i;
			count[i]--;
			c++;
		}
	}
	return;
}
int insert(Set *s,int n)
{
	int ch=search(s,n);
	if(ch==0)
	{
		s->cardinality++;
		if(s->cardinality==1)
		{
			s->elem=(int *)malloc(sizeof(int));
			s->elem[s->cardinality-1]=n;
			return 1;
		}
		else
		{
			s->elem=(int *)realloc(s->elem,s->cardinality*sizeof(int));
			s->elem[s->cardinality-1]=n;
			return 1;
		}
	}
	return 0;
}
Set merge(Set *s1,Set *s2)
{
	int i;
	for(i=0;i<s1->cardinality;i++)
	{
		insert(s2,s1->elem[i]);
	}
	return *s2;
}
int minelem(Set s)
{
	int i,sm;
	if(s.cardinality==0)
	return -1;
	sm=s.elem[0];
	for(i=0;i<s.cardinality;i++)
	{
		if(sm>s.elem[i])
		sm=s.elem[i];
	}
	return sm;
}
void printset(Set s)
{
	int i;
	for(i=0;i<s.cardinality;i++)
	printf("%d ",s.elem[i]);
	printf("%d\n",s.cardinality);
}
void insertEq(struct eqclass** cls,int label)
{
	Set s;
	struct eqclass *temp;
	initset(&s);
	insert(&s,label);
	if((*cls)==NULL)
	{
		(*cls)=(struct eqclass *)malloc(sizeof(struct eqclass));
		(*cls)->class=s;
		(*cls)->next=NULL;
		(*cls)->min=label;
		return;
	}
	temp=(struct eqclass *)malloc(sizeof(struct eqclass));
	temp->class=s;
	temp->next=(*cls);
	(*cls)=temp;
	(*cls)->min=label;
}
int insert2Eq(struct eqclass *cls,int class,int label)
{
	struct eqclass *ptr=cls;
	Set temp;
	int ch;
	while(ptr!=NULL)
	{
		temp=ptr->class;
		if(search(&temp,class)==1)
		{
			ch=insert(&temp,label);
			ptr->min=minelem(temp);
			ptr->class=temp;
			if(ch==1)
			return 1;
			break;
		}
		ptr=ptr->next;
	}
	return 0;
}
Set deleteEq(struct eqclass **cls,int key)
{
	struct eqclass *ptr,*nptr;
	Set temp;
	ptr=(*cls);
	nptr=ptr;
	if(ptr->min==key)
	{
		(*cls)=(*cls)->next;
		temp=ptr->class;
		free(ptr);
		return temp;
	}
	while(ptr!=NULL)
	{
		if(key==ptr->min)
		{
			nptr->next=ptr->next;
			ptr->next=NULL;
			temp=ptr->class;
			free(ptr);
			return temp;
		}
		nptr=ptr;
		ptr=ptr->next;		
	}
}
void mergeEq(struct eqclass **cls,int class,int label)
{
	struct eqclass *ptr=(*cls);
	Set temp,s1;
	int flag=1;
	if(class==label)
	return;
	while(ptr!=NULL)
	{
		temp=ptr->class;
		if(search(&temp,label)==1)
		{
			if(search(&temp,class)==1)
			return;
			s1=deleteEq(cls,ptr->min);
			flag=0;
			break;
		}
		ptr=ptr->next;
	}
	ptr=(*cls);
	if(flag==1)
	{
		//printf("%d %d\n",class,label);
	}
	while(ptr!=NULL)
	{
		temp=ptr->class;
		if(search(&temp,class)==1)
		{
			ptr->class=merge(&s1,&temp);
			ptr->min=minelem(ptr->class);
			break;
		}
		ptr=ptr->next;
	}
}
int searchEq(struct eqclass *cls,int label)
{
	struct eqclass *ptr=cls;
	Set temp;
	while(ptr!=NULL)
	{
		temp=ptr->class;
		if(ptr->min==label)
		return ptr->min;
		if(search(&temp,label)==1)
		return ptr->min;
		ptr=ptr->next;
	}
	return 0;
}
void printEq(struct eqclass *cls)
{
	struct eqclass *ptr=cls;
	while(ptr!=NULL)
	{
		printf("%d\n",ptr->min);
		ptr=ptr->next;
	}
}
int countClasses(struct eqclass *cls)
{
	int n=0;
	struct eqclass *ptr=cls;
	while(ptr!=NULL)
	{
		n++;
		ptr=ptr->next;
	}
	return n;
}
/*main()
{
	struct eqclass *cls=NULL;
	insertEq(&cls,1);
	printEq(cls);
	insertEq(&cls,2);
	printEq(cls);
	insertEq(&cls,3);
	printEq(cls);
	insertEq(&cls,4);
	printEq(cls);
	insertEq(&cls,5);
	mergeEq(&cls,1,2);
	mergeEq(&cls,1,3);
	mergeEq(&cls,1,3);
	mergeEq(&cls,5,4);
	mergeEq(&cls,5,1);
	printEq(cls);
}*/
	
	
	
	
	
	
	
	
	
	
	
