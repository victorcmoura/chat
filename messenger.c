// Aluno: Victor Correia de Moura
// Matrícula: 15/0150792
// CMD: gcc -o prog -lrt -lpthread messenger.c

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <mqueue.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "string_map.h"

#define MAX_MESSAGE_SIZE 522
#define MAX_QUEUE_SIZE 200
#define MAX_USERNAME_SIZE 10
#define MAX_QUEUE_NAME_SIZE 16

typedef struct {
    char* msg_buffer;
    char* to;
} message_struct;

struct mq_attr attr;

int input_mode = 0, should_quit = 0, chat_mode = 0;

char queue_name[MAX_QUEUE_NAME_SIZE] = "/chat-";

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

void receive_input(char* msg_buffer){
    mqd_t client_queue = mq_open(queue_name, O_RDONLY, 0666, &attr);
    // perror("Opening client queue");
    mq_receive(client_queue, (char*) msg_buffer, MAX_MESSAGE_SIZE, 0);
    // perror("Input receiving");
}

void* receive_message_thread(void* args){
    while(1){
        char* buffer = (char*) malloc(MAX_MESSAGE_SIZE);
        char* final_message = (char*) malloc(MAX_MESSAGE_SIZE);
        receive_input(buffer);
        unformat_from_message_protocol(final_message, buffer);
    
        char* sender_name = get_sender_queue_name_from_unformatted_message(final_message);

        map_insert(sender_name, final_message);
        if(chat_mode && strcmp(sender_name, queue_name) != 0){
            system("clear");
            print_conversation(sender_name);
        }
        free(sender_name);
    }
    pthread_exit(NULL);
}

void* send_message_thread(void* args){
    message_struct* msg_data = args;
    char* peer_queue_name = msg_data->to;
    char* msg_buffer = msg_data->msg_buffer;

    msg_buffer[strlen(msg_buffer)-1] = 0;
    
    mqd_t peer_queue = mq_open(peer_queue_name, O_WRONLY, 0666, &attr);
    // perror("Opening peer queue");
    mq_send(peer_queue, (char*) msg_buffer, MAX_MESSAGE_SIZE, 0);
    // perror("Sending output");
    mq_close(peer_queue);
    // perror("Closing peer queue");

    char* buffer = (char*) malloc(MAX_MESSAGE_SIZE);

    unformat_from_message_protocol(buffer, msg_buffer);
    if(strcmp(queue_name, peer_queue_name) != 0){
        map_insert(peer_queue_name, buffer);
    }

    free(msg_buffer);
    free(msg_data);

    pthread_exit(NULL);
}

void format_into_message_protocol(char* final_message, char* name, char* msg){
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

void clear_stdin(void){
    int c;
    do{
        c = getchar();
    }while (c != '\n' && c != EOF);
}

void remove_line_breaks(char* buffer, int bufferlen){
    int i;
    for(i = 0; i < bufferlen; i++){
        if(buffer[i] == '\n'){
            buffer[i] = '\0';
        }
    }
}

void create_client_queue(){
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MESSAGE_SIZE;
    attr.mq_flags = 0;
    mode_t prev_umask = umask(0000);
    mqd_t client_queue = mq_open(queue_name, O_CREAT, 0666, &attr);
    // perror("Opening client queue");
    mq_close(client_queue);
    // perror("Closing client queue");
    umask(prev_umask);
}

void send_output(char* msg_buffer, char* peer_queue_name){
    message_struct* msg = (message_struct*) malloc(sizeof(message_struct));
    msg->to = peer_queue_name;
    msg->msg_buffer = msg_buffer;

    pthread_t send_message_thread_id;
    pthread_create(&send_message_thread_id, NULL, (void*) send_message_thread, msg);
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

void send_message_menu(){
    system("clear");

    char* recipient_queue_name = choose_queue();

    clear_stdin();

    if(recipient_queue_name != NULL){
        printf("Chose send message\n");
    
        char* buffer = (char*) malloc(MAX_MESSAGE_SIZE);
        char* final_message = (char*) malloc(MAX_MESSAGE_SIZE);
        fgets(buffer, MAX_MESSAGE_SIZE, stdin);
        format_into_message_protocol(final_message, recipient_queue_name, buffer);
        send_output(final_message, recipient_queue_name);
        printf("Sending message to: %s\n", recipient_queue_name);
        free(buffer);
        sleep(1);
    }
}

void* handle_user_input(){
    input_mode = 0;
    int option;
    do{
        system ("stty -icanon -isig -echo min 1 time 0");
        option = getchar();
        system ("stty cooked echo -isig");
        if(option == 27){
            should_quit = 1;
            break;
        }else if(option == 'i'){
            input_mode = 1;
            break;
        }
    }while(1);

    pthread_exit(NULL);
}

void read_message_menu(char* recipient_queue_name){
    if(recipient_queue_name != NULL){
        chat_mode = 1;
        pthread_t handle_user_input_thread_id;
        pthread_create(&handle_user_input_thread_id, NULL, (void*) handle_user_input, NULL);
        
        system("clear");
        char** messages = map_get(recipient_queue_name);

        print_conversation(recipient_queue_name);
        
        while(1){
            if(input_mode){
                system("clear");
                print_conversation(recipient_queue_name);
                printf(">> ");
                char* buffer = (char*) malloc(MAX_MESSAGE_SIZE);
                char* final_message = (char*) malloc(MAX_MESSAGE_SIZE);
                fgets(buffer, MAX_MESSAGE_SIZE, stdin);
                format_into_message_protocol(final_message, recipient_queue_name, buffer);
                send_output(final_message, recipient_queue_name);
                pthread_create(&handle_user_input_thread_id, NULL, (void*) handle_user_input, NULL);
                system("clear");
                print_conversation(recipient_queue_name);
            }else if(should_quit){
                should_quit = 0;
                break;
            }
            sleep(0.5);
        }        
    }
    chat_mode = 0;
}

void pre_message_menu(){
    char* recipient_queue_name = choose_queue();
    clear_stdin();
    read_message_menu(recipient_queue_name);
    free(recipient_queue_name);
}

void main_menu(){
    while(1){
        system("clear");
        print_menu_options();

        system ("stty -icanon -isig -echo min 1 time 0");
        int option;
        do{
            option = getchar();
            if(option >= '1' && option <= '2'){
                break;
            }
        }while(1);
        system ("stty cooked echo -isig");

        system("clear");
        
        if(option == '1'){
            pre_message_menu();
        }else if(option == '2'){
            system("clear");
            printf("Exiting...\n");
            sleep(2);
            return;
        }
    }
}

void set_queue_name(){
    char buffer[MAX_QUEUE_NAME_SIZE - 6] = {0};
    
    FILE* fp;
    fp = popen("whoami", "r");

    if (fp == NULL) {
        printf("Something wrong happened. Try again later.\n" );
        exit(1);
    }

    fgets(buffer, MAX_QUEUE_NAME_SIZE - 6, fp);
    remove_line_breaks(buffer, MAX_QUEUE_NAME_SIZE);
    strcat(queue_name, buffer);
}

void init_message_receiving_thread(){
    pthread_t receive_message_thread_id;
    pthread_create(&receive_message_thread_id, NULL, (void*) receive_message_thread, NULL);
}

int main(void){
    set_queue_name();
    create_client_queue();
    init_message_receiving_thread();
    init_map(10000, 100);
    main_menu();
    mq_unlink(queue_name);
    return 0;
}
