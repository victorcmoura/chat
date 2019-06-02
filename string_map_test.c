#include <stdio.h>
#include "string_map.h"

int main(){
    char s1[10] = "Ola";
    char s2[10] = "Oie";
    char s3[10] = "Mais1";
    char s4[10] = "Nossa";
    char s5[10] = "Teste";
    char s6[10] = "Vai";


    char name1[20] = "victor";
    char name2[20] = "moura";
    char name3[20] = "victorcmoura";
    // Init map

    init_map(10000, 100);
    map_insert(name1, s1);
    map_insert(name2, s2);
    map_insert(name3, s3);
    map_insert(name3, s4);

    char** messages;

    messages = map_get(name1);
    int i;
    for(i = 1; i < (int) messages[0]; i++){
        printf("%s\n", messages[i]);
    }
    printf("\n");
    messages = map_get(name2);
    for(i = 1; i < (int) messages[0]; i++){
        printf("%s\n", messages[i]);
    }
    printf("\n");
    messages = map_get(name3);
    for(i = 1; i < (int) messages[0]; i++){
        printf("%s\n", messages[i]);
    }

    return 0;
}