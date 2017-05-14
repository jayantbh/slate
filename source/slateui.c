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


//#include <curses.h>
#include <ncurses.h>
//#include <panel.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "listfunc.h"

int HEIGHT, WIDTH;
WINDOW *TITLE_BAR, *EDITOR, *MENU;
char *filename;
struct node* NODE;
int TAB_WIDTH = 4;
const char MENU_ITEMS[] = "^C: Quit\t^W: Write\t^U: Undo";
/**
 * Signal Handlers
 */

static void finish(int sig) {
    endwin();

    // Clear up any other data structures or windows.

    exit(0);
}

/**
 * Utility functions
 */

WINDOW *create_window(int height, int width, int y, int x, int color_pair) {
    WINDOW *window = newwin(height, width, y, x);
    wbkgd(window, COLOR_PAIR(color_pair));
    refresh();
    wrefresh(window);
    return window;
}

int line_length(int y){
    char str[400] = {'\0'};
    mvwinstr(EDITOR, y, 0, &str);
    int i = WIDTH;

    while(str[i] == ' ' || str[i] == '\0'){
        i--;
    }
    return i;
}

int distance_from_char_to_cursor(int y, int x) {
    char str[400] = {'\0'};
    mvwinnstr(EDITOR, y, 0, &str, x - 1);
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

int special_key(){
    int ch;
    ch = getch();	//get [
    mvwprintw(TITLE_BAR, 20, 1, "Detected character 1 is = %c ", (char)ch);
    //printf("Detected character 1 is = %c ", (char)ch);
    ch = getch();
    //printf("Detected character 2 is = %c ", (char)ch);

    switch (ch) {
        case 65:    //A, escaped KEY_UP
            return (int) KEY_UP;
        case 66:    //B, escaped KEY_DOWN
            return (int) KEY_DOWN;
        case 67:    //C, escaped KEY_RIGHT
            return (int) KEY_RIGHT;
        case 68:    //D, escaped KEY_LEFT
            return (int) KEY_LEFT;
        default: break;
    }
    return 0;
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
    wprintw(EDITOR, getFileContents(NODE));
    wrefresh(EDITOR);
}

void init_menu() {
    MENU = create_window(1, WIDTH, HEIGHT - 1, 0, 2);
    wmove(MENU, 0, 0);
    wprintw(MENU, MENU_ITEMS);
    wrefresh(MENU);
}

void init_windows() {
    init_title_bar();
    init_menu();
    init_editor();
}

void init_colors() {
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLUE);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);
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
    scrollok(EDITOR, TRUE);
    move(1,0);
    wmove(EDITOR, 0, 0);
}

void keystroke_handler() {
    int ch;
    int x = 0, y = 0, increment = 0, decrement = 0, shift = 0;

    refresh();
    wrefresh(EDITOR);

    while ((ch = getch())) {
        wrefresh(TITLE_BAR);
        shift = 0;
        switch (ch) {
            case 27:    //^[ ESCAPE SEQUENCE BEGINS
                switch (special_key()){
                    case KEY_UP:
                        wscrl(EDITOR, -1);
//                        if (y == 0) {
//                            wscrl(EDITOR, 1);
//                        }
                        y = (y - 1 < 0) ? 0 : y - 1;
                        wmove(EDITOR, y, x);
                        break;
                    case KEY_DOWN:
                        if (y == HEIGHT - 2) {
                            wscrl(EDITOR, -1);
                        }
                        y = (y + 1 > HEIGHT - 3 && line_length(y + 1) > 0) ? HEIGHT - 3 : y + 1;
                        wmove(EDITOR, y, x);
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
                    default:
                        break;
                }
                break;
            case 21:    // CTRL + U : UNDO
                shift = undo(&NODE);
                x += shift;
                wmove(EDITOR, y, x);
                if (shift < 0) {
                    wdelch(EDITOR);
                }
                break;
            case 23:    // CTRL + W : WRITE
                writeBackToFile(NODE, filename); break;
            case 127:   //BACKSPACE
                switch ((int) NODE->data) {
                    case 9: decrement = distance_from_char_to_cursor(y, x); break;   // TAB
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
                    wmove(EDITOR, y, x);
                    wdelch(EDITOR);
                    NODE = deleteChar(NODE);
                }
                else if(x > 0){
                    x -= decrement;
                    if(x < 0){
                        x=0;
                    }
                    wmove(EDITOR, y, x);
                    wdelch(EDITOR);
                    NODE = deleteChar(NODE);
                }
                break;
            case 10:    //ENTER
                //waddch(editor, (char) ch);
                NODE = insertCharAfter(NODE, (char) ch);
                mvwprintw(EDITOR, 0, 0, getFileContents(NODE));
                getyx(EDITOR, y, x);
                x=0;
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
        mvwprintw(EDITOR, 0, 0, getFileContents(NODE));
        wmove(EDITOR, y, x);

        //TODO: Debug stuff
        mvwprintw(TITLE_BAR, 0, WIDTH - 32, "Ch:%6d | %3c", NODE->data, NODE->data);
        mvwprintw(TITLE_BAR, 0, WIDTH - 18, "X:%3d ", x);
        mvwprintw(TITLE_BAR, 0, WIDTH - 12, "Y:%3d", y);
        wrefresh(TITLE_BAR);
        //DEBUG Stuff ends

        wrefresh(EDITOR);   // This needs to be the last line in the loop
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
