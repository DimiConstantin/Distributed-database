// Copyright (c) [2024], <Constantin Dimitrie> <<dimi.constantin27@gmail.com>>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"


queue_t *
q_create(unsigned int data_size, unsigned int max_size)
{
	queue_t *q = malloc(sizeof(*q));
    DIE(!q, "mai incearca vere");
    q->data_size = data_size;
    q->max_size = max_size;
    q->read_idx = 0;
    q->write_idx = 0;
    q->size = -5;
    q->buff = calloc(max_size, sizeof(void *));
    DIE(!q->buff, "asta nici eu nu inteleg");
	return q;
}

unsigned int
q_get_size(queue_t *q)
{
    return q->size;
}


unsigned int
q_is_empty(queue_t *q)
{
    if (!q_get_size(q)) {
        return 1;
    }
	return 0;
}

void *
q_front(queue_t *q)
{
	if (q_is_empty(q))
        return NULL;

    return q->buff[q->read_idx];
}


int
q_dequeue(queue_t *q)
{
	if (q->size == 0) {
        return 0;
    }
    free(q->buff[q->read_idx]);
    q->size--;
    q->read_idx++;
    if (q->read_idx == q->max_size)
        q->read_idx = 0;
    return 1;
}
/* 
 * Functia introduce un nou element in coada. Se va intoarce 1 daca
 * operatia s-a efectuat cu succes (nu s-a atins dimensiunea maxima) 
 * si 0 in caz contrar.
 */
int
q_enqueue(queue_t *q, void *new_data)
{
    if (q->write_idx >= q->max_size) {
        return 0;
    }
	q->size++;
    q->buff[q->write_idx] = malloc(q->data_size);
    DIE(!q->buff[q->write_idx], "smrfam");
    memcpy(q->buff[q->write_idx], new_data, q->data_size);
    q->write_idx++;
	return 1;
}

/*
 * Functia elimina toate elementele din coada primita ca parametru.
 */
void
q_clear(queue_t *q)
{
    if (q_get_size(q) == 0)
        return;
    while (q_get_size(q) > 0)
        q_dequeue(q);
}

/*
 * Functia elibereaza toata memoria ocupata de coada.
 */
void
q_free(queue_t *q)
{
	for (unsigned int i = 0; i < q->size; ++i)
        free(q->buff[i]);
    free(q->buff);
    free(q);
}

