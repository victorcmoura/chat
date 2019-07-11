#ifndef GUI_UTILS_H
#define GUI_UTILS_H
#include "messenger_utils.h"
#include "channel_manager.h"

void print_conversation(char* recipient_queue_name){
    // system("clear");
    char** messages = map_get(recipient_queue_name);

    printf("=====> %s Conversation:\n\n", recipient_queue_name);
    
    int i;
    for(i = 1; i < len(recipient_queue_name); i++){
        printf("%s\n", messages[i]);
    }

    printf("\nPress i to enter insert mode or esc to return to chat list\n");
}

void print_broadcast_gui(){
    // system("clear");

    printf("=====> Sending broadcast:\n\n");

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

char* choose_queue(char* my_queue_name, char* created_queue_string){
    char** options = get_online_queues();

    printf("Online queues (pick one):\n");

    int index = 0;
    for(index = 0; strlen(options[index]) > 0; index++){
        printf("\t%d - [%s]\n", index+1, options[index]);
    }

    printf("\t%d - Broadcast to all\n", index+1);
    printf("\t%d - Create chat room\n", index+2);
    printf("\t%d - Back to main menu\n", index+3);

    int option;
    do{
        printf(">> ");
        scanf("%d", &option);
    }while(option < 1 || option > index+3);

    int i;
    for(i = 0; strlen(options[i]) > 0; i++){
        if(i+1 != option){
            free(options[i]);
        }
    }

    if(option == index+1){
        char* broadcast = (char*) malloc(MAX_QUEUE_NAME_SIZE);
        strcpy(broadcast, "broad_to_all");
        return broadcast;
    }else if(option == index+2){
        char* room_name = (char*) malloc(MAX_QUEUE_NAME_SIZE);
        char tmp[MAX_QUEUE_NAME_SIZE];
        char user_input[MAX_QUEUE_NAME_SIZE] = "";
        char prefix[10] = "/canal-";
        strcpy(room_name, prefix);
        int never_asked = 1;
        do{
            if(never_asked){
                // system("clear");
                printf("Insert the name of your queue: ");
                never_asked = 0;
            }else{
                // system("clear");
                printf("The queue could not be created, try another name: ");
            }
            scanf("%s", user_input);
            strcpy(tmp, prefix);
            strcat(tmp, user_input);
        }while(!can_create(tmp));
        create_channel(tmp, my_queue_name);
        strcpy(created_queue_string, tmp);
        return NULL;
    }else if(option == index+3){
        return NULL;
    }

    return options[option-1];
}

#endif