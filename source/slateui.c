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
#include <stdlib.h>

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
    wprintw(editor, "~\n~\n~");
    wrefresh(editor);
}

void init_menu() {
    menu = create_window(1, WIDTH, HEIGHT - 1, 0, 2);
    wmove(menu, 0, 0);
    wprintw(menu, "Menu options here");
    wrefresh(menu);
}

void init_windows() {
    init_title_bar();
    init_editor();
    init_menu();
}

void init_colors() {
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLUE);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);
}

void init_curses_config() {
    initscr();
    cbreak();
    refresh();      // Important to refresh screen before refresh window

    HEIGHT = LINES;
    WIDTH = COLS;
}

void init_slate() {
    init_curses_config();
    init_colors();
    init_windows();
    move(1, 0);
}

void keystroke_handler() {
    char ch;
    int x = 0, y = 1;

    initscr();
    keypad(editor, TRUE);
    refresh();

    while (ch = getch()) {
        switch ((int) ch) {
            case 10:    //ENTER
                getyx(stdscr, y, x);
                move(++y, 0);
                break;
            default:
                waddch(editor, (char) ch);
                move(y, ++x);
        }
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
