#include <stdio.h>
#include <string.h>
#include "channel_manager.h"

int main(){
    // Join existing channels
    printf("JOINING EXISTING CHANNELS\n\n");

    char channel1[] = "canal1";
    char channel2[] = "canal2";
    char channel3[] = "canal3";
    char channel4[] = "canal4";

    init_channel_manager();

    printf("Joined channels: %d\n", len_channels_joined());

    join_channel(channel1);
    join_channel(channel2);
    join_channel(channel3);
    join_channel(channel4);

    int n = len_channels_joined();
    printf("Joined channels: %d\n", n);
    int i;
    char** list = channels_joined_list();
    for(i = 0; i < n; i++){
        printf("\t %d) %s\n", i, list[i]);
    }

    // Quit existing channels

    printf("QUIT EXISTING CHANNELS\n\n");

    remove_joined_channel(channel2);


    n = len_channels_joined();
    printf("Joined channels: %d\n", n);
    list = channels_joined_list();
    for(i = 0; i < n; i++){
        printf("\t %d) %s\n", i, list[i]);
    }

    // Create channel
    printf("CREATE CHANNEL\n\n");
    char mychan1[] = "/canal-criei1";
    char mychan2[] = "/canal-criei2";

    n = len_channels_created();
    printf("Created channels: %d\n", n);

    create_channel(mychan1, "/chat-mora");
    create_channel(mychan2, "/chat-mora");
    
    // Add member in channel
    printf("ADD MEMBER IN CHANNEL\n\n");

    add_member_in_channel(mychan1, "/chat-victorcmoura");
    add_member_in_channel(mychan1, "/chat-victorcmoura1");
    add_member_in_channel(mychan2, "/chat-victorcmoura2");

    n = len_channels_created();
    printf("Created channels: %d\n", n);
    list = channels_created_list();
    for(i = 0; i < n; i++){
        printf("\t %d) %s\n", i, list[i]);
        char** members_names = get_members_from_channel(list[i]);
        CHANNEL* current = get_channel(list[i]);
        if(current == NULL){
            printf("Current is nul\n");
        }
        printf("\t Quantity of members: %d\n", current->n_members);
        int d;
        for(d = 0; d < current->n_members; d++){
            printf("\t\t %d) %s\n", d, members_names[d]);
        }
    }

    // Remove member from channel
    printf("REMOVE MEMBER FROM CHANNEL\n\n");

    remove_member_from_channel(mychan1, "/chat-victorcmoura");

    n = len_channels_created();
    printf("Created channels: %d\n", n);
    list = channels_created_list();
    for(i = 0; i < n; i++){
        printf("\t %d) %s\n", i, list[i]);
        char** members_names = get_members_from_channel(list[i]);
        CHANNEL* current = get_channel(list[i]);
        if(current == NULL){
            printf("Current is nul\n");
        }
        printf("\t Quantity of members: %d\n", current->n_members);
        int d;
        for(d = 0; d < current->n_members; d++){
            printf("\t\t %d) %s\n", d, members_names[d]);
        }
    }

    // Delete channel
    printf("DELETE CHANNEL 1\n\n");

    remove_created_channel(mychan1);

    n = len_channels_created();
    printf("Created channels: %d\n", n);
    list = channels_created_list();
    for(i = 0; i < n; i++){
        printf("\t %d) %s\n", i, list[i]);
        char** members_names = get_members_from_channel(list[i]);
        CHANNEL* current = get_channel(list[i]);
        if(current == NULL){
            printf("Current is nul\n");
        }
        printf("\t Quantity of members: %d\n", current->n_members);
        int d;
        for(d = 0; d < current->n_members; d++){
            printf("\t\t %d) %s\n", d, members_names[d]);
        }
    }

    remove_created_channel(mychan2);
    printf("DELETE CHANNEL 2\n\n");

    n = len_channels_created();
    printf("Created channels: %d\n", n);
    list = channels_created_list();
    for(i = 0; i < n; i++){
        printf("\t %d) %s\n", i, list[i]);
        char** members_names = get_members_from_channel(list[i]);
        CHANNEL* current = get_channel(list[i]);
        if(current == NULL){
            printf("Current is nul\n");
        }
        printf("\t Quantity of members: %d\n", current->n_members);
        int d;
        for(d = 0; d < current->n_members; d++){
            printf("\t\t %d) %s\n", d, members_names[d]);
        }
    }


}