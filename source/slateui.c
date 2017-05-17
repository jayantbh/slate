/**
 * SlateUI test code.
 * We're using this to learn how to use ncurses to build a UI.
 * Started on 12/11 by jayantbh.
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wuninitialized"
#pragma ide diagnostic ignored "CannotResolve"

/**
 * Begin Code
 */


#include <ncurses.h>
#include <panel.h>
#include <signal.h>
//#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "listfunc.h"
#include "keycodes.h"

int HEIGHT, WIDTH;
WINDOW *TITLE_BAR, *EDITOR, *MENU, *FIND_DIALOG;
PANEL *_TITLE_BAR, *_EDITOR, *_MENU, *_FIND_DIALOG;
char *filename;
struct node* NODE;
int TAB_WIDTH = 4;
const char MENU_ITEMS[] = "^C: Quit\t^W: Write\t^U: Undo\t^F: Find";

/**
 * Signal Handlers
 */

static void finish(int sig) {
    endwin();
    exit(0);
}

/**
 * Utility functions
 */

WINDOW *create_window(int height, int width, int y, int x, int color_pair) {
    WINDOW *window = newwin(height, width, y, x);
    wbkgd(window, (chtype) COLOR_PAIR(color_pair));
    refresh();
    wrefresh(window);
    return window;
}

void debug(int x, int y) {
    mvwprintw(TITLE_BAR, 0, WIDTH - 32, "Ch:%6d | %3c", NODE->data, NODE->data);
    mvwprintw(TITLE_BAR, 0, WIDTH - 18, "X:%3d ", x);
    mvwprintw(TITLE_BAR, 0, WIDTH - 12, "Y:%3d", y);
    wrefresh(TITLE_BAR);
}

// Returns the last column containing a character
int line_length(int y){
    char *str = (char *) calloc((size_t) WIDTH, sizeof(char));
    mvwinstr(EDITOR, y, 0, str);
    int i = WIDTH;

    while(str[i] == ' ' || str[i] == '\0'){
        i--;
    }
    return i;
}

int gap_before_cursor(WINDOW *window, int width, int y, int x) {
    char *str = (char *) calloc((size_t) width, sizeof(char));
    mvwinnstr(window, y, 0, str, x - 1);
    int i = (int) (strlen(str) - 1), distance = 1;
    while(i) {
        if (str[i] != ' ') {
            break;
        }
        distance++;
        i--;
    }
    return distance;
}

int characters_after_cursor(int y, int x) {
    //TODO: Handle tabs
    char *str = (char *) calloc((size_t) WIDTH - x + 1, sizeof(char));
    mvwinnstr(EDITOR, y, x, str, WIDTH - x + 1);
    int i = WIDTH - 1, distance = 0;
    while(i) {
        if (str[i] != ' ' && str[i] != '\0') {
            break;
        }
        distance++;
        i--;
    }
    return WIDTH - distance;
}

int characters_before_cursor(int y, int x) {
    //TODO: Handle tabs
    int length = line_length(y) + 1;
    if (length < x) {
        return length;
    }
    return x;
}

void print_line(char *BUFFER, int y, int x) {
    mvwprintw(EDITOR, y, 0, BUFFER);
    wmove(EDITOR, y, x);
    wclrtoeol(EDITOR);
}

void refresh_editor(int y) {
    wclear(EDITOR);
    int height = HEIGHT - 2, TOP_LIMIT = y, BOTTOM_LIMIT = y + height;
    char *BUFFER = (char *) calloc((size_t) WIDTH, sizeof(char));

    struct node* KEY = getHead(NODE)->next; // Skip one node, because the first item in the linked list is a non-printable character.
    int i = 0, line = 0;
    while (KEY != NULL)
    {
        int limit = WIDTH;
        char ch = KEY->data;

        switch (ch) {
            case '\t': limit -= TAB_WIDTH; break;
            default: break;
        }
        if (i + 1 == limit || ch == '\n') {
            if (line >= TOP_LIMIT && line < BOTTOM_LIMIT) {
                print_line(BUFFER, y + line, i);
                line++;
            }

            i = 0;
            memset(BUFFER, 0, sizeof BUFFER);
        }
        else {
            BUFFER[i] = ch;
            i++;
            if (KEY->next == NULL) {
                print_line(BUFFER, line, i);
                line++;
            }
        }
        KEY = KEY->next;
    }
}
/**
 * Initialization functions
 */

void init_title_bar() {
    TITLE_BAR = create_window(1, WIDTH, 0, 0, 2);
    wmove(TITLE_BAR, 0, 0);
    wprintw(TITLE_BAR, "Project Slate, dims:%d:%d", HEIGHT, WIDTH);
    wrefresh(TITLE_BAR);
}

void init_editor() {
    EDITOR = create_window(HEIGHT - 2, WIDTH, 1, 0, 0);
    wmove(EDITOR, 0, 0);
    wclear(EDITOR);
    wprintw(EDITOR, getContents(NODE));
    wrefresh(EDITOR);
}

void init_menu() {
    MENU = create_window(1, WIDTH, HEIGHT - 1, 0, 2);
    wmove(MENU, 0, 0);
    wprintw(MENU, MENU_ITEMS);
    wrefresh(MENU);
}

void init_find_dialog() {
    int width = (int) (WIDTH * 0.6);
    int height = 3;
    int x_pos = WIDTH/2 - width/2;
    int y_pos = HEIGHT/2 - 1;
    FIND_DIALOG = create_window(height, width, y_pos, x_pos, 1);
    wborder(FIND_DIALOG, ' ', ' ', 0, 0, ' ', ' ', ' ', ' ');
    wmove(FIND_DIALOG, 0, width/2 - 2);
    wprintw(FIND_DIALOG, "FIND");
    wmove(FIND_DIALOG, 1, 1);
    wrefresh(FIND_DIALOG);
}

void init_panels() {
    _TITLE_BAR = new_panel(TITLE_BAR);
    _EDITOR = new_panel(EDITOR);
    _MENU = new_panel(MENU);
    _FIND_DIALOG = new_panel(FIND_DIALOG);

    set_panel_userptr(_TITLE_BAR, _EDITOR);
    set_panel_userptr(_EDITOR, _MENU);
    set_panel_userptr(_MENU, _FIND_DIALOG);
    set_panel_userptr(_FIND_DIALOG, _TITLE_BAR);

    top_panel(_EDITOR);
    update_panels();
    doupdate();
}

void init_windows() {
    init_title_bar();
    init_menu();
    init_editor();
    init_find_dialog();
    init_panels();
}

void init_colors() {
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLUE);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(5, COLOR_BLUE, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
}

void init_curses_config() {
    initscr();
    cbreak();
    noecho();
    set_tabsize(TAB_WIDTH);
    refresh();      // Important to refresh screen before refresh window

    HEIGHT = LINES;
    WIDTH = COLS;
}

void init_slate() {
    init_curses_config();
    init_colors();
    init_windows();
    keypad(EDITOR, TRUE);
    keypad(FIND_DIALOG, TRUE);
    scrollok(EDITOR, TRUE);
    move(1,0);
    wmove(EDITOR, 0, 0);
}

void keystroke_handler() {
    int ch;
    int x = 0, y = 0, increment = 0, decrement = 0, shift = 0;
    int loc_x = 0, loc_y = 0;

    refresh();
    wrefresh(EDITOR);

    WINDOW* CURRENT_WINDOW = EDITOR;
    while ((ch = wgetch(CURRENT_WINDOW))) {
        wrefresh(TITLE_BAR);
        shift = 0;
        loc_x = 0;
        loc_y = 0;
        switch (ch) {
            case KEY_ESC:    // ^[ ESCAPE
                top_panel(_EDITOR);
                CURRENT_WINDOW = EDITOR;
                break;
            case KEY_UP:
                if (y - 1 < 0) {
                    y = 0;
                }
                else {
                    shift--; // 1 for newline
                    shift -= characters_before_cursor(y, x);
                    shift -= characters_after_cursor(y - 1, x);
                    NODE = moveCursor(NODE, shift);

                    y--;
                }
                break;
            case KEY_DOWN:
                if (y + 1 > HEIGHT - 3 && line_length(y + 1) > 0) {
                    y = HEIGHT - 3;
                }
                else if (line_length(y + 1) > 0) {
                    if (line_length(y + 1) < x) {
                        x = line_length(y + 1) + 1;
                    }
                    shift++; // 1 for newline
                    shift += characters_after_cursor(y, x);
                    shift += characters_before_cursor(y + 1, x);
                    NODE = moveCursor(NODE, shift);

                    y++;
                }
                break;
            case KEY_LEFT:
                //TODO: Handle Tabs
                if (x != 0) {
                    x = x - 1;
                    NODE = moveCursor(NODE, -1);
                }
                wmove(EDITOR, y, x);
                break;
            case KEY_RIGHT:
                //TODO: Handle Tabs
                if (x < line_length(y) + 1) {
                    x++;
                    NODE = moveCursor(NODE, 1);
                }
                wmove(EDITOR, y, x);
                break;
            case 21:    // CTRL + U : UNDO
                shift = undo(&NODE);
                x += shift;
                wmove(EDITOR, y, x);
                if (shift < 0) {
                    wdelch(EDITOR);
                }
                break;
            case 6:     // CTRL + F : FIND
                top_panel(_FIND_DIALOG);
                CURRENT_WINDOW = FIND_DIALOG;

                wmove(CURRENT_WINDOW, 1, 0);
                wclrtoeol(CURRENT_WINDOW);
                wmove(CURRENT_WINDOW, 1, 1);
                update_panels();
                doupdate();

                struct node* FIND = getEmptyList();
//                FIND = insertCharAfter(FIND, '\n');
                while ((ch = wgetch(CURRENT_WINDOW))) {
                    switch (ch) {
                        case KEY_LEFT:
                            //TODO: Handle Tabs
                            if (loc_x != 0) {
                                loc_x = loc_x - 1;
                                FIND = moveCursor(FIND, -1);
                            }
                            break;
                        case KEY_RIGHT:
                            //TODO: Handle Tabs
                            if (loc_x <= line_length(loc_y) + 1) {
                                loc_x++;
                                FIND = moveCursor(FIND, 1);
                            }
                            break;
                        case KEY_ENTER:    //ENTER
                            // TODO: Trigger Find
                            top_panel(_EDITOR);
                            CURRENT_WINDOW = EDITOR;
                            update_panels();
                            doupdate();
                            find(stringSlice(getContents(FIND), 1, INF), NODE);
                            break;
                        case KEY_ESC:    //ESCAPE
                            top_panel(_EDITOR);
                            CURRENT_WINDOW = EDITOR;
                            update_panels();
                            doupdate();
                            break;
                        case KEY_BKSP:   //BACKSPACE
                            switch ((int) FIND->data) {
                                case 9: decrement = gap_before_cursor(CURRENT_WINDOW, (int) (WIDTH * 0.6), loc_y, loc_x); break;   // TAB
                                default: decrement = 1;
                            }
                            if (loc_x == 0) {
                                break;
                            }
                            if(loc_x > 0){
                                loc_x -= decrement;
                                if(loc_x < 0){
                                    loc_x=0;
                                }
                            }
                            FIND = deleteChar(FIND);
                            break;
                        default:
                            switch (ch) {
                                case 9: increment = TAB_WIDTH - loc_x % TAB_WIDTH; break;   // TAB
                                default: increment = 1;
                            }
                            loc_x += increment;
                            FIND = insertCharAfter(FIND, (char) ch);
                    }

                    if (ch == KEY_ENTER || ch == KEY_ESC) {
                        debug(x, y);
                        wrefresh(CURRENT_WINDOW);
                        break;
                    }

                    wmove(CURRENT_WINDOW, 1, 0);
                    wclrtoeol(CURRENT_WINDOW);
                    char *contents = getContents(FIND);
                    for (int i = 1; i < strlen(contents); i++) {
                        mvwaddch(CURRENT_WINDOW, 1, i, contents[i]);
                    }

//                    mvwprintw(CURRENT_WINDOW, 1, 1, getContents(FIND));

                    wmove(CURRENT_WINDOW, loc_y + 1, loc_x + 1);
                    wrefresh(CURRENT_WINDOW);
                }
                break;
            case 11:    // CTRL + K : WRITE
            case 12:    // CTRL + L : WRITE
            case 23:    // CTRL + W : WRITE
                writeBackToFile(NODE, filename); break;
            case KEY_BKSP:   //BACKSPACE
                switch ((int) NODE->data) {
                    case 9: decrement = gap_before_cursor(CURRENT_WINDOW, WIDTH, y, x); break;   // TAB
                    default: decrement = 1;
                }
                if (x == 0 && y == 0) {
                    break;
                }
                if(x == 0){
                    y--;
                    if(y < 0){
                        y=0;
                    }

                    x = line_length(y) + 1;
                }
                else if(x > 0){
                    x -= decrement;
                    if(x < 0){
                        x=0;
                    }
                }
                NODE = deleteChar(NODE);
                break;
            case KEY_ENTER:    //ENTER
                NODE = insertCharAfter(NODE, (char) ch);
                x=0; y++;
                break;
            default:
                switch (ch) {
                    case 9: increment = TAB_WIDTH - x % TAB_WIDTH; break;   // TAB
                    default: increment = 1;
                }
                if(x + increment > WIDTH-1){
                    y++;
                    x=0;
                }
                x += increment;
                //waddch(editor, (char) ch);
                NODE = insertCharAfter(NODE, (char) ch);
        }
        refresh_editor(0);
        wmove(EDITOR, y, x);

        debug(x, y);
        wrefresh(CURRENT_WINDOW);   // This needs to be the last line in the loop

        // TODO: We might not need to do this here.
        update_panels();
        doupdate();
    }
}

int main(int argc, char *argv[]) {
    signal(SIGINT, finish);

    filename = argv[1];
    NODE = loadFileToList(filename);

    init_slate();
    keystroke_handler();

    finish(0);
    return 0;
}

/**
 * End Code
 */

#pragma clang diagnostic pop
