#ifndef MESSENGER_UTILS_H
#define MESSENGER_UTILS_H

#define MAX_MESSAGE_SIZE 522
#define MAX_QUEUE_SIZE 200
#define MAX_USERNAME_SIZE 10
#define MAX_QUEUE_NAME_SIZE 16

typedef struct {
    char* msg_buffer;
    char* to;
} message_struct;

typedef struct{
    char queue_name[MAX_QUEUE_NAME_SIZE];
} queue_struct;

int is_broadcast(char* msg){
    int index = 0;
    while(msg[index] != ':'){
        index++;
    }
    index++;
    if(msg[index] == 'a' && msg[index+1] == 'l' && msg[index+2] == 'l'){
        return 1;
    }
    return 0;
}

void unformat_from_message_protocol(char* decoded, char* message){
    char aux = 0;
    int d = 0, m = 0, unwanted_chars = -1;

    do{
        if(unwanted_chars < 0){
            aux = message[m++];
            if(aux == ':'){
                unwanted_chars *= -1; 
            }
            decoded[d++] = aux;
        }else{
            aux = message[m++];
            if(aux == ':'){
                unwanted_chars *= -1;
                decoded[d++] = ' ';
            }
        }
    }while(aux != 0);
}

void format_into_message_protocol(char* final_message, char* name, char* msg, char* queue_name){
    char marker[2] = ":";
    char to[MAX_QUEUE_NAME_SIZE];
    char message[MAX_MESSAGE_SIZE];

    strcpy(to, name);
    strcpy(message, msg);

    strcat(final_message, &queue_name[6]);
    strcat(final_message, marker);
    strcat(final_message, &to[6]);
    strcat(final_message, marker);
    strcat(final_message, message);
}

void format_into_join_request_protocol(char* final_message, char* queue_name){
    char marker[2] = ":";
    char message[10] = "JOIN";

    strcpy(final_message, &queue_name[6]);
    strcat(final_message, marker);
    strcat(final_message, message);
}

void format_into_broadcast_protocol(char* final_message, char* msg, char* queue_name){
    char marker[2] = ":";
    char to[MAX_QUEUE_NAME_SIZE];
    char message[MAX_MESSAGE_SIZE];

    strcpy(to, "/chat-all");
    strcpy(message, msg);

    strcat(final_message, &queue_name[6]);
    strcat(final_message, marker);
    strcat(final_message, &to[6]);
    strcat(final_message, marker);
    strcat(final_message, message);
}

char* get_sender_queue_name_from_unformatted_message(char* message){
    char chat_label[10] = "/chat-";
    char* sender_name = (char*) malloc(MAX_QUEUE_NAME_SIZE);
    sender_name[0] = 0;

    strcat(sender_name, chat_label);

    int i = 0;
    while(message[i] != ':'){
        sender_name[i+strlen(chat_label)] = message[i];
        i++;
    }
    sender_name[i+strlen(chat_label)] = 0;
    return sender_name;
}

int is_channel_name(char* name){
    char channel[] = "/canal";

    int i;
    for(i = 0; i < strlen(channel); i++){
        if(channel[i] != name[i]){
            return 0;
        }
    }

    return 1;
}

void remove_line_breaks(char* buffer, int bufferlen){
    int i;
    for(i = 0; i < bufferlen; i++){
        if(buffer[i] == '\n'){
            buffer[i] = '\0';
        }
    }
}

char** get_online_queues(){
    char** options = (char**) malloc(100);

    FILE* fp;
    fp = popen("ls /dev/mqueue", "r");

    if (fp == NULL) {
        printf("Something wrong happened. Try again later.\n" );
        exit(1);
    }

    int index = 0;
    while(1){
        char* buffer = (char*) malloc(MAX_QUEUE_NAME_SIZE);
        buffer[0] = '/';
        if(fgets(buffer+1, MAX_QUEUE_NAME_SIZE, fp) != NULL){
            remove_line_breaks(buffer, MAX_QUEUE_NAME_SIZE);
            options[index++] = buffer;
        }else{
            buffer[0] = 0;
            options[index] = buffer;
            break;
        }
    }

    return options;
}

#endif