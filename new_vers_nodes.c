#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "dictionary_hash_header.h"

const float load_factor = 0.72f;        // Filling percentage of array, on reaching which
                                         // map should be rebuilded
                                         // with new power of the hash-function
const float multiplier = 1.5f;          // Multiplier for a new power of the hash-function

enum
{
    defolt_hash_size = 72497,
    Q = 1002451u,
    R = 1034951u
};

struct hashnode_t
{
    unsigned counter;
    char *data;
    struct hashnode_t *next;
};

struct hashentry_t
{
    unsigned ncollisions;
    struct hashnode_t *collisions_top;
};

struct hashmap_t
{
    struct hashentry_t *entries;
    struct hashnode_t *top;
    unsigned size;
    unsigned load_size;
};

struct hashmap_t *hashmap_create ();

void hashmap_add (struct hashmap_t *poem_map, char *str);

void hashmap_destroy (struct hashmap_t *poem_map);

unsigned hashmap_find (struct hashmap_t *poem_map, char *str);

unsigned get_hash (char *pat, unsigned m);

struct hashnode_t *node_add (struct hashnode_t *cur, char *wrd);

void hashmap_rebuild (struct hashmap_t *poem_map);

struct hashmap_t *hashmap_create ()                                  // Function for creation
                                                                     // of the map
{
    unsigned m = defolt_hash_size;

    struct hashmap_t *poem_map = (struct hashmap_t *) calloc (1, sizeof (struct hashmap_t));
    assert (poem_map);

    poem_map->size = m;

    poem_map->load_size = 0;

    poem_map->top = (struct hashnode_t *) calloc (1, sizeof(struct hashnode_t));
    assert (poem_map->top);

    poem_map->top->counter = 0;
    poem_map->top->data = NULL;
    poem_map->top->next = NULL;

    poem_map->entries = (struct hashentry_t *) calloc (m, sizeof (struct hashentry_t));
    assert (poem_map->entries);

    for (int i = 0; i < m; i++)
    {
        poem_map->entries[i].ncollisions = 0;
        poem_map->entries[i].collisions_top = NULL;
    }

    return poem_map;
};

//------------------------------------------------------------------------------------------------------

void hashmap_add (struct hashmap_t *poem_map, char *str)        // Function for adding a node in the map
{
    assert (str != NULL);
    assert (poem_map != NULL);

    unsigned key = get_hash (str, poem_map->size);

    if (poem_map->load_size == 0)
    {
        poem_map->load_size++;

        poem_map->entries[key].ncollisions = 1;
        poem_map->entries[key].collisions_top = (struct hashnode_t *) calloc (1, sizeof(struct hashnode_t));
        assert (poem_map->entries[key].collisions_top);

        poem_map->entries[key].collisions_top->counter = 1;
        poem_map->entries[key].collisions_top->next = NULL;
        poem_map->entries[key].collisions_top->data = (char *) calloc (strlen(str), sizeof(char));
        assert (poem_map->entries[key].collisions_top->data);

        strcpy (poem_map->entries[key].collisions_top->data, str);

        poem_map->top->next = poem_map->entries[key].collisions_top;

        return ;
    }

    if (poem_map->load_size >= (unsigned)(load_factor * poem_map->size))
    {
        hashmap_rebuild (poem_map);
        key = get_hash (str, poem_map->size);
    }

    if (poem_map->entries[key].ncollisions == 0)
    {
        poem_map->load_size++;

        poem_map->entries[key].ncollisions = 1;

        poem_map->entries[key].collisions_top = node_add (poem_map->top, str)->next;

        return ;
    }

    struct hashnode_t *ktemp = poem_map->entries[key].collisions_top;
    for (int i = 0; i < poem_map->entries[key].ncollisions; i++)
    {
        if (strcmp (str, ktemp->data) == 0)
        {
            ktemp->counter++;

            return ;
        }
        ktemp = ktemp->next;
    }

    poem_map->load_size++;

    poem_map->entries[key].ncollisions++;
    poem_map->entries[key].collisions_top = node_add (poem_map->entries[key].collisions_top, str);
}

//------------------------------------------------------------------------------------------------------

void hashmap_destroy (struct hashmap_t *poem_map)                       // Function for destroying the map,
                                                                         // entries and freeing the dynamic
                                                                         // memory
{
    assert (poem_map != NULL);

    for (unsigned i = 0; i < poem_map->size; i++)
    {
        if (poem_map->entries[i].ncollisions == 0)
            continue;

        poem_map->entries[i].ncollisions = 0;
    }

    struct hashnode_t *temp = poem_map->top->next;
    struct hashnode_t *ntemp = temp->next;
    while (ntemp != NULL)
    {
        temp->counter = 0;
        free (temp->data);
        free (temp);

        temp = ntemp;
        ntemp = ntemp->next;
    }
    temp->counter = 0;
    free (temp->data);
    free (temp);

    free (poem_map->top);
    free (poem_map->entries);
    free (poem_map);
}

//------------------------------------------------------------------------------------------------------

unsigned hashmap_find (struct hashmap_t *poem_map, char *str)                      // Function for finding
                                                                                    // frequency of the
                                                                                    // given word in text
{
    assert (poem_map);
    assert (str);

    unsigned ans = get_hash (str, poem_map->size);

    struct hashnode_t *temp = poem_map->entries[ans].collisions_top;
    for (int i = 0; i < poem_map->entries[ans].ncollisions; i++)
    {
        if (strcmp (str, temp->data) == 0)
            return temp->counter;

        temp = temp->next;
    }

    return 0;
}

//------------------------------------------------------------------------------------------------------

unsigned get_hash (char *pat, unsigned m)                     // Function that calculates the hash-key for
                                                                // the given element
{
    unsigned p = 0;

    for (; *pat != '\0'; ++pat)
        p = ((p * R + *pat) % Q) % m;

    return p;
}

//------------------------------------------------------------------------------------------------------

struct hashnode_t *node_add (struct hashnode_t *cur, char *wrd)          // Function for adding a node in
                                                                          // the list of nodes
{
    assert (cur);
    assert (wrd);

    struct hashnode_t *newnode = (struct hashnode_t *) calloc (1, sizeof(struct hashnode_t));
    assert (newnode);

    newnode->counter = 1;

    newnode->data = (char *) calloc (strlen (wrd), sizeof(char));
    assert (newnode->data);

    strcpy (newnode->data, wrd);
    newnode->next = cur->next;
    cur->next = newnode;

    return cur;
}

//------------------------------------------------------------------------------------------------------

void hashmap_rebuild (struct hashmap_t *poem_map)               // Function for rebuilding map with new
                                                                 // power of the hash-function
{
    assert (poem_map);

    for (int i = 0; i < poem_map->size; i++)
    {
        if (poem_map->entries[i].ncollisions == 0)
            continue;

        poem_map->entries[i].ncollisions = 0;
        poem_map->entries[i].collisions_top = NULL;
    }
    free (poem_map->entries);

    poem_map->size = (unsigned)(poem_map->size * multiplier);

    poem_map->entries = (struct hashentry_t *) calloc (poem_map->size, sizeof(struct hashentry_t));
    assert (poem_map->entries);

    for (int i = 0; i < poem_map->size; i++)
    {
        poem_map->entries[i].ncollisions = 0;
        poem_map->entries[i].collisions_top = NULL;
    }

    struct hashnode_t *temp = poem_map->top->next;
    struct hashnode_t *ntemp = poem_map->top->next->next;
    while (ntemp != NULL)
    {
        unsigned hk = get_hash (temp->data, poem_map->size);

        if (poem_map->entries[hk].collisions_top == NULL)
        {
            poem_map->entries[hk].collisions_top = temp;
            poem_map->entries[hk].collisions_top->next = NULL;
        }
        else
        {
            temp->next = poem_map->entries[hk].collisions_top->next;
            poem_map->entries[hk].collisions_top->next = temp;
        }

        poem_map->entries[hk].ncollisions++;
        temp = ntemp;
        ntemp = ntemp->next;
    }

    if (poem_map->entries[get_hash (temp->data, poem_map->size)].collisions_top == NULL)
    {
        poem_map->entries[get_hash (temp->data, poem_map->size)].collisions_top = temp;
        poem_map->entries[get_hash (temp->data, poem_map->size)].collisions_top->next = NULL;
    }
    else
    {
        temp->next = poem_map->entries[get_hash (temp->data, poem_map->size)].collisions_top->next;
        poem_map->entries[get_hash (temp->data, poem_map->size)].collisions_top->next = temp;
    }

    poem_map->entries[get_hash (temp->data, poem_map->size)].ncollisions++;

    unsigned num = 0;
    while ((poem_map->entries[num].ncollisions == 0) && (num < poem_map->size))
        num++;

    poem_map->top->next = poem_map->entries[num].collisions_top;
    struct hashnode_t *buff = poem_map->entries[num].collisions_top;
    while (buff->next != NULL)
        buff = buff->next;
    for (int i = num + 1; i < poem_map->size; i++)
    {
        if (poem_map->entries[i].ncollisions == 0)
            continue;

        buff->next = poem_map->entries[i].collisions_top;
        while (buff->next != NULL)
            buff = buff->next;
    }

    assert (buff->next == NULL);
}
