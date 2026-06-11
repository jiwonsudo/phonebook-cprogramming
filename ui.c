#include "ui.h"

#include <curses.h>
#include <string.h>

static void draw_header(const char *title);
static void wait_message(const char *message);
static void status_line(const char *message);
static int prompt_text(const char *title, const char *label, char *buffer, size_t size, int required);
static void draw_contact_row(int y, int width, int selected, size_t number, const Contact *contact);
static void list_contacts(const PhoneBook *book);
static void search_contacts(const PhoneBook *book);
static void add_contact_screen(PhoneBook *book);
static void delete_contact_screen(PhoneBook *book);
static void main_menu(PhoneBook *book);

void run_phonebook_ui(PhoneBook *book, int loaded)
{
    // NCURSES 기본 설정. 키 입력을 바로 받고 화면 직접 제어 가능하게 함.
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (has_colors()) {
        // 색상 번호를 미리 등록해 두고 화면마다 재사용함.
        start_color();
        use_default_colors();
        init_pair(1, COLOR_CYAN, -1);
        init_pair(2, COLOR_BLACK, COLOR_CYAN);
        init_pair(3, COLOR_YELLOW, -1);
    }

    if (loaded != 0) {
        wait_message("data.txt could not be loaded. A new phonebook will be created.");
    }

    main_menu(book);
    endwin();
}

static void draw_header(const char *title)
{
    // 화면 전체를 지우고 공통 제목 영역을 다시 그림.
    erase();
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(0, 2, "Phonebook Manager");
    attroff(COLOR_PAIR(1) | A_BOLD);
    mvhline(1, 0, ACS_HLINE, COLS);
    attron(A_BOLD);
    mvprintw(2, 2, "%s", title);
    attroff(A_BOLD);
}

static void wait_message(const char *message)
{
    int y = LINES - 2;

    attron(COLOR_PAIR(3));
    mvprintw(y, 2, "%s", message);
    attroff(COLOR_PAIR(3));
    mvprintw(y + 1, 2, "Press any key to continue.");
    refresh();
    getch();
}

static void status_line(const char *message)
{
    move(LINES - 1, 0);
    clrtoeol();
    attron(COLOR_PAIR(3));
    mvprintw(LINES - 1, 2, "%s", message);
    attroff(COLOR_PAIR(3));
    refresh();
}

static int prompt_text(const char *title, const char *label, char *buffer, size_t size, int required)
{
    int ch;
    int result = 0;

    echo();
    curs_set(1);
    while (1) {
        draw_header(title);
        mvprintw(5, 2, "%s", label);
        mvprintw(7, 2, "> ");
        clrtoeol();
        move(7, 4);
        refresh();

        // 이전 입력 찌꺼기가 남지 않게 버퍼를 매번 비움.
        memset(buffer, 0, size);
        if (getnstr(buffer, (int)size - 1) == ERR) {
            result = -1;
            break;
        }
        trim_spaces(buffer);

        // 필수 입력이 아니거나, 값이 있으면 입력 완료로 봄.
        if (!required || buffer[0] != '\0') {
            break;
        }

        // 입력 중에는 echo 상태라 안내문 보여주기 전에 잠깐 끔.
        noecho();
        curs_set(0);
        status_line("This field is required. Press any key.");
        ch = getch();
        (void)ch;
        echo();
        curs_set(1);
    }
    noecho();
    curs_set(0);
    return result;
}

static void draw_contact_row(int y, int width, int selected, size_t number, const Contact *contact)
{
    int memo_width = width - 56;

    // 작은 터미널에서도 printf 폭이 음수가 되지 않게 막음.
    if (memo_width < 8) {
        memo_width = 8;
    }

    if (selected) {
        attron(COLOR_PAIR(2) | A_BOLD);
    }
    mvprintw(y, 2, "%3zu  %-28.28s %-18.18s %-*.*s", number, contact->name,
             contact->phone, memo_width, memo_width, contact->memo);
    if (selected) {
        attroff(COLOR_PAIR(2) | A_BOLD);
    }
}

static void list_contacts(const PhoneBook *book)
{
    int selected = 0;
    int top = 0;
    int ch;

    while (1) {
        // 제목, 표 헤더, 안내줄을 제외한 영역만 목록 출력에 사용함.
        int visible = LINES - 8;
        int i;

        draw_header("List");
        mvprintw(4, 2, "Total: %zu", book->count);
        attron(A_BOLD);
        mvprintw(5, 2, " No  Name                         Phone              Memo");
        attroff(A_BOLD);

        if (book->count == 0) {
            mvprintw(8, 2, "No contacts. Add a new entry from the main menu.");
        } else {
            // 선택 줄이 화면 밖으로 나가면 top을 움직여 스크롤처럼 보이게 함.
            if (selected < top) {
                top = selected;
            }
            if (selected >= top + visible) {
                top = selected - visible + 1;
            }
            for (i = 0; i < visible && (size_t)(top + i) < book->count; i++) {
                draw_contact_row(6 + i, COLS, top + i == selected,
                                 (size_t)top + i + 1, &book->items[top + i]);
            }
        }

        mvprintw(LINES - 2, 2, "UP/DOWN scroll  HOME/END jump  q/ESC back");
        refresh();

        ch = getch();
        // 방향키와 vi식 j/k 이동을 같이 지원함.
        if (ch == 'q' || ch == 'Q' || ch == 27) {
            return;
        } else if ((ch == KEY_UP || ch == 'k') && selected > 0) {
            selected--;
        } else if ((ch == KEY_DOWN || ch == 'j') && (size_t)selected + 1 < book->count) {
            selected++;
        } else if (ch == KEY_HOME) {
            selected = 0;
        } else if (ch == KEY_END && book->count > 0) {
            selected = (int)book->count - 1;
        }
    }
}

static void search_contacts(const PhoneBook *book)
{
    char query[MEMO_SIZE];
    IndexList results;
    int selected = 0;
    int top = 0;
    int ch;

    if (prompt_text("Search", "Search by name, phone, or memo", query, sizeof(query), 1) != 0) {
        return;
    }

    init_index_list(&results);
    if (build_search_results(book, query, &results) != 0) {
        free_index_list(&results);
        wait_message("Search failed because memory allocation failed.");
        return;
    }

    while (1) {
        // 검색 화면은 query, match 줄이 더 있어서 list보다 한 줄 적게 보여줌.
        int visible = LINES - 9;
        int i;

        draw_header("Search Results");
        mvprintw(4, 2, "Query: %s", query);
        mvprintw(5, 2, "Matches: %zu", results.count);
        attron(A_BOLD);
        mvprintw(6, 2, " No  Name                         Phone              Memo");
        attroff(A_BOLD);

        if (results.count == 0) {
            mvprintw(9, 2, "No matching contact was found.");
        } else {
            if (selected < top) {
                top = selected;
            }
            if (selected >= top + visible) {
                top = selected - visible + 1;
            }
            for (i = 0; i < visible && (size_t)(top + i) < results.count; i++) {
                // results에는 원본 연락처 번호만 들어 있으므로 다시 book에서 꺼냄.
                int contact_index = results.items[top + i];
                draw_contact_row(7 + i, COLS, top + i == selected,
                                 (size_t)contact_index + 1, &book->items[contact_index]);
            }
        }

        mvprintw(LINES - 2, 2, "UP/DOWN scroll  n new search  q/ESC back");
        refresh();

        ch = getch();
        if (ch == 'q' || ch == 'Q' || ch == 27) {
            break;
        } else if (ch == 'n' || ch == 'N') {
            free_index_list(&results);
            search_contacts(book);
            return;
        } else if ((ch == KEY_UP || ch == 'k') && selected > 0) {
            selected--;
        } else if ((ch == KEY_DOWN || ch == 'j') && (size_t)selected + 1 < results.count) {
            selected++;
        }
    }
    free_index_list(&results);
}

static void add_contact_screen(PhoneBook *book)
{
    char name[NAME_SIZE];
    char phone[PHONE_SIZE];
    char memo[MEMO_SIZE];

    if (prompt_text("Add", "Name", name, sizeof(name), 1) != 0) {
        return;
    }
    if (strchr(name, ':') != NULL) {
        // 콜론은 파일 구분자라 허용하면 저장 형식이 깨짐.
        wait_message("Colon ':' cannot be used in fields.");
        return;
    }

    if (prompt_text("Add", "Phone", phone, sizeof(phone), 1) != 0) {
        return;
    }
    if (strchr(phone, ':') != NULL) {
        wait_message("Colon ':' cannot be used in fields.");
        return;
    }

    if (prompt_text("Add", "Memo", memo, sizeof(memo), 0) != 0) {
        return;
    }
    if (strchr(memo, ':') != NULL) {
        wait_message("Colon ':' cannot be used in fields.");
        return;
    }

    if (add_contact(book, name, phone, memo) != 0) {
        wait_message("Could not add contact because memory allocation failed.");
        return;
    }

    // 추가 직후 저장해서 비정상 종료 때 데이터 손실을 줄임.
    if (save_contacts(book, DATA_FILE) != 0) {
        wait_message("Contact was added, but saving data.txt failed.");
    } else {
        wait_message("Contact added and saved.");
    }
}

static void delete_contact_screen(PhoneBook *book)
{
    int selected = 0;
    int top = 0;
    int ch;

    while (1) {
        int visible = LINES - 8;
        int i;

        draw_header("Delete");
        mvprintw(4, 2, "Select a contact and press ENTER or d.");
        attron(A_BOLD);
        mvprintw(5, 2, " No  Name                         Phone              Memo");
        attroff(A_BOLD);

        if (book->count == 0) {
            mvprintw(8, 2, "No contacts to delete.");
        } else {
            if (selected < top) {
                top = selected;
            }
            if (selected >= top + visible) {
                top = selected - visible + 1;
            }
            for (i = 0; i < visible && (size_t)(top + i) < book->count; i++) {
                draw_contact_row(6 + i, COLS, top + i == selected,
                                 (size_t)top + i + 1, &book->items[top + i]);
            }
        }

        mvprintw(LINES - 2, 2, "ENTER/d delete  UP/DOWN scroll  q/ESC back");
        refresh();

        ch = getch();
        if (ch == 'q' || ch == 'Q' || ch == 27) {
            return;
        } else if ((ch == KEY_UP || ch == 'k') && selected > 0) {
            selected--;
        } else if ((ch == KEY_DOWN || ch == 'j') && (size_t)selected + 1 < book->count) {
            selected++;
        } else if ((ch == '\n' || ch == KEY_ENTER || ch == 'd' || ch == 'D') && book->count > 0) {
            // 실수 삭제 방지용 확인 화면임.
            draw_header("Delete");
            mvprintw(5, 2, "Delete this contact?");
            draw_contact_row(7, COLS, 0, (size_t)selected + 1, &book->items[selected]);
            mvprintw(10, 2, "Press y to confirm, any other key to cancel.");
            refresh();
            ch = getch();
            if (ch == 'y' || ch == 'Y') {
                delete_contact(book, (size_t)selected);
                // 마지막 항목 삭제 후 선택 위치가 범위를 넘지 않게 조정함.
                if ((size_t)selected >= book->count && selected > 0) {
                    selected--;
                }
                if (save_contacts(book, DATA_FILE) != 0) {
                    wait_message("Contact was deleted, but saving data.txt failed.");
                } else {
                    wait_message("Contact deleted and saved.");
                }
            }
        }
    }
}

static void main_menu(PhoneBook *book)
{
    const char *menus[] = {"Search", "Add", "Delete", "List", "Save and Exit"};
    const int menu_count = (int)(sizeof(menus) / sizeof(menus[0]));
    int selected = 0;
    int ch;

    while (1) {
        int i;

        draw_header("Main Menu");
        mvprintw(4, 2, "Data file: %s", DATA_FILE);
        mvprintw(5, 2, "Contacts: %zu", book->count);

        for (i = 0; i < menu_count; i++) {
            if (i == selected) {
                attron(COLOR_PAIR(2) | A_BOLD);
            }
            mvprintw(8 + i * 2, 6, "%d. %s", i + 1, menus[i]);
            if (i == selected) {
                attroff(COLOR_PAIR(2) | A_BOLD);
            }
        }

        mvprintw(LINES - 2, 2, "UP/DOWN choose  ENTER run  1-5 shortcut  q save and exit");
        refresh();

        ch = getch();
        if (ch == KEY_UP && selected > 0) {
            selected--;
        } else if (ch == KEY_DOWN && selected + 1 < menu_count) {
            selected++;
        } else if (ch >= '1' && ch <= '5') {
            // 숫자 단축키도 ENTER 선택과 같은 흐름으로 처리함.
            selected = ch - '1';
            ch = '\n';
        } else if (ch == 'q' || ch == 'Q' || ch == 27) {
            selected = 4;
            ch = '\n';
        }

        if (ch == '\n' || ch == KEY_ENTER) {
            if (selected == 0) {
                search_contacts(book);
            } else if (selected == 1) {
                add_contact_screen(book);
            } else if (selected == 2) {
                delete_contact_screen(book);
            } else if (selected == 3) {
                list_contacts(book);
            } else if (selected == 4) {
                return;
            }
        }
    }
}
