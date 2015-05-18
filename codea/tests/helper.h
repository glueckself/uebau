typedef enum {
	ELEM_NUM=0,
	ELEM_LIST,
        ELEM_END
} eListElemType;

typedef struct {
	eListElemType type;
	
        void *next;
        long value;
    
} sListElem;

#define GET_PAYLOAD(dst, src) do {		\
	if((src)->type == ELEM_NUM)		\
		(long)(dst) = (src)->value;     \
	else                                    \
		(long)(dst) = (src)->next;	\
} while(0);

#define TAG_LIST(l) ((void*)((long)(l) | 1))
#define UNTAG_LIST(l) ((void*)((long)(l)-1))
#define TAG_NUM(n) ((n)<<1)
#define UNTAG_NUM(n) ((n)>>1)

#define IS_NUM(n) (((long)(n) & 1) == 0)
#define IS_LIST(n) (((long)(n) & 3) == 1)
#define IS_FUN(n) (((long)(n) & 3) == 3)

#define LIST_HEAD(l) ( *((long*)l) )
#define LIST_TAIL(l) ( *( ((long*)l)+1 ) )
	
#define LIST_EQUAL(list1, list2) do {   	\
    int i;                                      \
    long val;                                   \
    void *l1;                                   \
    sListElem *l2;                              \
    l1=(list1);                                 \
    l2=(list2);                                 \
    for(i=0; l2[i].type != ELEM_END; i++) {     \
        printf("checking list at %x\n", l1);    \
        if(!IS_LIST(l1)) {                      \
            printf("not a list: %x\n", (long)l1); \
            return 0;                           \
        }                                       \
        l1=UNTAG_LIST(l1);                      \
        val=LIST_HEAD(l1);                      \
        if(val != l2[i].value) {                \
            printf("value mismatch: %d/%d\n", val, l2[i].value); \
            return 0;                           \
        }                                       \
        l1=LIST_TAIL(l1);                       \
    }                                           \
    return 1;                                   \
} while(0)


