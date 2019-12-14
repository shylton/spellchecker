/*
 * CS 261 Data Structures
 * Assignment 5
 * Name: Shylton Matta
 * Date: 25 Nov 2019
 */

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

int hashFunction1(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += key[i];
    }
    return r;
}

int hashFunction2(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += (i + 1) * key[i];
    }
    return r;
}

/**
 * Creates a new hash table link with a copy of the key string.
 * @param key Key string to copy in the link.
 * @param value Value to set in the link.
 * @param next Pointer to set as the link's next.
 * @return Hash table link allocated on the heap.
 */
HashLink* hashLinkNew(const char* key, int value, HashLink* next)
{
    HashLink* link = malloc(sizeof(HashLink));
    assert(link);
    link->key = malloc(sizeof(char) * (strlen(key) + 1));
    assert(link->key);
    strcpy(link->key, key);
    link->value = value;
    link->next = next;
    return link;
}

/**
 * Free the allocated memory for a hash table link created with hashLinkNew.
 * @param link
 */
static void hashLinkDelete(HashLink* link)
{
    free(link->key);
    free(link);
}

/**
 * Initializes a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param map
 * @param capacity The number of table buckets.
 */
void hashMapInit(HashMap* map, int capacity)
{
    assert(map);
    map->capacity = capacity;
    map->size = 0;
    map->table = malloc(sizeof(HashLink*) * capacity);
    assert(map->table);
    for (int i = 0; i < capacity; i++)
    {
        map->table[i] = NULL;
    }
}

/**
 * Removes all links in the map and frees all allocated memory. You can use
 * hashLinkDelete to free the links.
 * @param map
 */
void hashMapCleanUp(HashMap* map)
{
    // FIXME: implement
    assert(map);

    for (int i = 0; i < map->capacity; i++) {
        HashLink* curLink = map->table[i];
        while (curLink != NULL) {
            HashLink* tempLink = curLink->next;
            hashLinkDelete(curLink);
            curLink = tempLink;
        }
    }
    free(map->table);
    map->size = 0;
}

/**
 * Creates a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param capacity The number of buckets.
 * @return The allocated map.
 */
HashMap* hashMapNew(int capacity)
{
    HashMap* map = malloc(sizeof(HashMap));
    assert(map);
    hashMapInit(map, capacity);
    return map;
}

/**
 * Removes all links in the map and frees all allocated memory, including the
 * map itself.
 * @param map
 */
void hashMapDelete(HashMap* map)
{
    hashMapCleanUp(map);
    free(map);
}

/**
 * Returns a pointer to the value of the link with the given key  and skip traversing as well. Returns NULL if no link with that key is in the table.
 *
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 *
 * @param map
 * @param key
 * @return Link value or NULL if no matching link.
 */
int* hashMapGet(HashMap* map, const char* key)
{
    // FIXME: implement
    assert(map);
    int* rtn = NULL;

    int idx = HASH_FUNCTION(key) % hashMapCapacity(map);
    HashLink* curLink = map->table[idx];

    while (NULL != curLink) {
        if (0 == strcmp(curLink->key, key)) {
            rtn = &(curLink->value);//save address of
            curLink = NULL;//to break out of while loop;
        } else {
            curLink = curLink->next;
        }
    }
    return rtn;
}

/**
 * Resizes the hash table to have a number of buckets equal to the given
 * capacity (double of the old capacity). After allocating the new table,
 * all of the links need to rehashed into it because the capacity has changed.
 *
 * Remember to free the old table and any old links if you use hashMapPut to
 * rehash them.
 *
 * @param map
 * @param capacity The new number of buckets.
 */
void resizeTable(HashMap* map, int capacity)
{
    // FIXME: implement
    assert(map);
    int oldCap = hashMapCapacity(map);
    HashMap* newMap = hashMapNew(oldCap * 2);
    HashLink* curLink;

    //cycle thru ea element
    for (int i = 0; i < oldCap; i++) {
        curLink = map->table[i];
        while (NULL != curLink) {
            hashMapPut(newMap, curLink->key, curLink->value);
            curLink = curLink->next;
        }
    }

    HashMap* tempMap = map;
    map = newMap;
    hashMapDelete(tempMap);

}

/**
 * Updates the given key-value pair in the hash table. If a link with the given
 * key already exists, this will just update the value and skip traversing. Otherwise, it will
 * create a new link with the given key and value and add it to the table
 * bucket's linked list. You can use hashLinkNew to create the link.
 *
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket.
 *
 * @param map
 * @param key
 * @param value
 */
void hashMapPut(HashMap* map, const char* key, int value)
{
    // FIXME: implement
    assert(map);

    int* putHere = hashMapGet(map, key);

    if (putHere != NULL) {
        *putHere = value;
    } else {
        int idx = HASH_FUNCTION(key) % hashMapCapacity(map);
        HashLink* tempLink = map->table[idx];
        map->table[idx] = hashLinkNew(key, value, tempLink);//adds to front
        map->size++;
    }
}

/**
 * Removes and frees the link with the given key from the table. If no such link
 * exists, this does nothing. Remember to search the entire linked list at the
 * bucket. You can use hashLinkDelete to free the link.
 * @param map
 * @param key
 */
void hashMapRemove(HashMap* map, const char* key)
{
    // FIXME: implement
    assert(map);

    //First check if key is in hash map. If its not, do nothing
    if (hashMapContainsKey(map, key)) {
        int idx = HASH_FUNCTION(key) % hashMapCapacity(map);
        HashLink* curLink = map->table[idx];
        while (1) {
            if (0 == strcmp(curLink->key, key)) {
                /* found key at first position in bucket */
                map->table[idx] = curLink->next;
                hashLinkDelete(curLink);
                map->size--;
                break;
            } else if (0 == strcmp(curLink->next->key, key)) {
                /* found in some other position */
                HashLink* tempLink = curLink->next;
                curLink->next = curLink->next->next;
                hashLinkDelete(tempLink);
                map->size--;
                break;
            } else {
                curLink = curLink->next;
            }
        }
    }
}

/**
 * Returns 1 if a link with the given key is in the table and 0 otherwise.
 *
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 *
 * @param map
 * @param key
 * @return 1 if the key is found, 0 otherwise.
 */
int hashMapContainsKey(HashMap* map, const char* key)
{
    // FIXME: implement
    assert(map);
    int idx = HASH_FUNCTION(key) % hashMapCapacity(map);

    HashLink* curLink = map->table[idx];

    while (curLink != NULL) {
        if (0 == strcmp(curLink->key, key)) {
            return 1;
        } else {
            curLink = curLink->next;
        }
    }

    return 0;//value not found
}

/**
 * Returns the number of links in the table.
 * @param map
 * @return Number of links in the table.
 */
int hashMapSize(HashMap* map)
{
    // FIXME: implement
    assert(map);
    return map->size;
}

/**
 * Returns the number of buckets in the table.
 * @param map
 * @return Number of buckets in the table.
 */
int hashMapCapacity(HashMap* map)
{
    // FIXME: implement
    assert(map);
    return map->capacity;
}

/**
 * Returns the number of table buckets without any links.
 * @param map
 * @return Number of empty buckets.
 */
int hashMapEmptyBuckets(HashMap* map)
{
    // FIXME: implement
    assert(map);
    int rtn = 0;

    for (int i = 0; i < map->capacity; i++) {
        if (NULL == map->table[i])
            rtn++;
    }
    return rtn;
}

/**
 * Returns the ratio of (number of links) / (number of buckets) in the table.
 * Remember that the buckets are linked lists, so this ratio tells you nothing
 * about the number of empty buckets. Remember also that the load is a floating
 * point number, so don't do integer division.
 * @param map
 * @return Table load.
 */
float hashMapTableLoad(HashMap* map)
{
    // FIXME: implement
    return ((float)hashMapSize(map)/ hashMapCapacity(map));
}

/**
 * Prints all the links in each of the buckets in the table.
 * @param map
 */
void hashMapPrint(HashMap* map)
{
  // FIXME: implement
  assert(map);

  if (0 == hashMapSize(map)) {
      printf("map is empty\n");
  } else {
     HashLink* curLink;
     for (int i = 0; i < map->capacity; i++) {
         curLink = map->table[i];

         if (curLink != NULL) {
             printf("\nbucket %d-> ",i);
             while (curLink != NULL) {
                 printf("[%s: %d] ", curLink->key, curLink->value);
                 curLink = curLink->next;
             }
         }
     }
     printf("\n");
  }
}

/**
 MAIN FOR TESTING


 int main(int argc, char const *argv[]) {
     HashMap* test1 = hashMapNew(2);
     hashMapPrint(test1);
     printf("size: %d\n",hashMapSize(test1) );
     //void hashMapPut(HashMap* map, const char* key, int value)
     hashMapPut(test1, "bob", 15);
     hashMapPut(test1, "tob", 18);
     hashMapPut(test1, "cob", 17);
     hashMapPut(test1, "zob", 77);
     printf("size: %d\n",hashMapSize(test1) );
     hashMapPrint(test1);
     hashMapRemove(test1, "zob");
     printf("size: %d\n",hashMapSize(test1) );
     hashMapPrint(test1);
     // hashMapCleanUp(test1);
     // hashMapPrint(test1);

     return 0;
 } */
