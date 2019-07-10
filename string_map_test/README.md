## String Map

In other languages, a data structure to store multiple users messages would look like:
```c++
// C++
map<string, vector<string>> messages;
```
```python
# Python3
messages = {'username': ["message one", "message two"]}
```
but, in C, it required me to code a custom data structure (basically a tridimensional matrix that stores characters and hashes the string to get the correspondent position in the array) and it ended like:
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