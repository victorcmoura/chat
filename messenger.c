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

#define MAX_MESSAGE_SIZE 522
#define MAX_QUEUE_SIZE 200
#define MAX_USERNAME_SIZE 10
#define MAX_QUEUE_NAME_SIZE 16

#include "string_map.h"
#include "messenger_utils.h"
#include "gui_utils.h"
#include "channel_manager.h"

struct mq_attr attr;

int input_mode = 0, should_quit = 0, chat_mode = 0;
char current_chat[MAX_QUEUE_NAME_SIZE] = {0};

char queue_name[MAX_QUEUE_NAME_SIZE] = "/chat-";

void receive_input(char* msg_buffer, char* monitored_queue_name){
    mqd_t client_queue = mq_open(monitored_queue_name, O_RDONLY, 0666, &attr);
    // perror("Opening client queue");
    mq_receive(client_queue, (char*) msg_buffer, MAX_MESSAGE_SIZE, 0);
    // perror("Input receiving");
}

void* receive_message_thread(void* args){
    queue_struct* queue_data = args;
    char* monitored_queue_name = queue_data->queue_name;

    while(1){
        char* buffer = (char*) malloc(MAX_MESSAGE_SIZE);
        char* final_message = (char*) malloc(MAX_MESSAGE_SIZE);
        receive_input(buffer, monitored_queue_name);
        
        printf("[%s]\n", buffer);

        int is_broad = 0;

        if(is_broadcast(buffer)){
            is_broad = 1;
        }

        unformat_from_message_protocol(final_message, buffer);
    
        char* sender_name = get_sender_queue_name_from_unformatted_message(final_message);

        if(is_broad){
            char tmp[MAX_MESSAGE_SIZE] = "Broadcast from ";
            strcat(tmp, final_message);
            strcpy(final_message, tmp);
        }

        map_insert(sender_name, final_message);
        if(chat_mode && strcmp(sender_name, queue_name) != 0 && strcmp(sender_name, current_chat) == 0){
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
    
    mqd_t peer_queue = mq_open(peer_queue_name, O_WRONLY, 0622, &attr);
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

void create_client_queue(){
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MESSAGE_SIZE;
    attr.mq_flags = 0;
    mode_t prev_umask = umask(0155);
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
        if(strcmp(recipient_queue_name, "broad_to_all") == 0){
            pthread_t handle_user_input_thread_id;
            pthread_create(&handle_user_input_thread_id, NULL, (void*) handle_user_input, NULL);
            
            system("clear");
            
            print_broadcast_gui();

            while(1){
                if(input_mode){
                    system("clear");
                    print_broadcast_gui();
                    printf(">> ");
                    char* buffer = (char*) malloc(MAX_MESSAGE_SIZE);
                    fgets(buffer, MAX_MESSAGE_SIZE, stdin);

                    char** online_queues = get_online_queues();
                    int i;
                    for(i = 0; strlen(online_queues[i]) > 0; i++){
                        if(!is_channel_name(online_queues[i])){
                            char* final_message = (char*) malloc(MAX_MESSAGE_SIZE);
                            format_into_broadcast_protocol(final_message, buffer, queue_name);
                            send_output(final_message, online_queues[i]);
                        }
                    }
                    system("clear");
                    break;
                }else if(should_quit){
                    should_quit = 0;
                    break;
                }
                sleep(0.5);
            }        
        }else{
            if(is_channel_name(recipient_queue_name) && can_join(recipient_queue_name)){
                char* join_final_message = (char*) malloc(MAX_MESSAGE_SIZE);
                format_into_join_request_protocol(join_final_message, queue_name);
                send_output(join_final_message, recipient_queue_name);
            }

            chat_mode = 1;
            strcpy(current_chat, recipient_queue_name);
            pthread_t handle_user_input_thread_id;
            pthread_create(&handle_user_input_thread_id, NULL, (void*) handle_user_input, NULL);
            
            system("clear");

            print_conversation(recipient_queue_name);
            
            while(1){
                if(input_mode){
                    system("clear");
                    print_conversation(recipient_queue_name);
                    printf(">> ");
                    char* buffer = (char*) malloc(MAX_MESSAGE_SIZE);
                    char* final_message = (char*) malloc(MAX_MESSAGE_SIZE);
                    fgets(buffer, MAX_MESSAGE_SIZE, stdin);
                    format_into_message_protocol(final_message, recipient_queue_name, buffer, queue_name);
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
}

void init_message_receiving_thread(char* monitored_queue_name){
    queue_struct* queue_data = (queue_struct*) malloc(sizeof(queue_struct));
    strcpy(queue_data->queue_name, monitored_queue_name);

    pthread_t receive_message_thread_id;
    pthread_create(&receive_message_thread_id, NULL, (void*) receive_message_thread, queue_data);
}

void pre_message_menu(){
    char created_queue[MAX_QUEUE_NAME_SIZE] = "";
    char* recipient_queue_name = choose_queue(queue_name, created_queue);
    
    if(strlen(created_queue) > 0){
        clear_stdin();
        init_message_receiving_thread(created_queue);
    }else{
        clear_stdin();
        read_message_menu(recipient_queue_name);
        free(recipient_queue_name);
    }
    
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

int main(void){
    set_queue_name();
    create_client_queue();
    init_message_receiving_thread(queue_name);
    init_map(10000, 100);
    init_channel_manager();
    main_menu();
    mq_unlink(queue_name);
    destroy_all_of_my_channels();
    return 0;
}
