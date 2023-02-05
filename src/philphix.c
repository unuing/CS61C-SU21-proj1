#include "hashtable.h"
#include "philphix.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* This hash table stores the dictionary. */
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

        /* Allocate a hash table to store the dictionary. */
        fprintf(stderr, "Creating hashtable\n");
        dictionary = createHashTable(0x61C, &stringHash, &stringEquals);

        fprintf(stderr, "Loading dictionary %s\n", argv[1]);
        readDictionary(argv[1]);
        fprintf(stderr, "Dictionary loaded\n");

        fprintf(stderr, "Processing stdin\n");
        processInput();

        return 0;
}

/*
 * This should hash a string to a bucket index.  void *s can be safely cast
 * to a char * (null terminated string)
 */
unsigned int stringHash(void *s)
{
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
{ return !strcmp(s1, s2); }

int isAlphanumeric(char c)
{
        return ('a' <= c && c <= 'z') ||
               ('A' <= c && c <= 'Z') ||
               ('0' <= c && c <= '9');
}

int isWhiteSpace(char c)
{ return c == ' ' || c == '\t' || c == '\n'; }

int notWhiteSpace(char c)
{ return c != ' ' && c != '\t' && c != '\n'; }

char *getstr(FILE *stream, int (*accept)(char))
{
        unsigned int strsize = 0, arrsize = 8;
        char *str = calloc(arrsize, sizeof(char));
        char c;
        while ((c = (char) getc(stream)) != EOF) {
                if (!accept(c)) {
                        ungetc(c, stream);
                        break;
                }
                str[strsize++] = c;
                if (strsize + 1 == arrsize) {
                        str = realloc(str, arrsize *= 2);
                }
        }
        str[strsize] = 0;
        return strsize ? str : NULL;
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
        FILE *dictFile = fopen(dictName, "r");
        if (dictFile == NULL) {
                fclose(dictFile);
                fprintf(stderr, "File does not exist.");
                exit(61);
        }
        char *key, *val;
        while (key = getstr(dictFile, isAlphanumeric)) {
                getstr(dictFile, isWhiteSpace);
                val = getstr(dictFile, notWhiteSpace);
                getstr(dictFile, isWhiteSpace);
                insertData(dictionary, key, val);
        }
        free(key);
        fclose(dictFile);
}

char *lowerAllButFirst(char *s)
{
        char *lowered = malloc(strlen(s) + 1);
        int i;
        for (i = 0; s[i]; i++) {
                lowered[i] = (char) (i == 0 ? s[i] : tolower(s[i]));
        }
        lowered[i] = 0;
        return lowered;
}

char *lowerAll(char *s)
{
        char *lowered = malloc(strlen(s) + 1);
        int i;
        for (i = 0; s[i]; i++) {
                lowered[i] = (char) tolower(s[i]);
        }
        lowered[i] = 0;
        return lowered;
}

char *findFromDictionary(char *key)
{
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
        char c;
        unsigned int size = 0, arrsize = 8;
        char *str = calloc(arrsize, sizeof(char));
        while (fread(&c, 1, 1, stdin)) {
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
                        fwrite(&c, 1, 1, stdout);
                }
        }
        char *found = findFromDictionary(str);
        printf("%s", found ? found : str);
        free(str);
}
