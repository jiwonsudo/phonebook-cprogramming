# Phonebook Module

`phonebook.c`와 `phonebook.h`는 전화번호부 데이터 처리를 담당합니다.

## 담당 기능

- 전화번호부 초기화
- 동적 배열 메모리 관리
- 연락처 추가
- 연락처 삭제
- 파일 로드
- 파일 저장
- 검색 결과 생성
- 문자열 앞뒤 공백 제거

## 주요 함수

### `init_phonebook()`

```c
void init_phonebook(PhoneBook *book);
```

`PhoneBook` 구조체를 초기화하고, 초기 저장 공간을 확보합니다.
처음 `items`를 `NULL`로 두기 때문에 `realloc()`을 안전하게 사용할 수 있습니다.

### `free_phonebook()`

```c
void free_phonebook(PhoneBook *book);
```

동적으로 할당한 연락처 배열을 해제합니다.
해제 후 포인터와 개수 값을 초기화해 잘못된 접근을 줄입니다.

### `add_contact()`

```c
int add_contact(PhoneBook *book, const char *name, const char *phone, const char *memo);
```

새 연락처를 추가합니다.
공간이 부족하면 배열을 먼저 확장합니다.
문자열은 `snprintf()`로 저장하므로 필드 크기보다 긴 값은 잘려서 저장됩니다.

### `delete_contact()`

```c
int delete_contact(PhoneBook *book, size_t index);
```

지정한 인덱스의 연락처를 삭제합니다.
삭제 후 뒤쪽 연락처들을 `memmove()`로 한 칸씩 앞으로 당깁니다.

### `load_contacts()`

```c
int load_contacts(PhoneBook *book, const char *path);
```

`data.txt` 파일을 읽어 연락처를 메모리에 저장합니다.
파일이 없으면 새 전화번호부로 시작하기 위해 정상 처리합니다.

### `save_contacts()`

```c
int save_contacts(const PhoneBook *book, const char *path);
```

현재 전화번호부 내용을 파일에 저장합니다.
쓰기 모드(`"w"`)로 열기 때문에 기존 파일 내용은 새 내용으로 교체됩니다.

### `build_search_results()`

```c
int build_search_results(const PhoneBook *book, const char *query, IndexList *results);
```

이름, 전화번호, 메모 중 하나라도 검색어를 포함하면 결과에 추가합니다.
대소문자는 구분하지 않습니다.

## 내부 함수

### `ensure_phonebook_capacity()`

배열 공간이 부족한지 확인하고, 부족하면 2배씩 늘립니다.
이 함수 덕분에 최소 100개 이상 연락처 저장이 가능합니다.

### `contains_case_insensitive()`

C 표준 함수만 사용해 대소문자를 무시하는 부분 문자열 검색을 직접 구현했습니다.

### `push_index()`

검색 결과 인덱스를 `IndexList`에 추가합니다.
검색 결과 개수도 미리 알 수 없기 때문에 동적 배열로 관리합니다.

