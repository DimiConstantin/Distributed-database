// Copyright (c) [2024], <Constantin Dimitrie> <<dimi.constantin27@gmail.com>>
#ifndef __LISTS_H__
#define __LISTS_H__

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);				        \
		}							\
	} while (0)

#define MAX_STRING_SIZE 64


typedef struct ll_node_t
{
    void* data;
    struct ll_node_t* next;
} ll_node_t;

typedef struct linked_list_t
{
    ll_node_t* head;
    int size;
} linked_list_t;

typedef struct dll_node_t dll_node_t;
struct dll_node_t
{
    void* data;
    dll_node_t *prev, *next;
};

typedef struct doubly_linked_list_t doubly_linked_list_t;
struct doubly_linked_list_t {
	dll_node_t *head;
	dll_node_t *tail;
	int size;
    int data_size;
};


typedef struct doc_type doc_type;
struct doc_type {
	void *key;
	void *content;
};

// --------------------DOUBLY LINKED LIST------------------//

doubly_linked_list_t *dll_create(unsigned int data_size);
dll_node_t *create_node(unsigned int data_size, const void* data);
dll_node_t *dll_get_nth_node(doubly_linked_list_t* list, int n);
void dll_add_nth_node(doubly_linked_list_t* list, int n, const void* new_data);
dll_node_t *dll_remove_nth_node(doubly_linked_list_t* list, int n);
unsigned int dll_get_size(doubly_linked_list_t* list);
void dll_free(doubly_linked_list_t** pp_list);
void dll_print_int_list(doubly_linked_list_t* list);
void dll_print_string_list(doubly_linked_list_t* list);

// --------------LINKED LIST---------------//

linked_list_t *ll_create();
void ll_add_nth_node(linked_list_t* list, int n, const void* new_data, unsigned int data_size);
ll_node_t *ll_remove_nth_node(linked_list_t* list, int n);
int ll_get_size(linked_list_t* list);
void ll_free(linked_list_t** pp_list);
void ll_print_int(linked_list_t* list);
void ll_print_string(linked_list_t* list);

#endif

