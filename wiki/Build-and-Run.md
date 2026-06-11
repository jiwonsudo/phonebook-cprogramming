# Build and Run

이 문서는 프로그램 컴파일과 실행 방법을 설명합니다.

## 필요 환경

- C 컴파일러
- NCURSES 라이브러리

macOS나 Linux 환경에서는 보통 `cc`와 `ncurses`를 사용할 수 있습니다.

## 컴파일

프로젝트 폴더에서 아래 명령을 실행합니다.

```sh
cc -Wall -Wextra -pedantic -std=c11 -o tel tel.c phonebook.c ui.c -lncurses
```

이 명령은 다음 소스 파일들을 함께 컴파일한 뒤 `tel` 실행 파일을 만듭니다.

- `tel.c`
- `phonebook.c`
- `ui.c`

NCURSES 사용을 위해 마지막에 `-lncurses` 옵션을 붙입니다.

## 실행

컴파일 후 아래 명령으로 실행합니다.

```sh
./tel
```

실행하면 NCURSES 기반 메뉴가 열립니다.

소스 파일을 모듈별로 나누었지만, 제출용으로는 위 한 줄 명령으로 바로 빌드할 수 있습니다.
