#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include <stddef.h>

#define DATA_FILE "data.txt"
#define INITIAL_CAPACITY 128
#define NAME_SIZE 30
#define PHONE_SIZE 20
#define MEMO_SIZE 100

typedef struct {
    char name[NAME_SIZE];
    char phone[PHONE_SIZE];
    char memo[MEMO_SIZE];
} Contact;

typedef struct {
    Contact *items;
    size_t count;
    size_t capacity;
} PhoneBook;

typedef struct {
    int *items;
    size_t count;
    size_t capacity;
} IndexList;

void init_phonebook(PhoneBook *book);
void free_phonebook(PhoneBook *book);
int add_contact(PhoneBook *book, const char *name, const char *phone, const char *memo);
int delete_contact(PhoneBook *book, size_t index);
int load_contacts(PhoneBook *book, const char *path);
int save_contacts(const PhoneBook *book, const char *path);

void init_index_list(IndexList *list);
void free_index_list(IndexList *list);
int build_search_results(const PhoneBook *book, const char *query, IndexList *results);

void trim_spaces(char *text);

#endif
