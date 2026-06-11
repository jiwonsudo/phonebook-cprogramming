# Data Model

이 문서는 프로그램에서 사용하는 주요 구조체를 설명합니다.

## Contact

`Contact`는 연락처 한 명의 정보를 저장합니다.

```c
typedef struct {
    char name[NAME_SIZE];
    char phone[PHONE_SIZE];
    char memo[MEMO_SIZE];
} Contact;
```

각 필드 크기는 `phonebook.h`에 상수로 정의되어 있습니다.

```c
#define NAME_SIZE 30
#define PHONE_SIZE 20
#define MEMO_SIZE 100
```

## PhoneBook

`PhoneBook`은 전체 연락처 목록을 저장합니다.

```c
typedef struct {
    Contact *items;
    size_t count;
    size_t capacity;
} PhoneBook;
```

필드 의미는 다음과 같습니다.

- `items`: `Contact` 배열을 가리키는 포인터
- `count`: 현재 저장된 연락처 개수
- `capacity`: 현재 할당된 배열 크기

## 동적 배열 방식

처음에는 `INITIAL_CAPACITY`만큼 공간을 확보합니다.

```c
#define INITIAL_CAPACITY 128
```

연락처를 추가할 때 공간이 부족하면 `ensure_phonebook_capacity()`에서 배열 크기를 2배씩 늘립니다.
그래서 연락처 개수를 100개로 고정하지 않고, 메모리가 허용하는 범위에서 계속 저장할 수 있습니다.

## IndexList

`IndexList`는 검색 결과를 저장할 때 사용합니다.

```c
typedef struct {
    int *items;
    size_t count;
    size_t capacity;
} IndexList;
```

검색 결과에 연락처 전체를 복사하지 않고, `PhoneBook.items` 배열의 인덱스만 저장합니다.
예를 들어 검색 결과 첫 번째가 원본 배열의 3번 연락처라면 `items[0] = 3`처럼 저장합니다.

이 방식의 장점은 다음과 같습니다.

- 검색 결과 저장 메모리가 작음
- 원본 연락처 번호를 그대로 표시하기 쉬움
- 연락처 구조체를 불필요하게 복사하지 않음

