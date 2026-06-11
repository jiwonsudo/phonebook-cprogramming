#include "phonebook.h"
#include "ui.h"

#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    PhoneBook book;
    int loaded;

    // 한글 데이터 출력 가능하게 현재 시스템 locale 사용함.
    setlocale(LC_ALL, "");
    init_phonebook(&book);
    loaded = load_contacts(&book, DATA_FILE);

    // UI 안에서 추가/삭제하면 즉시 저장하고, 종료 시 한 번 더 저장함.
    run_phonebook_ui(&book, loaded);

    if (save_contacts(&book, DATA_FILE) != 0) {
        fprintf(stderr, "Failed to save %s: %s\n", DATA_FILE, strerror(errno));
        free_phonebook(&book);
        return 1;
    }

    free_phonebook(&book);
    return 0;
}
