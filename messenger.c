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
// #include "stack.h" // Thread safe stack lib

#define MAX_MESSAGE_SIZE sizeof(char) * 500
#define MAX_QUEUE_SIZE 200
#define MAX_USERNAME_SIZE sizeof(char) * 30
#define MAX_QUEUE_NAME_SIZE sizeof(char) * 35

void open_peer_queue(char* queue_name, mqd_t* queue){
    struct mq_attr attr;

    attr.mq_maxmsg = MAX_QUEUE_SIZE;
    attr.mq_msgsize = MAX_MESSAGE_SIZE;
    attr.mq_flags = 0;

    *queue = mq_open(queue_name, O_RDWR, 0666, &attr);
}

void open_client_queue(char* queue_name, mqd_t* queue){
    struct mq_attr attr;

    attr.mq_maxmsg = MAX_QUEUE_SIZE;
    attr.mq_msgsize = MAX_MESSAGE_SIZE;
    attr.mq_flags = 0;

    *queue = mq_open(queue_name, O_RDWR | O_CREAT, 0666, &attr);
}

void receive_input(mqd_t input_queue, char* msg_buffer){
    mq_receive(input_queue, (void*) msg_buffer, MAX_MESSAGE_SIZE, 0);
}

void send_output(char* peer_queue_name, char* msg_buffer){
    mqd_t output_queue;
    open_peer_queue(peer_queue_name, &output_queue);
    mq_send(output_queue, (void*) msg_buffer, MAX_MESSAGE_SIZE, 0);
}

int main(){
    char msg[MAX_MESSAGE_SIZE] = "This is my message";
    char username[MAX_USERNAME_SIZE] = "victorcmoura";
    char my_queue_name[MAX_QUEUE_NAME_SIZE] = "/chat-";


    strcat(my_queue_name, username);

    mqd_t my_queue;
    open_client_queue(my_queue_name, &my_queue);

    send_output(my_queue_name, msg);

    char r_msg[MAX_MESSAGE_SIZE] = "Not my message";

    receive_input(my_queue, r_msg);
    
    printf("%s\n", r_msg);

    return 0;
}
