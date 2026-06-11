# Phonebook Manager Wiki

이 문서는 전화번호부 관리 프로그램 `tel`의 코드 구조와 동작 방식을 설명하는 wiki입니다.
프로그램은 C 언어와 NCURSES 라이브러리를 사용하며, `data.txt` 파일에 저장된 전화번호 데이터를 읽고 수정합니다.

## 주요 기능

- 전화번호 검색
- 새 연락처 추가
- 기존 연락처 삭제
- 전체 연락처 목록 보기
- `data.txt` 파일 로드 및 저장
- NCURSES 기반 메뉴 UI

## 문서 목록

- [Build and Run](Build-and-Run.md): 컴파일, 실행, 정리 방법
- [Project Structure](Project-Structure.md): 파일별 역할
- [Data Model](Data-Model.md): 구조체와 동적 배열 설명
- [File Format](File-Format.md): `data.txt` 저장 형식
- [Phonebook Module](Phonebook-Module.md): `phonebook.c/h` 설명
- [UI Module](UI-Module.md): `ui.c/h` 설명
- [Program Flow](Program-Flow.md): 프로그램 전체 실행 흐름

## 전체 구성 요약

프로그램은 크게 세 부분으로 나뉩니다.

1. `tel.c`
   프로그램 시작점입니다. 데이터 로드, UI 실행, 종료 시 저장을 담당합니다.

2. `phonebook.c`, `phonebook.h`
   연락처 자료구조, 동적 배열, 파일 입출력, 검색 기능을 담당합니다.

3. `ui.c`, `ui.h`
   NCURSES 화면 출력, 메뉴 이동, 사용자 입력 처리를 담당합니다.

## 실행 파일

과제 요구사항에 맞게 실행 파일 이름은 `tel`입니다.

```sh
./tel
```

