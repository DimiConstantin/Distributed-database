// Copyright (c) [2024], <Constantin Dimitrie> <<dimi.constantin27@gmail.com>>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "lists.h"

//  --------------DOUBLY LINKED LIST---------------//


/*
 * Functie care trebuie apelata pentru alocarea si initializarea unei liste.
 * (Setare valori initiale pentru campurile specifice structurii LinkedList).
 */
doubly_linked_list_t*
dll_create(unsigned int data_size)
{
	doubly_linked_list_t *list = malloc(sizeof(*list));
    DIE(!list, "mai ancearca");
    list->data_size = data_size;
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

dll_node_t *create_node(unsigned int data_size, const void* data)
{
    dll_node_t *node = malloc(sizeof(*node));
    DIE(!node, "uite nodu nu e nodu");
    node->data = malloc(data_size);
    DIE(!node->data, "ce bagi vere acolo");
    node->next = NULL;
    node->prev = NULL;
    memcpy(node->data, data, data_size);
    return node;
}

/*
 * Functia intoarce un pointer la nodul de pe pozitia n din lista.
 * Pozitiile din lista sunt indexate incepand cu 0 (i.e. primul nod din lista se
 * afla pe pozitia n=0). Daca n >= nr_noduri, atunci se intoarce nodul de pe
 * pozitia rezultata daca am "cicla" (posibil de mai multe ori) pe lista si am
 * trece de la ultimul nod, inapoi la primul si am continua de acolo. Cum putem
 * afla pozitia dorita fara sa simulam intreaga parcurgere?
 * Atentie: n>=0 (nu trebuie tratat cazul in care n este negativ).
 */
dll_node_t *dll_get_nth_node(doubly_linked_list_t* list, int n)
{
	dll_node_t *current = list->head;
	if (n == 0)
        return list->head;

    if (n >= list->size && list->size)
        n = n % list->size;

    for (int i = 0; i < n; ++i)
        current = current->next;
    return current;
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se creeaza un nou nod care e
 * adaugat pe pozitia n a listei reprezentata de pointerul list. Pozitiile din
 * lista sunt indexate incepand cu 0 (i.e. primul nod din lista se afla pe
 * pozitia n=0). Cand indexam pozitiile nu "ciclam" pe lista circulara ca la
 * get, ci consideram nodurile in ordinea de la head la ultimul (adica acel nod
 * care pointeaza la head ca nod urmator in lista). Daca n >= nr_noduri, atunci
 * adaugam nodul nou la finalul listei.
 * Atentie: n>=0 (nu trebuie tratat cazul in care n este negativ).
 */
void
dll_add_nth_node(doubly_linked_list_t* list, int n, const void* new_data)
{
    dll_node_t *node = create_node(list->data_size, new_data);
    if (list->head == NULL) {
        list->head = node;
        list->tail = node;
        node->prev = NULL;
        node->next = NULL;
        list->size++;
        return;
    }
    if (n == 0) {
        node->prev = NULL;
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
        list->size++;
        return;
    }
    dll_node_t *current = list->head;
    if (n >= list->size) {
        if (list->size == 0) {
            list->head = node;
            list->tail = node;
            node->prev = NULL;
            node->next = NULL;
            list->size++;
            return;
        }
        for (int i = 0; i < list->size - 1; ++i) {
            current = current->next;
        }
        current->next = node;
        node->prev = current;
        node->next = NULL;
        list->tail = node;
        list->size++;
        return;
    }
    for (int i = 0; i < n - 1; ++i) {
        current = current->next;
    }
    node->next = current->next;
    node->prev = current;
    current->next = node;
    node->next->prev = node;
    list->size++;
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Functia intoarce un pointer spre acest nod
 * proaspat eliminat din lista. Daca n >= nr_noduri - 1, se elimina nodul de la
 * finalul listei. Este responsabilitatea apelantului sa elibereze memoria
 * acestui nod.
 * Atentie: n>=0 (nu trebuie tratat cazul in care n este negativ).
 */
dll_node_t*
dll_remove_nth_node(doubly_linked_list_t* list, int n)
{
dll_node_t *current = list->head, *deleted;
    if (n == 0) {
        list->head = current->next;
        list->head->prev = NULL;
        current->next = NULL;
        list->size--;
        return current;
    }
    if (n >= list->size - 1) {
        for (int i = 0; i < list->size - 1; ++i)
            current = current->next;
        current->next = NULL;
        list->tail = current;
        list->size--;
        return current;
    }
    for (int i = 0; i < n - 1; ++i)
        current = current->next;
    deleted = current->next;
    current->next = current->next->next;
    current->next->prev = current;
    list->size--;
    return deleted;
}

/*
 * Functia intoarce numarul de noduri din lista al carei pointer este trimis ca
 * parametru.
 */
unsigned int
dll_get_size(doubly_linked_list_t* list)
{
    return list->size;
}

/*
 * Procedura elibereaza memoria folosita de toate nodurile din lista, iar la
 * sfarsit, elibereaza memoria folosita de structura lista.
 */
void
dll_free(doubly_linked_list_t** pp_list)
{
dll_node_t *current = (*pp_list)->head, *urm;
    for (int i = 0; i < (*pp_list)->size; ++i) {
        urm = current->next;
        doc_type *data = (doc_type *)current->data;
        free(data->key);
        free(data->content);
        free(data);
        free(current);
        current = urm;
    }
    free(*pp_list);
    *pp_list = NULL;
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza int-uri. Functia afiseaza toate valorile int stocate in nodurile
 * din lista separate printr-un spatiu, incepand de la primul nod din lista.
 */
void
dll_print_int_list(doubly_linked_list_t* list)
{
dll_node_t *current = list->head;
    for (int i = 0; i < list->size; ++i) {
            printf("%d ", *((int *)current->data));
            current = current->next;
    }
    printf("\n");
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza string-uri. Functia afiseaza toate string-urile stocate in
 * nodurile din lista separate printr-un spatiu, incepand de la ULTIMUL nod din
 * lista si in ordine inversa.
 */
void
dll_print_string_list(doubly_linked_list_t* list)
{
    dll_node_t *current;
    current = dll_get_nth_node(list, list->size - 1);
    while (current!= NULL) {
            printf("%s ", (char *)current->data);
            current = current->prev;
    }
    printf("\n");
}

//  --------------LINKED LIST---------------//

linked_list_t *ll_create()
{
    linked_list_t* ll;

    ll = malloc(sizeof(*ll));

    ll->head = NULL;
    ll->size = 0;

    return ll;
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se creeaza un nou nod care e
 * adaugat pe pozitia n a listei reprezentata de pointerul list. Pozitiile din
 * lista sunt indexate incepand cu 0 (i.e. primul nod din lista se afla pe
 * pozitia n=0). Daca n >= nr_noduri, noul nod se adauga la finalul listei. Daca
 * n < 0, eroare.
 */
void ll_add_nth_node(linked_list_t* list, int n, const void* new_data, unsigned int data_size)
{
    ll_node_t *prev, *curr;
    ll_node_t* new_node;

    if (!list) {
        return;
    }

    /* n >= list->size inseamna adaugarea unui nou nod la finalul listei. */
    if (n > list->size) {
        n = list->size;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    new_node = malloc(sizeof(*new_node));
    new_node->data = malloc(data_size);
    memcpy(new_node->data, new_data, data_size);

    new_node->next = curr;
    if (prev == NULL) {
        /* Adica n == 0. */
        list->head = new_node;
    } else {
        prev->next = new_node;
    }

    list->size++;
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Daca n >= nr_noduri - 1, se elimina nodul de
 * la finalul listei. Daca n < 0, eroare. Functia intoarce un pointer spre acest
 * nod proaspat eliminat din lista. Este responsabilitatea apelantului sa
 * elibereze memoria acestui nod.
 */
ll_node_t *ll_remove_nth_node(linked_list_t* list, int n)
{
    ll_node_t *prev, *curr;

    if (!list || !list->head) {
        return NULL;
    }

    /* n >= list->size - 1 inseamna eliminarea nodului de la finalul listei. */
    if (n > list->size - 1) {
        n = list->size - 1;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    if (prev == NULL) {
        /* Adica n == 0. */
        list->head = curr->next;
    } else {
        prev->next = curr->next;
    }

    list->size--;

    return curr;
}

/*
 * Functia intoarce numarul de noduri din lista al carei pointer este trimis ca
 * parametru.
 */
int ll_get_size(linked_list_t* list)
{
     if (!list) {
        return -1;
    }

    return list->size;
}

/*
 * Procedura elibereaza memoria folosita de toate nodurile din lista, iar la
 * sfarsit, elibereaza memoria folosita de structura lista si actualizeaza la
 * NULL valoarea pointerului la care pointeaza argumentul (argumentul este un
 * pointer la un pointer).
 */
void ll_free(linked_list_t** pp_list)
{
    ll_node_t* currNode;

    if (!pp_list || !*pp_list) {
        return;
    }

    while (ll_get_size(*pp_list) > 0) {
        currNode = ll_remove_nth_node(*pp_list, 0);
        free(currNode->data);
        currNode->data = NULL;
        free(currNode);
        currNode = NULL;
    }

    free(*pp_list);
    *pp_list = NULL;
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza int-uri. Functia afiseaza toate valorile int stocate in nodurile
 * din lista inlantuita separate printr-un spatiu.
 */
void ll_print_int(linked_list_t* list)
{
    ll_node_t* curr;

    if (!list) {
        return;
    }

    curr = list->head;
    while (curr != NULL) {
        printf("%d ", *((int*)curr->data));
        curr = curr->next;
    }

    printf("\n");
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza string-uri. Functia afiseaza toate string-urile stocate in
 * nodurile din lista inlantuita, separate printr-un spatiu.
 */
void ll_print_string(linked_list_t* list)
{
    ll_node_t* curr;

    if (!list) {
        return;
    }

    curr = list->head;
    while (curr != NULL) {
        printf("%s ", (char*)curr->data);
        curr = curr->next;
    }

    printf("\n");
}
