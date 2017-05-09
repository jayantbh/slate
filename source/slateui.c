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
WINDOW *title_bar, *editor, *menu;
char *filename;
struct node* NODE;

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
    mvwinstr(editor, y, 0, &str);
    int i = WIDTH;

    while(str[i] == ' ' || str[i] == '\0'){
        i--;
    }
    return i;
}

int special_key(){
    int ch;
    ch = getch();	//get [
    mvwprintw(title_bar, 20, 1, "Detected character 1 is = %c ", (char)ch);
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
    title_bar = create_window(1, WIDTH, 0, 0, 2);
    wmove(title_bar, 0, 0);
    wprintw(title_bar, "Project Slate, dims:%d:%d", HEIGHT, WIDTH);
    wrefresh(title_bar);
}

void init_editor() {
    editor = create_window(HEIGHT - 2, WIDTH, 1, 0, 0);
    wmove(editor, 0, 0);
    wprintw(editor, getFileContents(NODE));
    wrefresh(editor);
}

void init_menu() {
    menu = create_window(1, WIDTH, HEIGHT - 1, 0, 2);
    wmove(menu, 0, 0);
    wprintw(menu, "^C: Quit\t^W: Write");
    wrefresh(menu);
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
    refresh();      // Important to refresh screen before refresh window

    HEIGHT = LINES;
    WIDTH = COLS;
}

void init_slate() {
    init_curses_config();
    init_colors();
    init_windows();
    keypad(editor, TRUE);
    scrollok(editor, TRUE);
    move(1,0);
    wmove(editor, 0, 0);
}

void keystroke_handler() {
    int ch;
    int x = 0, y = 0, increment = 0, decrement = 0;

    refresh();
    wrefresh(editor);

    while ((ch = getch())) {
        wrefresh(title_bar);
        switch (ch) {
            case 27:    //^[ ESCAPE SEQUENCE BEGINS
                switch (special_key()){
                    case KEY_UP:
                        y = (y - 1 < 0) ? 0 : y - 1;
                        wmove(editor, y, x);
                        break;
                    case KEY_DOWN:
                        y = (y + 1 > HEIGHT - 3 && line_length(y + 1) > 0) ? HEIGHT - 3 : y + 1;
                        wmove(editor, y, x);
                        break;
                    case KEY_LEFT:
                        if (x != 0) {
                            x = x - 1;
                            NODE = moveCursor(NODE, -1);
                        }
                        wmove(editor, y, x);
                        break;
                    case KEY_RIGHT:
                        if (x + 1 > line_length(y)) {
                            x = line_length(y);
                        }
                        else {
                            x++;
                            NODE = moveCursor(NODE, 1);
                        }
                        wmove(editor, y, x);
                        break;
                    default:
                        break;
                }
                break;
            case 23: // ALT + S
                writeBackToFile(NODE, filename);
                break;
            case 127:   //BACKSPACE
                switch ((int) NODE->data) {
                    case 9: decrement = 8; break;   // TAB
                    default: decrement = 1;
                }
                if(x == 0){
                    y--;
                    if(y < 0){
                        y=0;
                    }

                    x = line_length(y)+1;
                }
                if(y > 0 || x > 0){
                    x -= decrement;
                    if(x < 0){
                        x=0;
                    }
                    wmove(editor, y, x);
                    wdelch(editor);
                    NODE = deleteChar(NODE);
                }
                break;
            case 10:    //ENTER
//                waddch(editor, (char) ch);
                NODE = insertCharBefore(NODE, (char) ch);
                mvwprintw(editor, 0, 0, getFileContents(NODE));
                getyx(editor, y, x);
                x=0;
                wmove(editor, y, 0);
                break;
            default:
                switch (ch) {
                    case 9: increment = 8; break;   // TAB
                    default: increment = 1;
                }
                if(x + increment > WIDTH-1){
                    y++;
                    x=0;
                }
                x += increment;
//                waddch(editor, (char) ch);
                NODE = insertCharBefore(NODE, (char) ch);
                mvwprintw(editor, 0, 0, getFileContents(NODE));
                wmove(editor, y, x);
        }
        //TODO: Debug stuff
        mvwprintw(title_bar, 0, WIDTH - 32, "Ch:%6d | %3c", NODE->data, (char) NODE->data);
        mvwprintw(title_bar, 0, WIDTH - 18, "X:%3d ", x);
        mvwprintw(title_bar, 0, WIDTH - 12, "Y:%3d", y);
        wrefresh(title_bar);
        //DEBUG Stuff ends

        wrefresh(editor);
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
