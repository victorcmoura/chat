#ifndef STRING_MAP_H
#define STRING_MAP_H
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>
#include <stdint.h>

sem_t map_semaphore;
char*** string_map;
int max_value;

void init_map(int n_keys, int n_values){
    max_value = n_keys;
    sem_init(&map_semaphore, 0, 1);
    int i;
    string_map = (char***) malloc(n_keys * sizeof(char***));

    for(i = 0; i < n_keys; i++){
        char* c = (char*) malloc(sizeof(char));
        *c = 1;
        string_map[i] = (char**) malloc(n_values * sizeof(char**));
        string_map[i][0] = c;
    }
}

unsigned long hash(unsigned char *str){
    unsigned long hash = 5381;
    int c;

    while (c = *str++){
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
        if(c == 0){
            break;
        }
    }

    return hash % max_value;
}

void map_insert(char* key, char* value){
    int pos = hash(key);
    // printf("%d\n", pos);
    char* permanent_value = (char*) malloc(1000);

    strcpy(permanent_value, value);

    sem_wait(&map_semaphore);
    // printf("%d\n", *string_map[pos][0]);
    char insert_pos = *string_map[pos][0];
    string_map[pos][insert_pos] = permanent_value;
    *string_map[pos][0] += 1;
    sem_post(&map_semaphore);
}

char** map_get(char* key){
    int pos = hash(key);
    char** ans; 
    sem_wait(&map_semaphore);
    ans = string_map[pos];
    sem_post(&map_semaphore);
    return ans;    
}

int len(char* key){
    char** result = map_get(key);
    return *result[0];
}

// TODO: Destroy map

// int destroy_stack(){
//     free(stack);
//     sem_destroy(&map_semaphore);
//     return 0;
// }

#endif