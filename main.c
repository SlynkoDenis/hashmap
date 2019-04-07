#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "dictionary_hash_header.h"

enum
{
    MAX_AVERAGE_WORD_LENGTH = 250,
};

void sorte_from_punctuation (char *s);

unsigned get_hash (char *pat, unsigned m);

void analysis (FILE *buff, struct hashmap_t *poem_map);

int main(int argc, char **argv)
{
    assert (argc > 1);

    FILE *poem = fopen (argv[1], "rb");
    assert (poem);

    FILE *inf = fopen ("analysis.txt", "w");
    assert (inf);

    struct hashmap_t *poem_map = hashmap_create ();

    char *str = (char *) calloc (MAX_AVERAGE_WORD_LENGTH, sizeof(char));        // Dynamic memory for
                                                                                 // words from file
    while (fscanf (poem, "%s", str) != EOF)
    {
        assert (str[MAX_AVERAGE_WORD_LENGTH - 1] == '\0');

        sorte_from_punctuation (str);

        hashmap_add (poem_map, str);
    }

    analysis (inf, poem_map);

    for (int i = 2; i < argc; i++)                                              // Printing frequency
                                                                                // of given words in file
    {
        sorte_from_punctuation (argv[i]);

        unsigned frequency = hashmap_find (poem_map, argv[i]);
        printf ("%u   ", frequency);
    }

    hashmap_destroy (poem_map);
    free (str);
    fclose (poem);
    return 0;
}

//------------------------------------------------------------------------------------------------------

void sorte_from_punctuation (char *s)                  // Function for dropping out non-alphabet symbols
                                                        // from the given word
{
    assert (s);

    char *temp = s;
    while (*temp != '\0')
    {
        if (isalpha (*temp) == 0)
        {
            *temp = '\0';
            return ;
        }

        if (isupper (*temp) != 0)
            *temp = tolower (*temp);

        temp++;
    }
}

//------------------------------------------------------------------------------------------------------

void analysis (FILE *buff, struct hashmap_t *poem_map)             // Function for getting information
                                                                    // about the map in solitude text file
{
    assert (buff);
    assert (poem_map);

    fprintf (buff, "       Main information about current map:\nGlobal size - %u;\n", poem_map->size);
    fprintf (buff, "Counter of filled baskets - %u;\n", poem_map->load_size);
    fprintf (buff, "\n");

    fprintf (buff, "List of nodes which are used in map:\n");
    struct hashnode_t *temp = poem_map->top->next;
    while (temp != NULL)
    {
        fprintf (buff, "%s - %u;\n", temp->data, temp->counter);
        temp = temp->next;
    }
    fprintf (buff, "\n");

    fprintf (buff, "Map:\n");
    for (int i = 0; i < poem_map->size; i++)
    {
        if (poem_map->entries[i].ncollisions == 0)
            continue;

        fprintf (buff, "   Hash #%d with %u collisions:\n", i, poem_map->entries[i].ncollisions);
        temp = poem_map->entries[i].collisions_top;
        for (int j = 0; j < poem_map->entries[i].ncollisions; j++)
        {
            fprintf (buff, "%s - %u;\n", temp->data, temp->counter);
            temp = temp->next;
        }
    }
    fprintf (buff, "       End of current call of information-function.\n");
}
