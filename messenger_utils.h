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

void format_into_channel_protocol(char* final_message, char* name, char* msg, char* queue_name){
    char marker[2] = ":";
    char channel_marker[] = "#";
    char to[MAX_QUEUE_NAME_SIZE];
    char message[MAX_MESSAGE_SIZE];

    strcpy(to, name);
    strcpy(message, msg);

    strcat(final_message, &queue_name[6]);
    strcat(final_message, marker);
    strcat(final_message, channel_marker);
    strcat(final_message, &to[7]);
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

void format_into_channel_broadcast(char* final_message, char*msg, char* receiver_name){
    char message_content[MAX_MESSAGE_SIZE];
    char marker[] = ":";
    char name_opener[] = "<";
    char name_closer[] = ">";
    char channel_name[MAX_QUEUE_NAME_SIZE];
    char sender_name[MAX_QUEUE_NAME_SIZE];
    char spacing[] = " ";

    int i = 0;
    while(msg[i] != ':'){
        sender_name[i] = msg[i];
        i++;
    }
    sender_name[i++] = 0;

    int j = 0;
    while(msg[i] != ':'){
        channel_name[j++] = msg[i++];
    }
    channel_name[j] = 0;
    i++;

    j = 0;

    while(msg[i] != 0){
        message_content[j++] = msg[i++];
    }
    message_content[j] = 0;

    strcpy(final_message, channel_name);
    strcat(final_message, marker);
    strcat(final_message, &receiver_name[6]);
    strcat(final_message, marker);
    strcat(final_message, name_opener);
    strcat(final_message, sender_name);
    strcat(final_message, name_closer);
    strcat(final_message, spacing);
    strcat(final_message, message_content);
}

void unformat_from_channel_broadcast(char* queue_to_be_saved, char* content, char* msg){
    char marker[] = ":";
    char channel_prefix[] = "/canal-";
    char content_buffer[MAX_MESSAGE_SIZE];
    
    strcpy(queue_to_be_saved, channel_prefix);

    int i = 1;
    int j = 7;
    while(msg[i] != ':'){
        queue_to_be_saved[j++] = msg[i++];
    }
    queue_to_be_saved[j] = 0;
    
    i++;
    while(msg[i++] != ':'){
    }
    i++;

    j = 0;
    while(msg[i] != '>'){
        content[j++] = msg[i++];
    }
    i++;
    strcat(content, marker);


    int a = 0;
    while(msg[i] != 0){
        content_buffer[a++] = msg[i++];
    }
    content_buffer[a] = 0;

    strcat(content, content_buffer);
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

char* joiner(char* msg){
    char* joiner_name = (char*) malloc(MAX_QUEUE_NAME_SIZE);
    char prefix[] = "/chat-";
    strcpy(joiner_name, prefix);
    int j = 6;
    int i = 0;
    while(msg[i] != ':' && msg[i] != 0){
        joiner_name[j++] = msg[i++];
    }
    joiner_name[j] = 0;
    i++;
    char join[] = "JOIN";
    int a;
    for(a = 0; a < strlen(join)+1; a++){
        if(msg[i++] != join[a]){
            free(joiner_name);
            return NULL;
        }
    }
    return joiner_name;
}

char* quitter(char* msg){
    char* quitter_name = (char*) malloc(MAX_QUEUE_NAME_SIZE);
    char prefix[] = "/chat-";
    strcpy(quitter_name, prefix);
    int j = 6;
    int i = 0;
    while(msg[i] != ':' && msg[i] != 0){
        quitter_name[j++] = msg[i++];
    }
    quitter_name[j] = 0;
    i++;
    char leave[] = "LEAVE";
    int a;
    for(a = 0; a < strlen(leave)+1; a++){
        if(msg[i++] != leave[a]){
            free(quitter_name);
            return NULL;
        }
    }
    return quitter_name;
}

int member_is_online(char* member_name){
    char** online = get_online_queues();
    int i = 0;
    while(strlen(online[i]) > 0){
        if(strcmp(online[i], member_name) == 0){
            return 1;
        }
    }
    return 0;
}

int parse_channel_message(char* msg, char* sender, char* content){
    char* sender_name = (char*) malloc(MAX_QUEUE_NAME_SIZE);
    char prefix[] = "/chat-";
    char marker[] = ":";
    strcpy(sender_name, prefix);
    int j = 6;
    int i = 0;
    while(msg[i] != ':' && msg[i] != 0){
        sender_name[j++] = msg[i++];
    }
    sender_name[j] = 0;
    i++;

    if(msg[i++] != '#'){
        return 0;
    }

    while(msg[i++] != ':'){
    }

    strcpy(sender, sender_name);
    strcpy(content, &sender_name[6]);
    strcat(content, ": ");

    int m = strlen(content);
    while(msg[i] != 0){
        content[m++] = msg[i++];
    }
    content[m] = 0;

    return 1;
}

void format_refusal(char* final_message, char* sender, char* queue_name){
    char channel_marker[] = "#";
    char marker[] = ":";
    char warn[] = ":NOT A MEMBER";
    strcpy(final_message, channel_marker);
    strcat(final_message, &queue_name[7]);
    strcat(final_message, marker);
    strcat(final_message, &sender[6]);
    strcat(final_message, warn);
}

#endif