# Program Flow

이 문서는 프로그램 실행 흐름을 설명합니다.

## 전체 흐름

```txt
main()
 ├─ setlocale()
 ├─ init_phonebook()
 ├─ load_contacts()
 ├─ run_phonebook_ui()
 │   ├─ initscr()
 │   ├─ main_menu()
 │   │   ├─ search_contacts()
 │   │   ├─ add_contact_screen()
 │   │   ├─ delete_contact_screen()
 │   │   └─ list_contacts()
 │   └─ endwin()
 ├─ save_contacts()
 └─ free_phonebook()
```

## 1. 프로그램 시작

`main()`에서 `setlocale(LC_ALL, "")`를 호출합니다.
이 설정은 한글 데이터가 터미널에서 최대한 정상 표시되도록 하기 위해 사용합니다.

## 2. 전화번호부 초기화

`init_phonebook()`을 호출해 `PhoneBook` 구조체를 초기화합니다.
초기 배열 공간은 `INITIAL_CAPACITY` 값인 128개입니다.

## 3. 파일 로드

`load_contacts()`가 `data.txt`를 읽습니다.
파일이 없으면 에러로 종료하지 않고 빈 전화번호부로 시작합니다.

## 4. UI 실행

`run_phonebook_ui()`가 NCURSES 화면을 초기화하고 메인 메뉴를 실행합니다.
사용자는 메뉴에서 검색, 추가, 삭제, 목록 보기를 수행합니다.

## 5. 데이터 저장

추가와 삭제 직후에도 `data.txt`에 저장합니다.
그리고 프로그램 종료 시 `main()`에서 한 번 더 `save_contacts()`를 호출합니다.

이렇게 하면 실행 중 변경 사항이 파일에 반영되고, 종료 시점의 최종 상태도 저장됩니다.

## 6. 메모리 해제

마지막으로 `free_phonebook()`을 호출해 동적 할당한 배열을 해제합니다.

