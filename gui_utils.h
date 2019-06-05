#ifndef GUI_UTILS_H
#define GUI_UTILS_H

void print_conversation(char* recipient_queue_name){
    system("clear");
    char** messages = map_get(recipient_queue_name);

    printf("=====> %s Conversation:\n\n", recipient_queue_name);
    
    int i;
    for(i = 1; i < len(recipient_queue_name); i++){
        printf("%s\n", messages[i]);
    }

    printf("\nPress i to enter insert mode or esc to return to chat list\n");
}

void clear_stdin(void){
    int c;
    do{
        c = getchar();
    }while (c != '\n' && c != EOF);
}

void print_menu_options(){
    printf("Chat (choose an option):\n");
    printf("\t1 - Enter chat\n");
    printf("\t2 - Exit\n");
}

char* choose_queue(){
    char* options[100] = {0};
    
    FILE* fp;
    fp = popen("ls /dev/mqueue", "r");

    if (fp == NULL) {
        printf("Something wrong happened. Try again later.\n" );
        exit(1);
    }

    printf("Online queues (pick one):\n");

    int index = 1;
    while(1){
        char* buffer = (char*) malloc(MAX_QUEUE_NAME_SIZE);
        buffer[0] = '/';
        if(fgets(buffer+1, MAX_QUEUE_NAME_SIZE, fp) != NULL){
            remove_line_breaks(buffer, MAX_QUEUE_NAME_SIZE);
            printf("\t%d - [%s]\n", index, buffer);
            options[index++] = buffer;
        }else{
            index--;
            free(buffer);
            break;
        }
    }

    printf("\t%d - Exit\n", index+1);

    int option;
    do{
        printf(">> ");
        scanf("%d", &option);
    }while(option < 1 || option > index+1);

    int i;
    for(i = 1; i< index; i++){
        if(i != option){
            free(options[i]);
        }
    }

    if(option == index+1){
        return NULL;    
    }

    return options[option];
}

#endif