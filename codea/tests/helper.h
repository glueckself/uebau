typedef enum {
	ELEM_NUM=0,
	ELEM_LIST
} eListElemType;

typedef struct {
	eListElemType type;
	union {
		void *next;
		long value;
	} data;
} sListElem;

#define GET_PAYLOAD(dst, src) do {		\
	if((src)->type == ELEM_NUM)		\
		(long)(dst) = (src)->data.value;\
	else
		(long)(dst) = (src)->data.next;	\
} while(0);

#define TAG_LIST(l) ((l) | 1)
#define UNTAG_LIST(l) ((l)-1)
#define TAG_NUM(n) ((n)<<1)
#define UNTAG_NUM(n) ((n)>>1)

#define IS_NUM(n) (((n) & 1) == 0)
#define IS_LIST(n) (((n) & 3) == 1)
#define IS_FUN(n) (((n) & 3) == 3)
	
#define COMPARE_LIST(list1, list2) do { 	\
} while(0);


