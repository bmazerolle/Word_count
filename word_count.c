/* ***********************************************************************************************
* Name: Ben Mazerolle 
* Student ID: V0086891 
* Course: SEng 265 A02 
* File Name: word_count.c
* File Description: A program to read an input file, count and index its words using a linked list, 
*					and display these values based on command line arguments
* ************************************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

/* Structure used to store individual strings */
typedef struct word{ 
	int length; 
	char *str; 
	struct word *down;
}word;

/* "Upper-level" structure that stores all strings of a specific length */
typedef struct count{ 
	int num_chars; 
	int num_words; 
	word *first_word; 
	struct count *next;
}count;

void free_list(count *first);
void free_count_list(count **head);
void print_list(count **head, int string_boolean); 
void front_back_split(count *source, count **front_ref, count **back_ref);
count *sorted_merge(count *front, count *back);
void sort_list(count **head);
int is_empty(count **head, count **tail);
void insert_count(count **head, count **tail, count *new_count);
char* to_lower(char **str);
void create_count(count **head, count **tail, word **new_word);
void sorted_insert(count **cur_count, word **new_word);
void insert_word(count **head, count **tail, word **new_word);
void create_word(count **head, count **tail, char *str);
void tokenize(count **head, count **tail, char words_buffer[]);


int main(int argc, char *argv[]){ 

	FILE *fp;
	count *head = NULL; 
	count *tail = NULL;
	int i;
	int infile = 0;
	int sort = 0;
	int print = 0; 
	
	if(argc<3){
		printf("Failure: not enough arguments");
		return -1; 
	}
	/* Determine location of input file in argv and presense of --sort and --print-words commands */
	for(i=0; i < argc; i++){ 
		if(strcmp(argv[i],"--infile")==0)
			infile = i;
		else if(strcmp(argv[i],"--sort")==0)
			sort = 1;
		else if(strcmp(argv[i],"--print-words")==0)
			print = 1;
	}

	fp = fopen(argv[infile+1], "r");
	/* Determine file size and allocate an array of correpsonding size to store the file's contents */
	fseek(fp, 0, SEEK_END);
	char words_buffer[ftell(fp)+1];
	fseek(fp, 0, SEEK_SET);
	
	if(fp==NULL){
		printf("Failure: unreadable file");
		return -1;
	}
	
	while(fgets(words_buffer, sizeof(words_buffer), fp)){
			if(strlen(words_buffer)>1)
				tokenize(&head, &tail, words_buffer);
	}
	/* Determine printing behavior based on command line arguments */
	if((print==1)&&(sort==1)){
		sort_list(&head);
		print_list(&head, print);
	}
	else if(sort == 1){
		sort_list(&head);
		print_list(&head, print);
	}
	else if(infile!=0)
		print_list(&head, print);
	/* Free the list and close the file */
	free_count_list(&head);
	fclose(fp);
	return 0;
}


/* Frees all memory malloc-ed for the word structs */
void free_list(count *first){ 

	word *temp_down = first->first_word;
	word *temp_down_next;
	
	while(temp_down!=NULL){
		temp_down_next = temp_down->down;
		free(temp_down->str);
		free(temp_down);
		temp_down = temp_down_next;
	}	
}

/* Frees all memory malloc-ed for the count struct list */
void free_count_list(count **head){ 
	
	count *temp = *head; 
	count *temp_next; 
	
	while(temp!=NULL){ 
		temp_next = temp->next;
		free_list(temp); 
		free(temp); 
		temp = temp_next;
	}
}

/* Prints the word count of each count struct, and if the string_boolean is set, prints the strings in the count list */
void print_list(count **head, int string_boolean){ 

	count *temp_traverse = *head;
	word *temp_down;
		for( ; temp_traverse!=NULL; temp_traverse = temp_traverse->next){ 
			printf("Count[%02d]=%02d;", temp_traverse->num_chars, temp_traverse->num_words);
			if(string_boolean == 1){
			printf(" (words: ");
				temp_down = temp_traverse->first_word;
				for( ; temp_down!=NULL; temp_down = temp_down->down){ 
					printf("\"%s\"", temp_down->str);
					if(temp_down->down!=NULL){
						if(temp_down->down->down == NULL)
							printf(" and ");
						else
							printf(", ");	
					}	
				}
				printf(")\n");
			}else
				printf("\n");	
		}
}

/* The following three functions are adapted from http://www.geeksforgeeks.org/merge-sort-for-linked-list/ */
/* Splits head into 'front' and 'back' sublists */
void front_back_split(count *source, count **front_ref, count **back_ref){
  count *fast;
  count *slow;
  if (source==NULL || source->next==NULL)
  {
    /* length < 2 cases */
    *front_ref = source;
    *back_ref = NULL;
  }
  else
  {
    slow = source;
    fast = source->next;
 
    /* Advance 'fast' two nodes, and advance 'slow' one node */
    while (fast != NULL)
    {
      fast = fast->next;
      if (fast != NULL)
      {
        slow = slow->next;
        fast = fast->next;
      }
    }
 
    /* 'slow' is before the midpoint in the list, so split it in two at that point. */
    *front_ref = source;
    *back_ref = slow->next;
    slow->next = NULL;
  }
}

/* Recursively merge sorts the front and back halves of the list */
count *sorted_merge(count *front, count *back){
  	count *result = NULL;
 
  	/* Base cases */
  	if (front == NULL)
  	   return(back);
	else if (back == NULL)
 	   return(front);
 
  /* Pick either a or b, and recur */
  if (front->num_words >= back->num_words){
     result = front;
     result->next = sorted_merge(front->next, back);
  }else{
     result = back;
     result->next = sorted_merge(front, back->next);
  }
  return(result);
}

/* Sorts the count list using mergesort */
void sort_list(count **head){
  	count *head_ref = *head;
 	count *front;
 	count *back;
 
	  /* Base case -- length 0 or 1 */
 	if ((head_ref == NULL) || (head_ref->next == NULL)){
    		return;
  	}
 
  	/* Split head into 'front' and 'back' sublists */
  	front_back_split(head_ref, &front, &back); 
 
  	/* Recursively sort the sublists */
  	sort_list(&front);
  	sort_list(&back);
 
  	/* answer = merge the two sorted lists together */
  	*head = sorted_merge(front, back);
}

/* Determines if the count list is empty */
int is_empty(count **head, count **tail){ 
	
	if((*head == NULL)||(*tail == NULL))
		return 1;

	return 0;
}

/* Inserts a new count struct in the list */
void insert_count(count **head, count **tail, count *new_count){
	/* If the list is empty, set the head and tail to be equal to the new count struct */
	if(is_empty(head, tail)){
		*head = new_count; 
		*tail = new_count;
		return;
	}
	/* If the new count struct is for lengths less than the current head's length, set it as the new head */
	if((new_count->num_chars)<((*head)->num_chars)){ 
		new_count->next = (*head);
		(*head) = new_count;
		return;
	}
	
	count *temp = *head;
	
	while(((temp->next)!=NULL) && ((temp->next->num_chars)<(new_count->num_chars))) 
		temp = temp->next;
	/* If at the end of the list, set the tail to point to the new count struct */	
	if(temp->next == NULL){
		count *last = (*tail);
		last->next = new_count; 
		(*tail) = new_count;
		return;
	}
	/* If no special instance, insert normally into the list */	
	count *next = temp->next;
	temp->next = new_count;
	new_count->next = next;	
	return;
}

/* Converts an input string to all-lowercase */
char* to_lower(char **str){ 
	
	unsigned char *mystr = (unsigned char *)(*str);

	while (*mystr) {
	        *mystr = tolower(*mystr);
	        mystr++;
	}
	return *str;
}

/* Creates a new count struct */
void create_count(count **head, count **tail, word **new_word){
	
	count *new_count = malloc(sizeof *new_count);
	int length = (*new_word)->length;

	/* Print an error if an error occurred malloc-ing a new count struct */
	if(new_count == NULL){
       		fprintf(stderr, "Error creating a new word node.\n");
       		exit(0);
    	}
	/* Initialize the new count struct's pointers, and set it's number of words to 1 */ 
	new_count->num_chars = length;
	new_count->num_words = 1;
	(*new_word)->str = to_lower(&((*new_word)->str));
	new_count->first_word = *new_word;
	new_count->next = NULL;
	insert_count(head, tail, new_count);	
	return;
}

/* Inserts a new word into the specific count's word list */
void sorted_insert(count **cur_count, word **new_word){ 
	
	word *trav = (*cur_count)->first_word;
	(*new_word)->str = to_lower(&((*new_word)->str));
	
	/* If the new word is alphanumerically higher than the first word of the count, make it the new first word */
	if(strncmp((*cur_count)->first_word->str, (*new_word)->str, strlen((*new_word)->str)+1) > 0){
		(*new_word)->down = (*cur_count)->first_word;
		(*cur_count)->first_word = *new_word;
		return;
	}
	/* If the new word is alphanumerically equal to the first word of the count, don't insert */ 
	if(strncmp(((*cur_count)->first_word)->str, (*new_word)->str, strlen((*new_word)->str)) == 0){
		free((*new_word)->str); 
		free(*new_word);
		return;
	}

	while(trav->down != NULL){
		/* If the new word is alphanumerically equal to traversal word, don't insert */ 
		if(strncmp(trav->str, (*new_word)->str, strlen((*new_word)->str)) == 0){
			free((*new_word)->str); 
			free(*new_word);
			return;
		}
		/* If the new word is alphanumerically less than the next traversal word, insert after traversale and before the next traversal */ 
		if(strncmp(trav->down->str, (*new_word)->str, strlen((*new_word)->str)+1) > 0){
			(*new_word)->down = trav->down;
			trav->down = (*new_word);
			return;
		}
		trav = trav->down;
	}
	/* If the new word is alphanumerically equal to the last word of the count, don't insert */ 
	if(strncmp(trav->str, (*new_word)->str, strlen((*new_word)->str)) == 0){
			free((*new_word)->str); 
			free(*new_word);
			return;
	}
	/* Otherwise, insert at the bottom of the list */
	trav->down = *new_word;
	return;
}

/* Determine the location to insert a new word struct */
void insert_word(count **head, count **tail, word **new_word){ 
	
	/* If the list is empty, create a new count struct that will contain the new word */
	if(is_empty(head, tail)){
		create_count(head, tail, new_word);//maybe switch
		return;
	}
	/* If the length of the new word equals the head, insert the word at the head struct */
	if((*head)->num_chars == (*new_word)->length){ 
		(*head)->num_words++;  
		sorted_insert(head, new_word);
		return;	
	}

	count *temp = *head;

	while(((temp->next)!=NULL) && ((temp->next->num_chars)<((*new_word)->length)))
		temp = temp->next;

	/* If at the end of the list, create a new count struct that will contain the new word */
	if(temp->next == NULL){
		create_count(head, tail, new_word);
		return;	
	}
	/* If the next count struct has length equal to the new words, length, insert the new word at the that struct */
	if(temp->next->num_chars == (*new_word)->length){ 
		temp->next->num_words++;  
		sorted_insert(&(temp->next), new_word);
		return;	
	}
	/* If the next struct in the count list doesn not have the same length as the new word, create a new count struct that will contain the new word */
	if((temp->next->num_chars)!=((*new_word)->length)){
		create_count(head, tail, new_word);
		return;	
	}
	return;
}

/* Creates a new word struct */
void create_word(count **head, count **tail, char *str){ 

	word *new_word = malloc(sizeof *new_word);
	int length = strlen(str)+1;

	/* Print an error if an error occurred malloc-ing a new word struct */
	if(new_word == NULL){
       		fprintf(stderr, "Error creating a new word node.\n");
       		exit(0);
    	}

	new_word->str = (char*)malloc(length*sizeof(str));

	/* Print an error if an error occurred malloc-ing space for the word's string */
	if(str == NULL){
       		fprintf(stderr, "Error with input string.\n");
       		exit(0);
    	}

	/* If no malloc errors, place the input string into the word and send to insert function */
	strncpy(new_word->str, str, length);
	new_word->length = length-1;
	new_word->down = NULL;
	insert_word(head, tail, &new_word);
	return;
}

/* Tokenizes an input line */
void tokenize(count **head, count **tail, char words_buffer[]){ 
	
	/* Tokenize the first string */
	char *cur = strtok(words_buffer, " .,;()\t\r\v\f\n");
	if(cur)
		create_word(head, tail, cur);

	/* Tokenize the following strings */
	while(cur){
		cur = strtok(NULL, " .,;()\t\r\v\f\n"); 
		if(cur)
			create_word(head, tail, cur);
	}
	return;
}


