// Copyright (c) [2024], <Constantin Dimitrie> <<dimi.constantin27@gmail.com>>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

/*
 * Functii de comparare a cheilor:
 */
int compare_function_ints(void *a, void *b)
{
	int int_a = *((int *)a);
	int int_b = *((int *)b);

	if (int_a == int_b) {
		return 0;
	} else if (int_a < int_b) {
		return -1;
	} else {
		return 1;
	}
}

int compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	if (strcmp(str_a, str_b) == 0) {
		return 0;
	} else if (strcmp(str_a, str_b) < 0) {
		return -1;
	} else {
		return 1;
	}
}

/*
 * Functii de hashing:
 */
unsigned int hash_function_int(void *a)
{
	/*
	 * Credits: https://stackoverflow.com/a/12996028/7883884
	 */
	unsigned int uint_a = *((unsigned int *)a);

	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = (uint_a >> 16u) ^ uint_a;
	return uint_a;
}

unsigned int hash_function_string(void *a)
{
	/*
	 * Credits: http://www.cse.yorku.ca/~oz/hash.html
	 */
	unsigned char *puchar_a = (unsigned char*) a;
	unsigned long hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

	return hash;
}

/*
 * Functie apelata pentru a elibera memoria ocupata de cheia si valoarea unei
 * perechi din hashtable. Daca cheia sau valoarea contin tipuri de date complexe
 * aveti grija sa eliberati memoria luand in considerare acest aspect.
 */
void key_val_free_function(void *data) {
    info *data_info = data;
    free(data_info->key);
    free(data_info->value);
}

/*
 * Functie apelata dupa alocarea unui hashtable pentru a-l initializa.
 * Trebuie alocate si initializate si listele inlantuite.
 */
hashtable_t *ht_create(int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*),
		void (*key_val_free_function)(void*))
{
    hashtable_t *map = malloc(sizeof(*map));
    DIE(!map, "oooooooo ceausesculeeeee");
    map->compare_function = compare_function;
    map->hash_function = hash_function;
    map->key_val_free_function = key_val_free_function;
    map->hmax = hmax;
    map->size = 0;
    map->buckets = (linked_list_t **)calloc(hmax, sizeof(linked_list_t *));
    DIE(!map->buckets, "ceai facut vere aici");
    for (int i = 0; i < hmax; ++i) {
        map->buckets[i] = ll_create();
    }
    return map;
}

/*
 * Functie care intoarce:
 * 1, daca pentru cheia key a fost asociata anterior o valoare in hashtable
 * folosind functia put;
 * 0, altfel.
 */
int ht_has_key(hashtable_t *ht, void *key)
{
    unsigned int idx = ht->hash_function(key) % ht->hmax;
    ll_node_t *current = ht->buckets[idx]->head;
	if (current == NULL) {
		return 0;
	}
    for (int i = 0; i < ht->buckets[idx]->size; ++i) {
        info *data = (info *)current->data;
        if (ht->compare_function(key, data->key)) {
            return 1;
		}
        current = current->next;
    }
	return 0;
}


void *ht_get(hashtable_t *ht, void *key)
{
	unsigned int idx = ht->hash_function(key) % ht->hmax;
    ll_node_t *current = ht->buckets[idx]->head;
    for (int i = 0; i < ht->buckets[idx]->size; ++i) {
        info *data = (info *)current->data;
        if (!ht->compare_function(key, data->key))
            return data->value;
        current = current->next;
    }
	return NULL;
}

/*
 * Atentie! Desi cheia este trimisa ca un void pointer (deoarece nu se impune
 * tipul ei), in momentul in care se creeaza o noua intrare in hashtable (in
 * cazul in care cheia nu se gaseste deja in ht), trebuie creata o copie a
 * valorii la care pointeaza key si adresa acestei copii trebuie salvata in
 * structura info asociata intrarii din ht. Pentru a sti cati octeti trebuie
 * alocati si copiati, folositi parametrul key_size.
 *
 * Motivatie:
 * Este nevoie sa copiem valoarea la care pointeaza key deoarece dupa un apel
 * put(ht, key_actual, value_actual), valoarea la care pointeaza key_actual
 * poate fi alterata (de ex: *key_actual++). Daca am folosi direct adresa
 * pointerului key_actual, practic s-ar modifica din afara hashtable-ului cheia
 * unei intrari din hashtable. Nu ne dorim acest lucru, fiindca exista riscul sa
 * ajungem in situatia in care nu mai stim la ce cheie este inregistrata o
 * anumita valoare.
 */
void ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	unsigned int idx = ht->hash_function(key) % ht->hmax;
    if (ht_has_key(ht, key)) {
        ll_node_t *current = ht->buckets[idx]->head;
        for (int i = 0; i < ht->buckets[idx]->size; ++i) {
            if (!ht->compare_function(((info *)current->data)->key, key)) {
				free(((info *)current->data)->value);
				((info *)current->data)->value = malloc(value_size);
				DIE(!((info *)current->data)->value, "rusine");
				memcpy(((info *)current->data)->value, value, value_size);
				return;
			}
			current = current->next;
        }
    } else {
		info new;
        new.key = malloc(key_size);
        new.value = malloc(value_size);
		memcpy(new.key, key, key_size);
		memcpy(new.value, value, value_size);
        ll_add_nth_node(ht->buckets[idx], 0, &new, sizeof(info));
        ht->size++;
    }
}

void ht_put_database(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	unsigned int idx = ht->hash_function(key) % ht->hmax;
    if (ht_has_key(ht, key)) {
        ll_node_t *current = ht->buckets[idx]->head;
        for (int i = 0; i < ht->buckets[idx]->size; ++i) {
            if (!ht->compare_function(((info *)current->data)->key, key)) {
				free(((info *)current->data)->value);
				((info *)current->data)->value = malloc(value_size);
				DIE(!((info *)current->data)->value, "rusine");
				memcpy(((info *)current->data)->value, value, value_size);
				return;
			}
			current = current->next;
        }
    } else {
		info new;
        new.key = malloc(key_size);
        new.value = malloc(value_size);
		memcpy(new.key, key, key_size);
		memcpy(new.value, value, value_size);
        ll_add_nth_node(ht->buckets[idx], 0, &new, sizeof(info));
        ht->size++;
    }
}

/*
 * Procedura care elimina din hashtable intrarea asociata cheii key.
 * Atentie! Trebuie avuta grija la eliberarea intregii memorii folosite pentru o
 * intrare din hashtable (adica memoria pentru copia lui key --vezi observatia
 * de la procedura put--, pentru structura info si pentru structura Node din
 * lista inlantuita).
 */
void ht_remove_entry(hashtable_t *ht, void *key)
{
	int i;
	unsigned int idx = ht->hash_function(key) % ht->hmax;
	ll_node_t *current = ht->buckets[idx]->head;
	for (i = 0; i < ht->buckets[idx]->size; ++i) {
        if (!ht->compare_function(((info *)current->data)->key, key))
			break;
        current = current->next;
    }
	if (current != NULL) {
		ll_node_t *removed = ll_remove_nth_node(ht->buckets[idx], i);
        key_val_free_function(removed->data);
		free(removed->data);
        free(removed);
        ht->size--;
    }
}

void ll_free(linked_list_t** pp_list)
{
    ll_node_t* currNode;

    if (!pp_list || !*pp_list) {
        return;
    }

    while (ll_get_size(*pp_list) > 0) {
        currNode = ll_remove_nth_node(*pp_list, 0);
        key_val_free_function(currNode->data);
        currNode->data = NULL;
        free(currNode);
        currNode = NULL;
    }

    free(*pp_list);
    *pp_list = NULL;
}


/*
 * Procedura care elibereaza memoria folosita de toate intrarile din hashtable,
 * dupa care elibereaza si memoria folosita pentru a stoca structura hashtable.
 */
void ht_free(hashtable_t *ht)
{
	if (ht == NULL)
		return;

	for (int i = 0; i < ht->hmax; ++i)
		ll_free(&(ht->buckets[i]));
	free(ht->buckets);
	free(ht);
}

unsigned int ht_get_size(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->size;
}

unsigned int ht_get_hmax(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->hmax;
}
