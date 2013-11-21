/*
 * fw.c
 *
 *  Created on: Apr 16, 2011
 *      Author: Mattics
 *
 *      http://www.gidforums.com/t-4120.html
 *      http://www.algorithmist.com/index.php/Bubble_sort.c
 */

#include "header.h"

#define BUFFERSIZE 5
#define HASHSIZE 10001
#define TOPSIZE 10

char *getWord(FILE *fp);
unsigned hash(char *word);
struct list_node *lookUp(char *word);
void install(char *word);
char *strdup(char *s);
void bubbleSort(struct list_node **topWords, int tableSize);

/* Global declaration of the hash table */
static struct list_node *hashTable[HASHSIZE];

/* Structure of a node */
struct list_node {
	struct list_node *next;
	char *word;
	int count;
};

int main(int argc, char *argv[])
{
	FILE *fp;
	struct list_node *start, *lowestNode, **topWords;
	char *word;
	int i, counter = 1, sentinel = 0, tableSize, numWords = 0, n;

	/* If command has "-n", check to see if valid */
	if (strcmp("-n", argv[1]) == 0) {

		/* If inputs are less than/equal than 2, then output error */
		if (argc <= 2) {
			fprintf(stderr, "usage: fw [-n num] [ file1 "
					"[ file2 [...] ] ]\n");
			return 0;
		} else {

			/* Checks to see if the tableSize is a valid number */
			for (i = 0; i < strlen(argv[2]); i++) {

				/* If not valid, then output error message */
				if (argv[2][i] < 48 || argv[2][i] > 57) {
					fprintf(stderr, "usage: fw [-n num] [ file1 "
							"[ file2 [...] ] ]\n");
					return 0;
				}
			}

			/* Size of topWords array gets initialized if valid */
			tableSize = atoi(argv[2]);

			/* Start at the third argument */
			i = 3;
		}
	} else {

		/* If user didn't enter tablesize, set to default */
		i = 1;
		tableSize = 10;
	}

	/* Scans in file(s) and inputs words into hash table */
	for (; i < argc; i++) {

		/* If file can't be opened, output error */
		if(!(fp = fopen(argv[i], "r")))
			perror(argv[i]);
		else {

			/* Scans in word and inputs into hash table, if word != NULL */
			while ((word = getWord(fp)) != NULL) {
				if (word[0] != '\0') {
					install(strdup(word));
				}
				free(word);
			}
			fclose(fp);
		}
	}

	/* If tablesize is the same as default, then set to default */
	if (tableSize == TOPSIZE) {
		tableSize = TOPSIZE;
	}

	/* Create topWords array based on user input */
	topWords = malloc(tableSize * sizeof(*topWords));

	/* Initalize all indexes to NULL */
	for (i = 0; i < tableSize; i++) {
		topWords[i] = NULL;
	}

	/* Scan each node in hash table and checks for most common words */
	for (i = 0; i < HASHSIZE; i++) {
		if (hashTable[i] != NULL) {

			/*If hash table index exists, traverse through linked list */
			for (start = hashTable[i]; start != NULL;
					start = start->next) {

				/* Calculate number of words in hash table */
				numWords++;

				/* Enter this part once, initializes everything */
				if (sentinel == 0) {
					lowestNode = start;
					topWords[0] = lowestNode;
					sentinel = 1;

				/* Goes in here every other time */
				} else {

					/* As long as topWords array is not full, fill it */
					if (counter < tableSize) {
						for (n = 1; n < tableSize; n++) {
							if (topWords[n] == NULL) {
								topWords[n] = start;
								counter++;

								/* Updates lowest node */
								if (topWords[n]->count
										< lowestNode->count) {
									lowestNode = topWords[n];
								}
								break;
							}
						}

					/* When table is full, go here */
					} else {

						/* If node is more common than lowest node, swap */
						if (start->count > lowestNode->count) {
							topWords[tableSize - 1] = start;
							lowestNode = start;
						}
					}
				}

				/* Sorts the array and reinitializes the lowest node */
				bubbleSort(topWords, tableSize);
				for (n = 0; topWords[n] != NULL; n++) {
					lowestNode = topWords[n];
				}
			}
		}
	}

	/* Prints out the top # words */
	printf("The top %d words (out of %d) are:\n",
			tableSize, numWords);

	/* If user input is greater than unique words, resize the table */
	if (tableSize > numWords) {
		tableSize = numWords;
	}

	for (i = 0; i < tableSize; i++) {
		printf("%9d %s\n", topWords[i]->count, topWords[i]->word);
	}

	free(topWords);

	return 0;
}

/*
 * Function that checks to see if the word already exists in the hash table
 */
struct list_node *lookUp(char *word)
{
	struct list_node *node;

	/* Goes through the hash table to see if the word already exists */
	for (node = hashTable[hash(word)]; node != NULL;
			node = node->next) {

		/* If exists, return node, otherwise, return NULL */
		if (strcmp(word, node->word) == 0) {
			return node;
		}
	}

	return NULL;
}

/*
 * Adds the word to the hash table
 */
void install(char *word)
{
	struct list_node *node;
	unsigned hashval = hash(word);

	/* If word does not exist in the table, go here */
	if ((node = lookUp(word)) == NULL) {
		node = (struct list_node*)malloc(sizeof(*node));

		/* If allocating memory fails, exit */
		if (node == NULL) {
			return;
		}

		/* Adds the word in a node to the beginning of the linked list */
		node->word = word;
		node->next = hashTable[hashval];
		hashTable[hashval] = node;
		node->count = 1;

	/* If word already exists, increment the count */
	} else {
		node->count++;
	}
}

/*
 * Hashes the word so it can be put into the hash table
 */
unsigned hash(char *word)
{
	unsigned hashval;

	for (hashval = 0; *word != '\0'; word++)
		hashval = *word + 31 * hashval;

	return hashval % HASHSIZE;
}

/*
 * Sorts the topWords array based occurence of the word
 */
void bubbleSort(struct list_node **topWords, int tableSize)
{
	int i, j;
	struct list_node *temp;

	/* If word is not NULL */
	for (i = 0; i < tableSize && topWords[i] != NULL; i++) {
		for (j = 0; j < tableSize - 1 && topWords[j+1] != NULL; j++) {

			/* If two words have the same occurence number */
			if (topWords[j]->count == topWords[j+1]->count) {

				/* Sorts the two words alphabetically in reverse */
				if (strcmp(topWords[j]->word, topWords[j+1]->word) < 0) {
					temp = topWords[j+1];
					topWords[j+1] = topWords[j];
					topWords[j] = temp;
				}

			/* Sorts the words based on occurence of the word */
			} else if (topWords[j]->count < topWords[j+1]->count) {
				temp = topWords[j+1];
				topWords[j+1] = topWords[j];
				topWords[j] = temp;
			}
		}
	}
}

/*
 * Converts the word to get inputted into the hash table
 */
char *strdup(char *s)
{
   char *new;
   new = malloc(strlen(s) + 1);
   if (new != NULL)
   {
      strcpy(new, s);
   }
   return new;
}

/*
 * Grabs the word from the files
 */
char *getWord(FILE *fileInput)
{
	char *word, c;
	int size = sizeof(char) * BUFFERSIZE;
	int i = 0;

	/* Initializes the memory for word to be stored */
	word = (char *)malloc(size);

	/* If memory alloc fails, print out error */
	if (word == NULL) {
		fprintf(stderr, "Malloc error.\n");
		return 0;
	}

	/* As long as the letter scanned in is an alphabetic letter */
	while (isalpha(c = fgetc(fileInput))){

		/* If letter is uppercase, lowercase it */
		if (isupper(c))
			c = tolower(c);

		word[i] = c;

		/* Resize the memory allocation when it starts to get full */
		if (i >= (size - 2)) {
			size += BUFFERSIZE;
			word = (char *)realloc(word, size);

			/* If realloc fails, print out error message */
			if (word == NULL) {
				fprintf(stderr, "Realloc error.\n");
				return 0;
			}
		}
		i++;
	}
	word[i] = '\0';

	/* At end of file, return NULL */
	if (c == EOF)
		return NULL;

	return word;
}
