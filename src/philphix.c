/*
 * Include the provided hash table library.
 */
#include "hashtable.h"

/*
 * Include the header file.
 */
#include "philphix.h"

/*
 * Standard IO and file routines.
 */
#include <stdio.h>

/*
 * General utility routines (including malloc()).
 */
#include <stdlib.h>

/*
 * Character utility routines.
 */
#include <ctype.h>

/*
 * String utility routines.
 */
#include <string.h>

/*
 * This hash table stores the dictionary.
 */
HashTable *dictionary;

/*
 * The MAIN routine.  You can safely print debugging information
 * to standard error (stderr) as shown and it will be ignored in 
 * the grading process.
 */
int main(int argc, char **argv)
{
        if (argc != 2) {
                fprintf(stderr, "Specify a dictionary\n");
                return 1;
        }
        /*
         * Allocate a hash table to store the dictionary.
         */
        fprintf(stderr, "Creating hashtable\n");
        dictionary = createHashTable(0x61C, &stringHash, &stringEquals);

        fprintf(stderr, "Loading dictionary %s\n", argv[1]);
        readDictionary(argv[1]);
        fprintf(stderr, "Dictionary loaded\n");

        fprintf(stderr, "Processing stdin\n");
        processInput();

        /*
         * The MAIN function in C should always return 0 as a way of telling
         * whatever program invoked this that everything went OK.
         */
        return 0;
}

/*
 * This should hash a string to a bucket index.  void *s can be safely cast
 * to a char * (null terminated string)
 */
unsigned int stringHash(void *s)
{
        // -- TODO --
        // fprintf(stderr, "need to implement stringHash\n");

        /* To suppress compiler warning until you implement this function, */
        unsigned int hash = 0;
        for (char *p = s; *p; p++) {
                hash = hash * 31 + (*p);
        }
        return hash;
}

/*
 * This should return a nonzero value if the two strings are identical 
 * (case sensitive comparison) and 0 otherwise.
 */
int stringEquals(void *s1, void *s2)
{
        // -- TODO --
        // fprintf(stderr, "You need to implement stringEquals");
        /* To suppress compiler warning until you implement this function */
        return !strcmp(s1, s2);
}

int isAlphanumeric(char c)
{
        return ('a' <= c && c <= 'z') ||
               ('A' <= c && c <= 'Z') ||
               ('0' <= c && c <= '9');
}

/*
 * This function should read in every word and replacement from the dictionary
 * and store it in the hash table.  You should first open the file specified,
 * then read the words one at a time and insert them into the dictionary.
 * Once the file is read in completely, return.  You will need to allocate
 * (using malloc()) space for each word.  As described in the spec, you
 * can initially assume that no word is longer than 60 characters.  However,
 * for the final bit of your grade, you cannot assumed that words have a bounded
 * length.  You CANNOT assume that the specified file exists.  If the file does
 * NOT exist, you should print some message to standard error and call exit(61)
 * to cleanly exit the program.
 */
void readDictionary(char *dictName)
{
        // -- TODO --
        // fprintf(stderr, "You need to implement readDictionary\n");
        FILE *dictFile = fopen(dictName, "r");
        if (dictFile == NULL) {
                fclose(dictFile);
                fprintf(stderr, "File does not exist.");
                exit(61);
        }
        char *key, *val;
        while (1) {
                key = malloc(61);
                if (fscanf(dictFile, "%s", key) == EOF) {
                        free(key);
                        break;
                }
                val = malloc(61);
                fscanf(dictFile, "%s", val);
                insertData(dictionary, key, val);
        }
        fclose(dictFile);
}

char *lowerAllButFirst(char *s) {
        char *lowered = malloc(strlen(s) + 1);
        strcpy(lowered, s);
        for (int i = 1; s[i]; i++) {
                lowered[i] = tolower(s[i]);
        }
        return lowered;
}

char *lowerAll(char *s) {
        char *lowered = malloc(strlen(s) + 1);
        strcpy(lowered, s);
        for (int i = 0; s[i]; i++) {
                lowered[i] = tolower(s[i]);
        }
        return lowered;
}

char *findFromDictionary(char *key) {
        char *labf = lowerAllButFirst(key);
        char *la = lowerAll(key);
        char *foundData = findData(dictionary, key);
        if (foundData == NULL) {
                foundData = findData(dictionary, labf);
        }
        if (foundData == NULL) {
                foundData = findData(dictionary, la);
        }
        free(labf);
        free(la);
        return foundData;
}

/*
 * This should process standard input (stdin) and perform replacements as 
 * described by the replacement set then print either the original text or 
 * the replacement to standard output (stdout) as specified in the spec (e.g., 
 * if a replacement set of `taest test\n` was used and the string "this is 
 * a taest of  this-proGram" was given to stdin, the output to stdout should be 
 * "this is a test of  this-proGram").  All words should be checked
 * against the replacement set as they are input, then with all but the first
 * letter converted to lowercase, and finally with all letters converted
 * to lowercase.  Only if all 3 cases are not in the replacement set should 
 * it report the original word.
 *
 * Since we care about preserving whitespace and pass through all non alphabet
 * characters untouched, scanf() is probably insufficent (since it only considers
 * whitespace as breaking strings), meaning you will probably have
 * to get characters from stdin one at a time.
 *
 * Do note that even under the initial assumption that no word is longer than 60
 * characters, you may still encounter strings of non-alphabetic characters (e.g.,
 * numbers and punctuation) which are longer than 60 characters. Again, for the 
 * final bit of your grade, you cannot assume words have a bounded length.
 */
void processInput()
{
        // -- TODO --
        // fprintf(stderr, "You need to implement processInput\n");
        char c;
        char *str = malloc(8);
        unsigned int size = 0, arrsize = 8;
        while ((c = (char) getc(stdin)) != EOF) {
//        while (fread(&c, 1, 1, stdin)) {
                if (isAlphanumeric(c)) {
                        str[size++] = c;
                        str[size] = 0;
                        if (size + 1 == arrsize) {
                                str = realloc(str, arrsize *= 2);
                        }
                } else {
                        char *found = findFromDictionary(str);
                        printf("%s", found ? found : str);
                        size = 0;
                        str[0] = 0;
                        printf("%c", c);
//                        fwrite(&c, 1, 1, stdout);
                }
        }
        char *found = findFromDictionary(str);
        printf("%s", found ? found : str);
        free(str);
}
