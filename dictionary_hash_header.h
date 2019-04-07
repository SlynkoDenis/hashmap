#pragma once

struct hashmap_t;

//------------------------------------------------------------------------------------------------------

struct hashmap_t *hashmap_create ();

void hashmap_add (struct hashmap_t *poem_map, char *str);

void hashmap_destroy (struct hashmap_t *poem_map);

unsigned hashmap_find (struct hashmap_t *poem_map, char *str);
