#ifndef CHANNEL_MANAGER_H
#define CHANNEL_MANAGER_H

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
#include "messenger_utils.h"


typedef struct member{
    char name[200];
    struct member* next;
}MEMBER;

typedef struct channel{
    int n_members;
    char name[200];
    MEMBER* members;
    struct channel* next;
}CHANNEL;

typedef struct header{
    int n_channels_created;
    int n_channels_joined;
    CHANNEL* my_channels;
    CHANNEL* joined_channels;
}HEADER;

HEADER channel_header;

void init_channel_manager(){
    channel_header.n_channels_created = 0;
    channel_header.n_channels_joined = 0;
    channel_header.my_channels = NULL;
    channel_header.joined_channels = NULL;
}

CHANNEL* last_channel_in_joined(){
    CHANNEL* current = channel_header.joined_channels;
    if(current == NULL){
        // Never joined channel
    }else{
        // Have joined a channel
        while(current->next != NULL){
            current = current->next;
        }
    }
    return current;   
}

CHANNEL* last_channel_in_created(){
    CHANNEL* current = channel_header.my_channels;
    if(current == NULL){
        // Never created channel
    }else{
        // Have created a channel
        while(current->next != NULL){
            current = current->next;
        }
    }
    return current;   
}

int can_join(char* channel_name){
    CHANNEL* current = channel_header.joined_channels;
    if(current == NULL){
        return 1;
    }else{
        while(current != NULL){
            if(strcmp(current->name, channel_name) == 0){
                return 0;
            }
            current = current->next;
        }
    }
    return 1;
}

int can_create(char* channel_name){
    char** queue_list = get_online_queues();
    CHANNEL* current = channel_header.joined_channels;
    if(current == NULL){
        
    }else{
        while(current != NULL){
            if(strcmp(current->name, channel_name) == 0){
                return 0;
            }
            current = current->next;
        }
    }

    int index = 0;

    while(strlen(queue_list[index]) > 0){
        char* current_queue = queue_list[index++];
        if(strcmp(current_queue, channel_name) == 0){
            return 0;
        }
    }

    return 1;
}

void join_channel(char* channel_name){
    if(can_join(channel_name)){
        CHANNEL* new_channel = (CHANNEL*) malloc(sizeof(CHANNEL));

        new_channel->n_members = -1;
        new_channel->next = NULL;
        new_channel->members = NULL;
        strcpy(new_channel->name, channel_name);
        
        CHANNEL* last_channel = last_channel_in_joined();

        if(last_channel == NULL){
            channel_header.joined_channels = new_channel;
        }else{
            last_channel->next = new_channel;
        }
        channel_header.n_channels_joined++;
    }
}

void add_channel_in_created(char* channel_name, char* creator){
    CHANNEL* new_channel = (CHANNEL*) malloc(sizeof(CHANNEL));
    strcpy(new_channel->name, channel_name);
    new_channel->members = (MEMBER*) malloc(sizeof(MEMBER));
    new_channel->members->next = NULL;
    strcpy(new_channel->members->name, creator);
    new_channel->n_members = 1;
    
    CHANNEL* last_channel = last_channel_in_created();
    if(last_channel == NULL){
        channel_header.my_channels = new_channel;
    }else{
        last_channel->next = new_channel;
    }
}

void create_channel(char* channel_name, char* creator){
    if(can_create(channel_name)){
        struct mq_attr* attr = (struct mq_attr*) malloc(sizeof(struct mq_attr));
        attr->mq_maxmsg = 10;
        attr->mq_msgsize = 522;
        attr->mq_flags = 0;
        mode_t prev_umask = umask(0155);
        mqd_t client_queue = mq_open(channel_name, O_CREAT, 0666, attr);
        // perror("Opening client queue");
        mq_close(client_queue);
        // perror("Closing client queue");
        umask(prev_umask);

        add_channel_in_created(channel_name, creator);
        channel_header.n_channels_created++;
    }
}

CHANNEL* get_channel(char* channel_name){
    CHANNEL* current = channel_header.my_channels;
    if(current == NULL){

    }else{
        while(strcmp(current->name, channel_name) != 0){
            current = current->next;
            if(current == NULL){
                break;
            }
        }
    }
    return current;
}

MEMBER* last_member_in_channel(char* channel_name){
    CHANNEL* channel = get_channel(channel_name);
    if(channel == NULL){
        return NULL;
    }else{
        MEMBER* current = channel->members;
        if(current == NULL){
            return NULL;
        }else{
            while(current->next != NULL){
                current = current->next;
            }
            return current;
        }
    }
}

int member_exists(MEMBER* first_member, char* member_name){
    MEMBER* current = first_member;
    while(current != NULL){
        if(strcmp(current->name, member_name) == 0){
            return 1;
        }
        current = current->next;
    }
    return 0;
}

void add_member_in_channel(char* channel_name, char* member_name){
    if(!can_create(channel_name)){
        // Channel exists
        CHANNEL* channel = get_channel(channel_name);
        if(member_exists(channel->members, member_name)){
            // member already added
        }else{
            //have to add member
            if(channel->members == NULL){
                // no members
                printf("[This should never happen] - No members existing in channel\n");
            }else{
                MEMBER* last_member = last_member_in_channel(channel_name);
                last_member->next = (MEMBER*) malloc(sizeof(MEMBER));
                strcpy(last_member->next->name, member_name);
                last_member->next->next = NULL;
            }
            channel->n_members++;
        }
    }
}

void remove_member_from_channel(char* channel_name, char* member_name){
    if(!can_create(channel_name)){
        // Channel exists
        CHANNEL* channel = get_channel(channel_name);
        if(member_exists(channel->members, member_name)){
            MEMBER* prev = NULL;
            MEMBER* current = channel->members;
            if(strcmp(member_name, current->name) == 0){
                channel->members = current->next;
            }else{
                while(strcmp(member_name, current->name) != 0){
                    prev = current;
                    current = current->next;
                    if(current == NULL){
                        return;
                    }
                }
                prev->next = current->next;
            }
            free(current);
            channel->n_members--;
        }
    }
}

int have_joined_channel(char* channel_name){
    if(can_join(channel_name)){
        return 0;
    }
    return 1;
}

int len_channels_joined(){
    return channel_header.n_channels_joined;
}

char** channels_joined_list(){
    CHANNEL* current = channel_header.joined_channels;

    if(current != NULL){
        char** channels_list = (char**) malloc(sizeof(char*) * channel_header.n_channels_joined);
        int index = 0;

        channels_list[index++] = current->name;

        while(current->next != NULL){
            current = current->next;
            channels_list[index++] = current->name;
        }
        return channels_list;
    }else{
        return NULL;
    }
}

char** channels_created_list(){
    CHANNEL* current = channel_header.my_channels;

    if(current != NULL){
        char** channels_list = (char**) malloc(sizeof(char*) * channel_header.n_channels_joined);
        int index = 0;

        channels_list[index++] = current->name;

        while(current->next != NULL){
            current = current->next;
            channels_list[index++] = current->name;
        }
        return channels_list;
    }else{
        return NULL;
    }
}

char** get_members_from_channel(char* channel_name){
    if(!can_create(channel_name)){
        CHANNEL* channel = get_channel(channel_name);
        MEMBER* current = channel->members;
        char** members_list = (char**) malloc(sizeof(char*) * channel->n_members);
        int index = 0;
        while(current != NULL){
            members_list[index++] = current->name;
            current = current->next;
        }
        return members_list;
    }
    return NULL;
}

void remove_joined_channel(char* channel_name){
    if(have_joined_channel(channel_name)){
        CHANNEL* prev = NULL;
        CHANNEL* current = channel_header.joined_channels;
        
        if(strcmp(current->name, channel_name) == 0){
            channel_header.joined_channels = current->next;
        }else{
            while(strcmp(current->name, channel_name) != 0){
                prev = current;
                current = current->next;
            }
            prev->next = current->next;
        }
        free(current);
        channel_header.n_channels_joined--;
    }
}

int len_channels_created(){
    return channel_header.n_channels_created;
}

void remove_created_channel(char* channel_name){
    if(!can_create(channel_name)){
        // Channel exists
        CHANNEL* current = channel_header.my_channels;
        if(current != NULL){
            if(strcmp(current->name, channel_name) == 0){
                channel_header.my_channels = current->next;
                channel_header.n_channels_created--;
                free(current);
                mq_unlink(channel_name);
            }else{
                CHANNEL* prev;
                while(strcmp(current->name, channel_name) != 0 && current != NULL){
                    prev = current;
                    current = current->next;
                }

                if(current != NULL){
                    prev->next = current->next;
                    free(current);
                    mq_unlink(channel_name);
                    channel_header.n_channels_created--;
                }
            }
        }
    }
}

void destroy_all_of_my_channels(){
    char** channels = channels_created_list();
    int n = channel_header.n_channels_created;
    int i;
    for(i = 0; i < n; i++){
        mq_unlink(channels[i]);
    }
}

int is_member(char* name, char* channel_name){
    if(!can_join(channel_name)){
        return 1;
    }
    
    CHANNEL* channel = get_channel(channel_name);
    
    if(channel != NULL){
        return member_exists(channel->members, name);
    }
    return 0;
}

int is_my_channel(char* channel_name){
    CHANNEL* channel = get_channel(channel_name);
    return channel != NULL;
}

#endif