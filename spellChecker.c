#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
    // FIXME: implement
    char* curWord = nextWord(file);

    while(curWord != NULL) {
        hashMapPut(map, curWord,99);//using 99 as placeholder for debugging
        free(curWord);
        curWord = nextWord(file);
    }
}

/**
 * Calculates the Levenshtein distance between two words
 * @param word1, word2
 * return int
 */
int levDist(char* word1, char* word2){
    int len1 = (unsigned)strlen(word1) +1;
    int len2 = (unsigned)strlen(word2) +1;
    int d[len1][len2];
    int minCost;

    for (int i = 0; i < len1; i++) {
        for (int j = 0; j < len2; j++) {
            d[i][j] = 0;
        }
    }

    for (int i = 1; i < len1; i++) {
        d[i][0] = i;//word1 goes down row
    }

    for (int j = 1; j < len2; j++) {
        d[0][j] = j;//word2 goes accross col
    }

    for (int j = 1; j < len2; j++) {
        for (int i = 1; i < len1; i++) {
            minCost = d[i-1][j-1] + (word1[i-1] == word2[j-1] ? 0 : 1);//substitution
            minCost = minCost < (d[i-1][j] +1) ? minCost : (d[i-1][j] +1);//deletion
            minCost = minCost < (d[i][j-1] +1) ? minCost : (d[i][j-1] +1);//insertion

            d[i][j] = minCost;
        }
    }

    for (int i = 0; i < len1; i++) {
        for (int j = 0; j < len2; j++) {
            // printf("%d ",d[i][j] );
        }
        // printf("\n" );
    }
    // printf("result: %d\n",d[len1-1][len2-1] );
    return d[len1-1][len2-1];
}

/**
 * Binary search algo seen in class. returns the index to put an element and
    maintain order
 * @param HashLink array, size of array, value to be used for ordering
 * return int
 */
 int binSrch(HashLink array[], int size, int val) {
     assert(array);
     int low = 0;
     int high = size;
     int mid;

     while (low < high) {
         mid = (low + high)/2;
         if (array[mid].value < val) {
             low = mid +1;
         } else {
             high = mid;
         }
     }
     return low;
 }

 /**
  * put a value in specific index to maintain order. slides higher values
  */
  void linkInsert(HashLink array[], HashLink* addMe, int size, int idx) {
      for (int i = size-1; i >= idx; i--) {
          array[i] = array[i-1];
      }

      array[idx].key = addMe->key;
      array[idx].value = addMe->value;
  }

/**
 * Checks the spelling of the word provded by the user. If the word is spelled incorrectly,
 * print the 5 closest words as determined by a metric like the Levenshtein distance.
 * Otherwise, indicate that the provded word is spelled correctly. Use dictionary.txt to
 * create the dictionary.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    // FIXME: implement
    HashMap* map = hashMapNew(1000);
    int numSuggestions = 5;
    HashLink possibles[numSuggestions];//array of possible suggestions for mispelled word

    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);


    char inputBuffer[256];
    while (1)
    {
        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);

        for (int i = 0; inputBuffer[i]; i++) {//make it all lowercase
            inputBuffer[i] = tolower(inputBuffer[i]);
        }

        if (strcmp(inputBuffer, "quit") == 0)
            break;

        // Implement the spell checker code here..
        //STEP1. if correctly mispelled, print msg then loop
        if (hashMapContainsKey(map, inputBuffer)) {
            printf("The inputed word '%s' is spelled correctly.\n", inputBuffer);
        } else {//word is misspelled
            HashLink* curLink;
            int putHere;

            for (int i = 0; i < numSuggestions; i++) {//initialize it with placeholders
                possibles[i].key = "n/a";
                possibles[i].value = 999;
                possibles[i].next = NULL;//not used
            }
            //STEP1. check entire dict for possible suggestions
            for (int i = 0; i < hashMapCapacity(map); i++) {
                curLink = map->table[i];
                while (curLink != NULL) {
                    curLink->value = levDist(curLink->key, inputBuffer);
                    putHere = binSrch(possibles, numSuggestions, curLink->value);
            //STEP2. add to array of suggestions if within 5 lowests
                    if (putHere < numSuggestions) {
                        linkInsert(possibles, curLink, numSuggestions, putHere);
                    }
                    curLink = curLink->next;
                }
            }
            //STEP3. print message with suggestions
            printf("The inputed word '%s' is misspelled. Did you mean?\n", inputBuffer);
            for (int i = 0; i < 5; i++) {
                printf("%s(%d) ",possibles[i].key, possibles[i].value );
            }
            printf("\n");
        }
    }

    hashMapDelete(map);
    printf("bye\n");
    return 0;
}
