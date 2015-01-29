/* Author: Luming Zhang
   Functions imeplemented: Mem_Init, Mem_Alloc, Mem_Free, Mem_Dump
*/


#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mem.h"



#define DEBUG 0
#define MERGE 1
/***** Policies *****/
#define BF 0 // BESTFIT
#define WF 1 // WORSTFIT
#define FF 2 // FIRSTFIT

#define MAGIC_NUM 1010;

typedef struct __header_t 
{
    int size;
    int magic;
} header_t;


typedef struct __node_t
{
    int size;
    struct __node_t *next;
    struct __node_t *prev;
} node_t;

node_t* head = 0;

int m_error = 0;
int init = 0;

void merge_prev(node_t* node_to_merge)
{
    node_t* prev_node;
    if(node_to_merge->prev==NULL)
    {
	   return;
    }
    else
    {
    	prev_node = node_to_merge->prev;
    	if(prev_node->size + (long)prev_node == (long)node_to_merge)
    	{
    	    prev_node->size += node_to_merge->size;
    	    prev_node->next = node_to_merge->next;
    	}
    }
}

void merge_next(node_t* node_to_merge)
{
    node_t* next_node;
    if(node_to_merge->next==NULL)
    {
        return;
    }
    else
    {
        next_node = node_to_merge->next;
        if(node_to_merge->size + (long)node_to_merge == (long)next_node)
        {
            node_to_merge->size += next_node->size;
            node_to_merge->next = next_node->next;
        }
    }
}




void insert_inorder(node_t* node_to_insert)
{
    node_t* curr = head;
    if(head==NULL)
    {
	head = node_to_insert;	
    }
    else//head points to certain node
    {
	while(curr!=NULL)//while curr is not null
	{
	    if(curr<node_to_insert)//if address of curr smaller than node to in
	    {
    		if(curr->next==NULL)//curr is the last node
    		{
    		    curr->next = node_to_insert;
    		    node_to_insert->prev = curr;
    		    node_to_insert->next = NULL;
    		    if(MERGE) merge_prev(node_to_insert);
    		    return;
    		}
    		else // curr isn't the last node
    		{
    		    curr = curr->next;
    		}
	    }
	    else // curr>node_to_insert
	    {
    		if(curr->prev!=NULL)
    		{
    		    node_to_insert->next = curr;
    		    node_to_insert->prev = curr->prev;
    		    curr->prev->next = node_to_insert;
    		    curr->prev = node_to_insert;
    		    if(MERGE)
    		    {
    			merge_prev(node_to_insert);
    			merge_next(node_to_insert);
    		    }
    		}
    		else // curr->prev==NULL
    		{
    		    node_to_insert->prev = NULL;
    		    node_to_insert->next = curr;
    		    head = node_to_insert;
    		    curr->prev = node_to_insert;
    		    if(MERGE) merge_next(node_to_insert);

    		}
		    return;
	    }

	} 
    

    }
}



int Mem_Init(int sizeOfRegion)
{
    int page_size=0;
    int quotient = 0;
    int remainder = 0;
    int number_bytes = 0;
    int fd = open("/dev/zero", O_RDWR);

    if(init)
    {
	   return -1;
    }

    if(sizeOfRegion<=0)
    {
    	m_error = E_BAD_ARGS;
    	return -1;
    }
    page_size = getpagesize();
    
    if(DEBUG)
    {
	   printf("Page size is %d\n",page_size);
    }
    quotient = sizeOfRegion/page_size;
    remainder = sizeOfRegion%page_size;

    if(DEBUG)
    {
	   printf("Quotient is %d, remainder is %d\n",quotient,remainder);
    }
    if(remainder) 
        quotient++;
    number_bytes = quotient*page_size;
    if(DEBUG)
    {
	   printf("Number of byte to be request is %d\n",number_bytes);
    }
    head = mmap(NULL,number_bytes,PROT_READ | PROT_WRITE,MAP_PRIVATE,fd,0);
    
    if (head == MAP_FAILED) 
    {
    	perror("mmap"); 
    	exit(1); 
    }
    if(DEBUG)
    {
	   printf("Address of request memory is %p\n",head);
    }
    close(fd);
    head->size = number_bytes;
    head->next = NULL;
    head->prev = NULL;
    init++;
    return 0;
}

void *Mem_Alloc(int size, int style)
{
    int size_alloc = 0;
    int found = 0;

    /*int wrost = 0;
    node_t* wrost_node;
    int best = 0;
    node_t* best_node;
    */
    void* ptr;
    node_t* curr;
    header_t* header;
    node_t* tmp;
    int tmp_size;
    node_t *tmp_next;
    node_t *tmp_prev;
 
    void* result;

    if(size%8!=0)
    {
	   size_alloc = ((size/8)+1)*8;
    }
    if(DEBUG)
    {
    	printf("\nMem_Alloc begins.\n");
    	printf("Original size to alloc: %d, actual size: %d\n",size,size_alloc);
    }
    curr = head;
    while((curr!=NULL)&&(found==0))
    {
	   if(DEBUG) printf("Current node: %p.\n",curr);
    	if(curr->size >= size_alloc+sizeof(header_t))
    	{
    	    tmp_size = curr->size;//save the info about this node
    	    tmp_next = curr->next;
    	    tmp_prev = curr->prev;
    	    ptr = curr;// - 2*sizeof(int);
    	    header = (header_t*)ptr;
    	    header->size = size_alloc;
    	    header->magic = MAGIC_NUM;
    	    if(DEBUG)
    	    {
    		printf("Size of header: %ld\n",sizeof(header_t));
    		printf("Header begins at %p\n",header);
    		printf("Size to allocate is %d\n",header->size);
    		
    	    }
    	    tmp_size = tmp_size - size_alloc - sizeof(header_t);
    	    if(DEBUG) printf("Space remaining: %d.\n",tmp_size);
    	    found = 1;
    	    
    	}
	   if(found==0) curr = curr->next;
    }
   
    result = header+1;
    tmp = (node_t*)(result+size_alloc);
    tmp->size = tmp_size;
    tmp->next = tmp_next;
    tmp->prev = tmp_prev;
    if(tmp->prev==NULL)
    {
	   head = tmp;
    }
    else
    {
	   tmp->prev->next = tmp;
    }

    if(DEBUG)
    {
	   printf("Allocated address: %p\n",result);
	   printf("Mem_Alloc ends.\n");
    }
    return result;
	
}

int Mem_Free(void *ptr)
{
    int size_freed = 0;
    int magic = 0;
    node_t* tmp;
    if(ptr==NULL) return 0;
    size_freed = *(int*)(ptr-8)+sizeof(header_t);
    magic = *(int*)(ptr-4);
    if(DEBUG)
    {
    	printf("\nMem_Free begins.\n");
    	printf("The address to be freed: %p\n",ptr);
    	printf("Size to be freed(header included): %d\n",size_freed);
    	printf("Magic number is %d\n",magic);
    }
    tmp = (node_t*)(ptr-8);
    tmp->size = size_freed;
    tmp->next = NULL;
    tmp->prev = NULL;
    if(DEBUG)
    {
    	printf("The new free list node: %p\n",tmp);
    	printf("Mem_Free ends.\n");
    } 
    insert_inorder(tmp);
    return 0;
}

void Mem_Dump()
{
    node_t* curr;
    curr = head;
    printf("\nDump begins.\n");
    while(curr!=NULL)
    {
    	printf("Size of region: %d, next node: %p, prev node: %p\n",curr->size,curr->next,curr->prev);
    	curr = curr->next;
    }
    printf("Dump ends.\n");
}

