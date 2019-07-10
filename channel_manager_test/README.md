## Channel Manager

To implement the chat room feature (channels), I had to build a custom data structure schema that could manage which rooms I have currently created, how many and which users have joined each room and which rooms I have joined. The implementation is based on a HEADER struct that contains two linked lists:
- The channels I have created (```my_channels```);
- The channels I have joined (```joined_channels```);

and some metadata:
- Number of channels I have created (```n_channels_created```);
- Number of channels I have joined (```n_channels_joined```);

```c
typedef struct header{
    int n_channels_created;
    int n_channels_joined;
    CHANNEL* my_channels;
    CHANNEL* joined_channels;
}HEADER;
```

In each of these linked lists, there are many ```CHANNEL``` nodes that contain the following data:
- The number of members that joined the channel (```n_members```);
- The name of the channel (```name```);
- A linked list containing all of the ```MEMBER``` nodes (```members```);
- The pointer to the next node (```next```);

```c
typedef struct channel{
    int n_members;
    char name[200];
    MEMBER* members;
    struct channel* next;
}CHANNEL;
```

Each ```MEMBER``` node in the ```member``` linked list contains the name of the member and the pointer to the next member:

```c
typedef struct member{
    char name[200];
    struct member* next;
}MEMBER;
```

And now, that's how it works:

```c
char my_personal_queue_name = "/chat-victorcmoura";
char my_friends_queue_name = "/chat-mouracvictor"

char channel1[] = "/canal-example";
char channel2[] = "/canal-example2";

// Join existing channels
join_channel(channel1);

// Quit existing channels
remove_joined_channel(channel1);

// Create channel
create_channel(channel2, my_personal_queue_name);

// Add member in channel
add_member_in_channel(channel2, my_friends_queue_name);

// Remove member from channel
remove_member_from_channel(channel2, my_friends_queue_name);

// Delete channel
remove_created_channel(channel2);
```