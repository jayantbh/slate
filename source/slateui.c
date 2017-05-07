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
#include <stdlib.h>
#include "listfunc.h"

int HEIGHT, WIDTH;
WINDOW *title_bar, *editor, *menu;

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
    char str[200] = {'\0'};
    mvwinstr(editor, y, 0, &str);
    int i = WIDTH-1;
    while(str[i] == ' '){
        i--;
    }
    return i;
}

int special_key(){
    int ch;
    ch = getch();	//get [
//                mvprintw(14, 0, "Detected character 1 is = %c ", (char)c);
    ch = getch();
//                mvprintw(13, 0, "Detected character 2 is = %c ", (char)c);
    switch (ch) {
        case 65:    //A, escaped KEY_UP
            return (int) KEY_UP;
            break;
        case 66:    //B, escaped KEY_DOWN
            return (int) KEY_DOWN;
            break;
        case 67:    //C, escaped KEY_RIGHT
            return (int) KEY_RIGHT;
            break;
        case 68:    //D, escaped KEY_LEFT
            return (int) KEY_LEFT;
            break;
        default:
            break;
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
    //TODO: Clean this when needed. #cleanup
    wprintw(editor, "~\n~\n~");
    wrefresh(editor);
}

void init_menu() {
    menu = create_window(1, WIDTH, HEIGHT - 1, 0, 2);
    wmove(menu, 0, 0);
    wprintw(menu, "Ctrl+C:Quit");
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
    long unsigned int ch;
    int x = 0, y = 0, n = 0;    //n is unused

    refresh();
    wrefresh(editor);

    while ((ch = getch())) {
        wrefresh(title_bar);
        switch ((int) ch) {
            case 27:    //^[ ESCAPE SEQUENCE BEGINS
                switch (special_key()){
                    case KEY_UP:
                        y = (y-1 < 0)?0:y-1;
                        wmove(editor, y, x);
                        break;
                    case KEY_DOWN:
                        y = (y+1 > HEIGHT-3)?HEIGHT-3:y+1;
                        wmove(editor, y, x);
                        break;
                    case KEY_LEFT:
                        x = (x-1 < 0)?0:x-1;
                        wmove(editor, y, x);
                        break;
                    case KEY_RIGHT:
                        x = (x+1 > WIDTH-1)?WIDTH-1:x+1;    //Adding 1+1 for Title bar and Footer
                        wmove(editor, y, x);
                        break;
                    default:
                        break;
                }
                break;
            case 127:   //BACKSPACE
                if(x == 0){
                    y--;
                    if(y < 0){
                        y=0;
                    }

//                    mvwprintw(title_bar, 0, WIDTH - 2, "%d", line_length(str));
                    x = line_length(y)+1;

                }
                if(y > 0 || x > 0){
                    x--;
                    if(x < 0){
                        x=0;
                    }
                    wmove(editor, y, x);
                    wdelch(editor);
                }
                break;
            case 10:    //ENTER
                waddch(editor, (char) ch);
                getyx(editor, y, x);
                x=0;
                wmove(editor, y, 0);
                break;
            default:
                if(x > WIDTH-1){
                    y++;
                    x=0;
                }
                waddch(editor, (char) ch);
                wmove(editor, y, ++x);
        }
        //TODO: Debug stuff
        mvwprintw(title_bar, 0, WIDTH - 18, "X:%3d ", x);
        mvwprintw(title_bar, 0, WIDTH - 12, "Y:%3d", y);
        wrefresh(title_bar);
        //DEBUG Stuff ends

        wrefresh(editor);
    }
}

int main(void) {
    signal(SIGINT, finish);

    init_slate();
    keystroke_handler();

    finish(0);
    return 0;
}

/**
 * End Code
 */

#pragma clang diagnostic pop
