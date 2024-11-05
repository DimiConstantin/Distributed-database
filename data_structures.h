// Copyright (c) [2024], <Constantin Dimitrie> <<dimi.constantin27@gmail.com>>
#pragma once

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


typedef struct queue_t queue_t;
struct queue_t
{
	/* Dimensiunea maxima a cozii */
	unsigned int max_size;
	/* Dimensiunea cozii */
	unsigned int size;
	/* Dimensiunea in octeti a tipului de date stocat in coada */
	unsigned int data_size;
	/* Indexul de la care se vor efectua operatiile de front si dequeue */
	unsigned int read_idx;
	/* Indexul de la care se vor efectua operatiile de enqueue */
	unsigned int write_idx;
	/* Bufferul ce stocheaza elementele cozii */
	void **buff;
};


typedef struct ll_node_t
{
    void* data;
    struct ll_node_t* next;
} ll_node_t;

typedef struct linked_list_t
{
    ll_node_t* head;
    unsigned int size;
} linked_list_t;

typedef struct info info;
struct info {
	void *key;
	void *value;
};


typedef struct doc_type doc_type;
struct doc_type {
	void *key;
	void *content;
};

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

typedef struct hashtable_t hashtable_t;
struct hashtable_t {
	linked_list_t **buckets;
	unsigned int size;
	unsigned int hmax; /* Nr. de bucket-uri. */
	unsigned int (*hash_function)(void*);
	int (*compare_function)(void*, void*);
	void (*key_val_free_function)(void*);
};


queue_t *q_create(unsigned int data_size, unsigned int max_size);
unsigned int q_get_size(queue_t *q);
unsigned int q_is_empty(queue_t *q);
void * q_front(queue_t *q);
int q_dequeue(queue_t *q);
int q_enqueue(queue_t *q, void *new_data);
void q_clear(queue_t *q);
void q_free(queue_t *q);


linked_list_t *ll_create();
void ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data, unsigned int data_size);
ll_node_t *ll_remove_nth_node(linked_list_t* list, unsigned int n);
unsigned int ll_get_size(linked_list_t* list);
void ll_free(linked_list_t** pp_list);
void ll_print_int(linked_list_t* list);
void ll_print_string(linked_list_t* list);


int compare_function_ints(void *a, void *b);
int compare_function_chars(void *a, void *b);
int compare_function_strings(void *a, void *b);
void key_val_free_function(void *data);
hashtable_t *ht_create(unsigned int hmax, unsigned int (*hash_function)(void*), int (*compare_function)(void*, void*),
		void (*key_val_free_function)(void*));
int ht_has_key(hashtable_t *ht, void *key);
void *ht_get(hashtable_t *ht, void *key);
void ht_put(hashtable_t *ht, void *key, unsigned int key_size, void *value, unsigned int value_size);
void ht_remove_entry(hashtable_t *ht, void *key);
void ht_free(hashtable_t *ht);
unsigned int ht_get_size(hashtable_t *ht);
unsigned int ht_get_hmax(hashtable_t *ht);

doubly_linked_list_t *dll_create(unsigned int data_size);
dll_node_t *create_node(unsigned int data_size, const void* data);
dll_node_t *dll_get_nth_node(doubly_linked_list_t* list, int n);
void dll_add_nth_node(doubly_linked_list_t* list, int n, const void* new_data);
dll_node_t *dll_remove_nth_node(doubly_linked_list_t* list, int n);
unsigned int dll_get_size(doubly_linked_list_t* list);
void dll_free(doubly_linked_list_t** pp_list);
void dll_print_int_list(doubly_linked_list_t* list);
void dll_print_string_list(doubly_linked_list_t* list);
