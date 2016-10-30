typedef struct
{
	int *elem;
	int cardinality;
} Set;
struct eqclass
{
	int min;
	Set class;
	struct eqclass *next;
};
void initset(Set *);
int search(Set *s,int n);
int insert(Set *s,int n);
Set merge(Set *s1,Set *s2);
int minelem(Set s);
void printset(Set s);
void insertEq(struct eqclass** cls,int label);
int insert2Eq(struct eqclass *cls,int class,int label);
Set deleteEq(struct eqclass **cls,int key);
void mergeEq(struct eqclass **cls,int class,int label);
int searchEq(struct eqclass *cls,int label);
void printEq(struct eqclass *cls);
int countClasses(struct eqclass *);
