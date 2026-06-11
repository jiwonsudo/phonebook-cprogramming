#include "phonebook.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int ensure_phonebook_capacity(PhoneBook *book, size_t needed);
static void trim_newline(char *text);
static int contains_case_insensitive(const char *text, const char *pattern);
static int push_index(IndexList *list, int value);

void init_phonebook(PhoneBook *book)
{
    // realloc을 바로 쓸 수 있게 NULL과 0으로 초기화함.
    book->items = NULL;
    book->count = 0;
    book->capacity = 0;

    if (ensure_phonebook_capacity(book, INITIAL_CAPACITY) != 0) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }
}

void free_phonebook(PhoneBook *book)
{
    free(book->items);
    book->items = NULL;
    book->count = 0;
    book->capacity = 0;
}

static int ensure_phonebook_capacity(PhoneBook *book, size_t needed)
{
    Contact *new_items;
    size_t new_capacity;

    if (book->capacity >= needed) {
        return 0;
    }

    // 공간 부족하면 2배씩 키움. 100개 제한 없이 계속 저장 가능함.
    new_capacity = book->capacity ? book->capacity : INITIAL_CAPACITY;
    while (new_capacity < needed) {
        new_capacity *= 2;
    }

    new_items = realloc(book->items, new_capacity * sizeof(Contact));
    if (new_items == NULL) {
        return -1;
    }

    book->items = new_items;
    book->capacity = new_capacity;
    return 0;
}

int add_contact(PhoneBook *book, const char *name, const char *phone, const char *memo)
{
    Contact *contact;

    // 새 연락처 1개가 들어갈 공간을 먼저 확보함.
    if (ensure_phonebook_capacity(book, book->count + 1) != 0) {
        return -1;
    }

    contact = &book->items[book->count++];
    // 배열 크기보다 긴 입력은 잘라서 저장함.
    snprintf(contact->name, sizeof(contact->name), "%s", name);
    snprintf(contact->phone, sizeof(contact->phone), "%s", phone);
    snprintf(contact->memo, sizeof(contact->memo), "%s", memo);
    return 0;
}

int delete_contact(PhoneBook *book, size_t index)
{
    if (index >= book->count) {
        return -1;
    }

    // 삭제한 칸 뒤쪽 자료를 한 칸씩 앞으로 당김.
    if (index + 1 < book->count) {
        memmove(&book->items[index], &book->items[index + 1],
                (book->count - index - 1) * sizeof(Contact));
    }
    book->count--;
    return 0;
}

int load_contacts(PhoneBook *book, const char *path)
{
    FILE *file;
    char line[512];

    file = fopen(path, "r");
    if (file == NULL) {
        // data.txt가 없으면 새 전화번호부로 시작하게 에러 처리 안 함.
        return errno == ENOENT ? 0 : -1;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char *name;
        char *phone;
        char *memo;
        char *first_sep;
        char *second_sep;

        trim_newline(line);
        if (line[0] == '\0') {
            continue;
        }

        // strtok 안 씀. 마지막 memo가 비어 있어도 "Kim:010:" 형태를 살려야 함.
        first_sep = strchr(line, ':');
        if (first_sep == NULL) {
            continue;
        }
        *first_sep = '\0';

        second_sep = strchr(first_sep + 1, ':');
        if (second_sep == NULL) {
            continue;
        }
        *second_sep = '\0';

        name = line;
        phone = first_sep + 1;
        memo = second_sep + 1;
        // 파일에 공백이 섞여 있어도 화면에는 정리된 값으로 보여줌.
        trim_spaces(name);
        trim_spaces(phone);
        trim_spaces(memo);
        add_contact(book, name, phone, memo);
    }

    fclose(file);
    return 0;
}

int save_contacts(const PhoneBook *book, const char *path)
{
    FILE *file;
    size_t i;

    // 전체 내용을 새로 씀. 삭제된 연락처가 파일에 남지 않게 하기 위함.
    file = fopen(path, "w");
    if (file == NULL) {
        return -1;
    }

    for (i = 0; i < book->count; i++) {
        fprintf(file, "%s:%s:%s\n", book->items[i].name, book->items[i].phone,
                book->items[i].memo);
    }

    if (fclose(file) != 0) {
        return -1;
    }
    return 0;
}

static void trim_newline(char *text)
{
    size_t len = strlen(text);

    while (len > 0 && (text[len - 1] == '\n' || text[len - 1] == '\r')) {
        text[--len] = '\0';
    }
}

void trim_spaces(char *text)
{
    char *start = text;
    char *end;

    // 앞쪽 공백 위치를 찾음.
    while (isspace((unsigned char)*start)) {
        start++;
    }

    // 앞 공백이 있으면 문자열 전체를 앞으로 당김.
    if (start != text) {
        memmove(text, start, strlen(start) + 1);
    }

    end = text + strlen(text);
    // 뒤쪽 공백은 끝 문자부터 '\0'로 바꿔 제거함.
    while (end > text && isspace((unsigned char)*(end - 1))) {
        *(--end) = '\0';
    }
}

static int contains_case_insensitive(const char *text, const char *pattern)
{
    size_t pattern_len = strlen(pattern);
    size_t i;

    if (pattern_len == 0) {
        return 1;
    }

    // C 기본 문자열 함수에 대소문자 무시 부분검색이 없어 직접 비교함.
    for (i = 0; text[i] != '\0'; i++) {
        size_t j = 0;
        while (pattern[j] != '\0' && text[i + j] != '\0' &&
               tolower((unsigned char)text[i + j]) == tolower((unsigned char)pattern[j])) {
            j++;
        }
        if (j == pattern_len) {
            return 1;
        }
    }
    return 0;
}

void init_index_list(IndexList *list)
{
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

void free_index_list(IndexList *list)
{
    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

static int push_index(IndexList *list, int value)
{
    int *new_items;
    size_t new_capacity;

    // 검색 결과 개수도 미리 알 수 없으므로 동적 배열로 관리함.
    if (list->count == list->capacity) {
        new_capacity = list->capacity ? list->capacity * 2 : 32;
        new_items = realloc(list->items, new_capacity * sizeof(int));
        if (new_items == NULL) {
            return -1;
        }
        list->items = new_items;
        list->capacity = new_capacity;
    }
    list->items[list->count++] = value;
    return 0;
}

int build_search_results(const PhoneBook *book, const char *query, IndexList *results)
{
    size_t i;

    results->count = 0;
    for (i = 0; i < book->count; i++) {
        const Contact *contact = &book->items[i];
        if (contains_case_insensitive(contact->name, query) ||
            contains_case_insensitive(contact->phone, query) ||
            contains_case_insensitive(contact->memo, query)) {
            // 검색 결과는 복사하지 않고 원본 배열의 인덱스만 저장함.
            if (push_index(results, (int)i) != 0) {
                return -1;
            }
        }
    }
    return 0;
}
