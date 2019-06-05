#ifndef MESSENGER_UTILS_H
#define MESSENGER_UTILS_H

typedef struct {
    char* msg_buffer;
    char* to;
} message_struct;

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

void remove_line_breaks(char* buffer, int bufferlen){
    int i;
    for(i = 0; i < bufferlen; i++){
        if(buffer[i] == '\n'){
            buffer[i] = '\0';
        }
    }
}

#endif