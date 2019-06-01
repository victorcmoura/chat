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

#define MAX_MESSAGE_SIZE 522
#define MAX_QUEUE_SIZE 200
#define MAX_USERNAME_SIZE 30
#define MAX_QUEUE_NAME_SIZE 35

struct mq_attr attr;

char queue_name[MAX_QUEUE_NAME_SIZE] = "/chat-";

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

void create_client_queue(char* queue_name){
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MESSAGE_SIZE;
    attr.mq_flags = 0;

    mqd_t client_queue = mq_open(queue_name, O_CREAT, 0666, &attr);
    // perror("Opening client queue");
    mq_close(client_queue);
    // perror("Closing client queue");
}

void receive_input(char* msg_buffer, char* queue_name){
    mqd_t client_queue = mq_open(queue_name, O_RDONLY, 0666, &attr);
    // perror("Opening client queue");
    mq_receive(client_queue, (char*) msg_buffer, MAX_MESSAGE_SIZE, 0);
    // perror("Input receiving");
}

void send_output(char* msg_buffer, char* queue_name){
    mqd_t peer_queue = mq_open(queue_name, O_WRONLY, 0666, &attr);
    // perror("Opening peer queue");
    mq_send(peer_queue, (char*) msg_buffer, MAX_MESSAGE_SIZE, 0);
    // perror("Sending output");
    mq_close(peer_queue);
    // perror("Closing peer queue");
}

void print_menu_options(){
    printf("Chat (choose an option):\n");
    printf("\t1 - Send message\n");
    printf("\t2 - Read message\n");
    printf("\t3 - Exit\n");
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
        if(fgets(buffer+1, MAX_QUEUE_NAME_SIZE - 6, fp) != NULL){
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
    for(i = 1; i<= index; i++){
        if(i != option){
            free(options[i]);
        }
    }
    return options[option];
}

void send_message_menu(){
    system("clear");

    char* recipient_queue_name = choose_queue();

    clear_stdin();

    printf("Chose send message\n");
    
    char buffer[MAX_MESSAGE_SIZE];
    fgets(buffer, MAX_MESSAGE_SIZE, stdin);
    send_output(buffer, recipient_queue_name);

    printf("Sending message to: %s\n", recipient_queue_name);
    free(recipient_queue_name);
    sleep(1);
}

void read_message_menu(){
    system("clear");
    
    char buffer[MAX_MESSAGE_SIZE];
    receive_input(buffer, queue_name);
    
    printf("Message 1:\n");
    printf("\t%s\n", buffer);
    printf("Press enter to exit\n");
    clear_stdin();
}

void main_menu(){
    while(1){
        system("clear");
        print_menu_options();

        system ("stty raw");
        int option;
        do{
            option = getchar();
            if(option >= '1' && option <= '3'){
                break;
            }
            printf("Invalid option =( Try again\n");
        }while(1);
        system ("stty cooked");

        system("clear");
        
        if(option == '1'){
            send_message_menu();
        }else if(option == '2'){
            read_message_menu();
        }else if(option == '3'){
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
    create_client_queue(queue_name);
    main_menu();
    mq_unlink(queue_name);
    return 0;
}
