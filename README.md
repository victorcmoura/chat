# Chat with POSIX mqueues

## Checkpoint 1 (06/04/2019)

### Implements
- Interactive GUI
    - Blocks special commands such as CTRL + C
    - User can choose options by pressing the correspondent key without pressing enter (When options can be chosen with only one key)
    - Exclusive chat window per queue
    - Messages in a session are stored in a custom coded string map, so the user can navigate to another chat window without losing sent or received messages

- Message protocol
    - User to user messaging
    - Due to the GUI, it is hard to send a message to an unexistant queue (there is a specific situation when this can occur)
    - Chat nickname is the user's unix username

## Known issues
- ~When a user A is chatting with user B and user B becomes offline (the queue is deleted), if chat A don't close the current chat window, it will be possible to send a message to an unexistant queue~ (FIXED)
- If another user chooses the same username as the user's unix username, it is not possible to choose another
- Message broadcasting was 90% implemented but, due to a segfault bug occurring in the GUI thread, it was not released on this checkpoint

## Challenges with threading
- User input thread
    - The challenge was to change the tty behaviour to enable stdin flushing without a line break
        - By changing incorrectly, this caused my stdout to ignore lots of formatting and the GUI became unreadable. But the problem was fixed =)
- Message outputting thread
    - I haven't had big issues
- Message receiving thread
    - I haven't had big issues

## Challenges unrelated with threading
### String manipulation 
 In C, it is such an overhead to a Python developer to deal with this.
### Data structures
In other languages, a data structure to store multiple users messages would look like:
```c++
// C++
map<string, vector<string>> messages;
```
```python
# Python3
messages = {'username': ["message one", "message two"]}
```
but, in C, it ended looking like:
```c
// string_map.h code snippet
void init_map(int n_keys, int n_values){
    max_value = n_keys;
    sem_init(&map_semaphore, 0, 1);
    int i;
    string_map = (char***) malloc(n_keys * sizeof(char***));

    for(i = 0; i < n_keys; i++){
        char* c = (char*) malloc(sizeof(char));
        *c = 1;
        string_map[i] = (char**) malloc(n_values * sizeof(char**));
        string_map[i][0] = c;
    }
}
```
and, to convert the strings into the 3-dimentional indexes:
```c
// string_map.h code snippet
unsigned long hash(unsigned char *str){
    unsigned long hash = 5381;
    int c;

    while (c = *str++){
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
        if(c == 0){
            break;
        }
    }

    return hash % max_value;
}
```
but now, it works like magic:
```c

init_map(1000, 11); // Up to 1000 users with 10 messages each

map_insert("victorcmoura", "message one");
char** victorcmoura_messages = map_get("victorcmoura");

int i;
for(i = 1; i < len("victorcmoura"); i++){
    printf("%s\n", messages[i]);
}
```
Output:
```
message one
```
