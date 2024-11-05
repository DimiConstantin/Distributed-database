/*
 * Copyright (c) 2024, <Constantin Dimtrie>
 */

#include <stdio.h>
#include <string.h>
#include "lru_cache.h"
#include "utils.h"

// functie care elibereaza un nod din cache
void free_cache_node(dll_node_t *node) {
    doc_type *data = (doc_type *)node->data;
    free(data->key);
    free(data->content);
    free(data);
    free(node);
}

lru_cache *init_lru_cache(unsigned int cache_capacity) {
    lru_cache *cache = malloc(sizeof(lru_cache));
    DIE(!cache, "mori");
    cache->capacity = cache_capacity;
    cache->ht = ht_create(cache_capacity, hash_string, compare_function_strings, key_val_free_function);
    cache->order = dll_create(sizeof(doc_type));
    return cache;
}

bool lru_cache_is_full(lru_cache *cache) {
    if (!cache)
        return false;
    if (cache->order->size == cache->capacity)
        return true;
    return false;
}

void free_lru_cache(lru_cache **cache) {
    if (!cache)
        return;
    ht_free((*cache)->ht);
    dll_free(&((*cache)->order));
    free(*cache);
    *cache = NULL;
}

// functie care muta un nod la finalul listei
void move_to_end(lru_cache *cache, dll_node_t *node) {
    if (!cache || !node)
        return;
    if (node == cache->order->tail) {
        return;
    }
    if (node == cache->order->head && cache->order->size != 1) {
        cache->order->head = node->next;
        cache->order->head->prev = NULL;
        cache->order->tail->next = node;
        node->prev = cache->order->tail;
        cache->order->tail = node;
        node->next = NULL;
    } else {
        node->prev->next = node->next;
        node->next->prev = node->prev;
        cache->order->tail->next = node;
        node->prev = cache->order->tail;
        cache->order->tail = node;
        node->next = NULL;
    }
}

bool lru_cache_put(lru_cache *cache, void *key, void *value,
                   void **evicted_key) {
    if (ht_has_key(cache->ht, key)) {
        //  daca documentul exista in cache, actualizez continutul si mut nodul la finalul listei
        dll_node_t **node_ptr = ht_get(cache->ht, key);
        dll_node_t *node = *node_ptr;
        doc_type *data = (doc_type *)node->data;
        memcpy(data->content, value, DOC_CONTENT_LENGTH);
        move_to_end(cache, node);
        ht_put(cache->ht, key, DOC_NAME_LENGTH, &cache->order->tail, sizeof(dll_node_t *));
        return false;
    }

    if (lru_cache_is_full(cache)) {
        //  daca cache-ul este plin, elimin primul nod si adaug noul nod la finalul listei
        if (cache->order->size == 1) {
            dll_node_t *node = cache->order->head;
            memcpy(*evicted_key, ((doc_type *)node->data)->key, DOC_NAME_LENGTH);
            ht_remove_entry(cache->ht, ((doc_type *)node->data)->key);
            free_cache_node(node);
            cache->order->size--;
            cache->order->head = NULL;
            cache->order->tail = NULL;
        } else {
            dll_node_t *node = cache->order->head;
            memcpy(*evicted_key, ((doc_type *)node->data)->key, DOC_NAME_LENGTH);
            ht_remove_entry(cache->ht, ((doc_type *)node->data)->key);
            cache->order->head = node->next;
            cache->order->head->prev = NULL;
            free_cache_node(node);
            cache->order->size--;
        }
    }
    // adaug noul nod la finalul listei si in hashtable
    doc_type new_data;
    new_data.key = malloc(DOC_NAME_LENGTH);
    DIE(!new_data.key, "hai capriorii");
    new_data.content = malloc(DOC_CONTENT_LENGTH);
    DIE(!new_data.content, "a furat rapid datele");
    memcpy(new_data.key, key, DOC_NAME_LENGTH);
    memcpy(new_data.content, value, DOC_CONTENT_LENGTH);
    dll_add_nth_node(cache->order, cache->order->size , &new_data);
    ht_put(cache->ht, key, DOC_NAME_LENGTH, &cache->order->tail, sizeof(dll_node_t *));
    return true;
}


void *lru_cache_get(lru_cache *cache, void *key) {
    if (ht_has_key(cache->ht, key)) {
        // daca documentul exista in cache
        if (!ht_get(cache->ht, key))
            return NULL;
        dll_node_t **node_ptr = ht_get(cache->ht, key);
        doc_type *data = (doc_type *)(*node_ptr)->data;
        dll_node_t *node = *node_ptr;
        move_to_end(cache, node);  //  mut nodul la finalul listei
        return data->content;  //  returnez continutul
    }
    return NULL;
}

void lru_cache_remove(lru_cache *cache, void *key) {
    if (ht_has_key(cache->ht, key)) {
        //  daca documentul exista in cache, tratez edge case-urile
        dll_node_t **node_ptr = ht_get(cache->ht, key);
        doc_type *data = (doc_type *)(*node_ptr)->data;
        dll_node_t *node = *node_ptr;
        if (cache->order->size == 1) {
            cache->order->head = NULL;
            cache->order->tail = NULL;
            cache->order->size--;
            free_cache_node(node);
            ht_remove_entry(cache->ht, key);
            return;
        }
        if (cache->order->size == 2) {
            if (node == cache->order->head) {
                cache->order->head = cache->order->tail;
                cache->order->tail->prev = NULL;
                cache->order->size--;
                free_cache_node(node);
                ht_remove_entry(cache->ht, key);
                return;
            }
            cache->order->tail = cache->order->head;
            cache->order->head->next = NULL;
            cache->order->size--;
            free_cache_node(node);
            ht_remove_entry(cache->ht, key);
            return;
        }
        if (node == cache->order->head) {
            cache->order->head = node->next;
            cache->order->head->prev = NULL;
            cache->order->size--;
            free_cache_node(node);
            ht_remove_entry(cache->ht, key);
            return;
        }
        if (node == cache->order->tail) {
            cache->order->tail = node->prev;
            cache->order->tail->next = NULL;
            cache->order->size--;
            free_cache_node(node);
            ht_remove_entry(cache->ht, key);
            return;
        }
        node->prev->next = node->next;
        node->next->prev = node->prev;
        cache->order->size--;
        free_cache_node(node);
        ht_remove_entry(cache->ht, key);
    }
}
