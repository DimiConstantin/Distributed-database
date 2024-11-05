/*
 * Copyright (c) 2024, <>
 */


#include <stdio.h>
#include "server.h"
#include "lru_cache.h"
#include "utils.h"

static response
*server_edit_document(server *s, char *doc_name, char *doc_content) {
    // aloc memorie pentru response
    response *res = malloc(sizeof(response));
    DIE(!res, "pfaaa");
    res->server_id = s->id;
    res->server_log = malloc(MAX_LOG_LENGTH);
    DIE(!res->server_log, "pfaaa");
    res->server_response = malloc(MAX_RESPONSE_LENGTH);
    DIE(!res->server_response, "tsaaaa");
    void **evicted = malloc(sizeof(void *));
    DIE(!evicted, "pfaaa");
    *evicted = malloc(DOC_NAME_LENGTH);
    DIE(!*evicted, "pfaaa");

    if (lru_cache_get(s->cache, doc_name))  {
        // documentul exista in cache
        sprintf(res->server_response, MSG_B, doc_name);
        sprintf(res->server_log, LOG_HIT, doc_name);
        // actualizez continutul documentului in hashtable
        ht_put(s->database, doc_name, DOC_NAME_LENGTH, doc_content, DOC_CONTENT_LENGTH);
        lru_cache_put(s->cache, doc_name, doc_content, evicted);
        free(*evicted);
        free(evicted);
        return res;
    } else {
        if (ht_has_key(s->database, doc_name)) {
            // documentul exista in database, dar nu in cache
            sprintf(res->server_response, MSG_B, doc_name);
            ht_put(s->database, doc_name, DOC_NAME_LENGTH, doc_content, DOC_CONTENT_LENGTH);
            lru_cache_put(s->cache, doc_name, doc_content, evicted);
            if (lru_cache_is_full(s->cache)) {
                sprintf(res->server_log, LOG_EVICT, doc_name, (char *)*evicted);
                free(*evicted);
                free(evicted);
                return res;
            } else {
                sprintf(res->server_log, LOG_MISS, doc_name);
                free(*evicted);
                free(evicted);
                return res;
            }
        } else {
            // documentul nu exista nici in cache, nici in database
            // deci il creez si il adaug in ambele
            sprintf(res->server_response, MSG_C, doc_name);
            ht_put(s->database, doc_name, DOC_NAME_LENGTH, doc_content, DOC_CONTENT_LENGTH);
            if (!lru_cache_is_full(s->cache)) {
                lru_cache_put(s->cache, doc_name, doc_content, evicted);
                sprintf(res->server_log, LOG_MISS, doc_name);
                free(*evicted);
                free(evicted);
                return res;
            } else {
                lru_cache_put(s->cache, doc_name, doc_content, evicted);
                sprintf(res->server_log, LOG_EVICT, doc_name, (char *)*evicted);
                free(*evicted);
                free(evicted);
                return res;
            }
        }
    }
}

static response
*server_get_document(server *s, char *doc_name) {
    // aloc memorie pentru response
    response *res = malloc(sizeof(response));
    DIE(!res, "pfaaa");
    res->server_id = s->id;
    res->server_log = malloc(MAX_LOG_LENGTH);
    DIE(!res->server_log, "pfaaa");
    res->server_response = malloc(MAX_RESPONSE_LENGTH);
    DIE(!res->server_response, "tsaaaa");
    void **evicted = malloc(sizeof(void *));
    DIE(!evicted, "pfaaa");
    *evicted = malloc(DOC_NAME_LENGTH);
    DIE(!*evicted, "pfaaa");
    if (lru_cache_get(s->cache, doc_name)) {
        // documentul exista in cache
        sprintf(res->server_log, LOG_HIT, doc_name);
        strcpy(res->server_response, lru_cache_get(s->cache, doc_name));

        free(*evicted);
        free(evicted);
        return res;
    } else {
        if (ht_get(s->database, doc_name)) {
            // documentul exista in database, dar nu in cache
            char *content = (char *)ht_get(s->database, doc_name);
            strcpy(res->server_response, content);
            lru_cache_put(s->cache, doc_name, content, evicted);
        } else {
            free(res->server_response);
            res->server_response = NULL;
            sprintf(res->server_log, LOG_FAULT, doc_name);
            free(*evicted);
            free(evicted);
            return res;
        }
    }

    if (lru_cache_is_full(s->cache))
        sprintf(res->server_log, LOG_EVICT, doc_name, (char *)*evicted);
    else
        sprintf(res->server_log, LOG_MISS, doc_name);
    // actualizez cache-ul
    char *to_write = (char *)ht_get(s->database, doc_name);
    lru_cache_put(s->cache, doc_name, to_write, evicted);
    free(*evicted);
    free(evicted);
    return res;
}

server *init_server(unsigned int cache_size) {
    // aloc memorie pentru server
    server *s = malloc(sizeof(server));
    DIE(!s, "mori");
    s->cache = init_lru_cache(cache_size);
    s->tasks = q_create(sizeof(request), TASK_QUEUE_SIZE);
    s->database = ht_create(MAX_LOG_LENGTH, hash_string, compare_function_strings, key_val_free_function);
    return s;
}

response *server_handle_request(server *s, request *req) {
    if (req->type == EDIT_DOCUMENT) {
        // se executa comanda de editare a documentului
        response *res = malloc(sizeof(*res));
        DIE(!res, "ce are vere");
        res->server_id = s->id;
        res->server_log = malloc(MAX_LOG_LENGTH);
        DIE(!res->server_log, "ce are vere si asta");
        res->server_response = malloc(MAX_RESPONSE_LENGTH);
        DIE(!res->server_response, "am innebunit");

        // se creeaza o copie a request-ului pentru a fi adaugat in coada de task-uri
        request *copy = malloc(sizeof(*copy));
        DIE(!copy, "CEAUSESCULEEEEEE");
        copy->doc_content = malloc(DOC_CONTENT_LENGTH);
        DIE(!copy->doc_content, "OOOOOOOO");
        copy->doc_name = malloc(DOC_NAME_LENGTH);
        DIE(!copy->doc_name, "forta steaua");
        copy->type = EDIT_DOCUMENT;

        memcpy(copy->doc_name, req->doc_name, DOC_NAME_LENGTH);
        memcpy(copy->doc_content, req->doc_content, DOC_CONTENT_LENGTH);
        q_enqueue(s->tasks, copy);

        sprintf(res->server_response, MSG_A, EDIT_REQUEST, copy->doc_name);
        sprintf(res->server_log, LOG_LAZY_EXEC, s->tasks->size);
        free(copy);

        return res;
    } else if (req->type == GET_DOCUMENT) {
        // se executa comanda de obtinere a documentului
        // comanda GET declanseaza executia tuturor task-urilor din coada
        while (!q_is_empty(s->tasks)) {
            request *command = (request *)q_front(s->tasks);
            response *out = server_edit_document(s, command->doc_name, command->doc_content);
            PRINT_RESPONSE(out);
            free(command->doc_name);
            free(command->doc_content);
            q_dequeue(s->tasks);
        }
        return server_get_document(s, req->doc_name);
    }
}

void free_server(server **s) {
    if (!s || !(*s))
        return;
    free_lru_cache(&((*s)->cache));
    if (!q_is_empty((*s)->tasks)) {
        // daca coada de task-uri nu este goala
        // eliberez memoria alocata pentru fiecare task
        while (!q_is_empty((*s)->tasks)) {
            request *command = (request *)q_front((*s)->tasks);
            free(command->doc_name);
            free(command->doc_content);
            q_dequeue((*s)->tasks);
        }
    }
    q_free((*s)->tasks);
    free((*s)->tasks);
    ht_free(((*s)->database));
    free(*s);
    *s = NULL;
}
