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

int main(void) {
    char ch;
    int x, y;

    initscr();
    keypad(stdscr, TRUE);
    addstr("Type a few lines of text\n");
    addstr("Press ` to quit\n");
    refresh();

    while ((ch = getch()) != '`') {
        if ((int) ch == 10) {
            getyx(stdscr, y, x);
            move(++y, 0);
        }
    }

    endwin();
    return 0;
}



/**
 * End Code
 */

#pragma clang diagnostic pop
