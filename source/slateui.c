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
#include "slate.h"

WINDOW *TITLE_BAR, *EDITOR, *MENU, *FIND_DIALOG, *REPLACE_DIALOG;
PANEL *_TITLE_BAR, *_EDITOR, *_MENU, *_FIND_DIALOG, *_REPLACE_DIALOG;

int HEIGHT, WIDTH, LINE_COUNT, FIND_INDEX = -1, FIND_X = -1, FIND_Y = -1;
char *FILENAME, *FIND_STRING = "", *REPLACE_STRING = "";
struct node* NODE;
int TAB_WIDTH = 4;
const char MENU_ITEMS[] = "^C: Quit\t^W: Write\t^U: Undo\t^F: Find\t^R: Replace";

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
int line_length(WINDOW* window, int y, int x){
    char *str = (char *) calloc((size_t) WIDTH, sizeof(char));
    mvwinstr(window, y, x, str);
    int i = WIDTH - x - 1;

    while(str[i] == ' ' && i--);
    return i + 1;
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

int characters_after_cursor(WINDOW* window, int y, int x) {
    //TODO: Handle tabs
    int distance = line_length(window, y, 0) - x;   // +1 because `x` will be in whole numbers, but `line_length` will return natural numbers
    if (distance < 0) {
        return 0;
    }
    return distance;
}

int characters_before_cursor(WINDOW* window, int y, int x) {
    //TODO: Handle tabs
    int length = line_length(window, y, 0);
    if (length < x) {
        return length;
    }
    return x;
}

void print_line(char *BUFFER, int y, int x, int *node_index, int *position_iterator) {
    int highlight_length = (int) strlen(FIND_STRING);
    int highlighted_index, highlighted_node_index, last_highlighted_index = -1;
    int i = 0;
    bool do_highlight;
    while(i < strlen(BUFFER)) {
        do_highlight = (*node_index == positionArray[*position_iterator]) && isFindDirty;
        if (do_highlight) {
            highlighted_index = i;
            highlighted_node_index = *node_index;

            if (*position_iterator == FIND_INDEX) {
                FIND_X = i;
                FIND_Y = y;
            }

            wattron(EDITOR, COLOR_PAIR(3));
            while (highlighted_node_index < positionArray[*position_iterator] + highlight_length) {
                mvwaddch(EDITOR, y, highlighted_index, (chtype) BUFFER[highlighted_index]);
                last_highlighted_index = highlighted_index;
                highlighted_index++;
                if (BUFFER[highlighted_index]) {
                    highlighted_node_index++;
                }
                else {
                    return;
                }
            }
            wattroff(EDITOR, COLOR_PAIR(3));
            (*position_iterator)++;
        }
        else if (BUFFER[i] && i > last_highlighted_index) {
            mvwaddch(EDITOR, y, i, (chtype) BUFFER[i]);
        }

        i++;
        (*node_index)++;
    }
    wmove(EDITOR, y, x);
    wclrtoeol(EDITOR);
}

void refresh_editor(int y) {
    wclear(EDITOR);
    int height = HEIGHT - 2;
    int max_allowed_y_line = LINE_COUNT - height + 1;

    if (y > 0 && y > max_allowed_y_line) {
        y = max_allowed_y_line;
    }
    if (LINE_COUNT <= height) {
        y = 0;
    }
    if (y < 0) {
        y = 0;
    }

    int TOP_LIMIT = y, BOTTOM_LIMIT = y + height;
    char *BUFFER = (char *) calloc((size_t) WIDTH, sizeof(char));
    bool line_is_in_view, line_ends, result_out_of_view;

    struct node* KEY = getHead(NODE)->next; // Skip one node, because the first item in the linked list is a non-printable character.
    int i = 0, ch_index = 0, line = 0, node_index = 1, position_iterator = 0, total_lines = 0, current_line;  // current_line exists just for semantics
    while (KEY != NULL)
    {
        int limit = WIDTH;
        char ch = KEY->data;

        switch (ch) {
            case '\t': limit -= TAB_WIDTH; break;
            default: break;
        }

        line_is_in_view = current_line >= TOP_LIMIT && current_line < BOTTOM_LIMIT;
        line_ends = i + 1 == limit || ch == '\n';
        if (line_ends) {
            if (line_is_in_view) {
                print_line(BUFFER, line, i, &node_index, &position_iterator);
                line++;
            }
            else {
                node_index += strlen(BUFFER);
            }
            if (ch == '\n') {
                node_index++;
            }

            i = 0;
            total_lines++;

            memset(BUFFER, 0, WIDTH * (sizeof BUFFER));
            if (KEY->next == NULL) {
                break;
            }
        }
        else {
            BUFFER[i] = ch;
            i++;
            if (line_is_in_view && KEY->next == NULL) {
                print_line(BUFFER, line, i, &node_index, &position_iterator);
                line++;
            }
        }

        KEY = KEY->next;
        current_line = total_lines;
        result_out_of_view = ch_index == positionArray[position_iterator] && position_iterator == FIND_INDEX && isFindDirty && FIND_X == -1 && FIND_Y == -1;
        if (result_out_of_view) {
            FIND_X = i;
            FIND_Y = line;
            NODE = moveCursorToIndex(NODE, ch_index - 1);
        }
        ch_index++;
    }

    LINE_COUNT = total_lines;
}
/**
 * Initialization functions
 */

void init_title_bar() {
    TITLE_BAR = create_window(1, WIDTH, 0, 0, 2);
    wmove(TITLE_BAR, 0, 0);
    wprintw(TITLE_BAR, "Project Slate, dims:%d:%d, accessing file: %s", HEIGHT, WIDTH, FILENAME);
    wrefresh(TITLE_BAR);
}

void init_editor() {
    EDITOR = create_window(HEIGHT - 2, WIDTH, 1, 0, 0);
    wmove(EDITOR, 0, 0);
    wclear(EDITOR);
    refresh_editor(0);
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
    mvwprintw(FIND_DIALOG, 0, width/2 - 2, "FIND");
    wmove(FIND_DIALOG, 1, 1);
    wrefresh(FIND_DIALOG);
}

void init_replace_dialog() {
    int width = (int) (WIDTH * 0.6);
    int height = 6;
    int x_pos = WIDTH/2 - width/2;
    int y_pos = HEIGHT/2 - height/2;
    REPLACE_DIALOG = create_window(height, width, y_pos, x_pos, 1);
    mvwhline(REPLACE_DIALOG, 0, 1, 0, width - 2);
    mvwhline(REPLACE_DIALOG, 2, 1, 0, width - 2);
    mvwhline(REPLACE_DIALOG, 4, 1, 0, width - 2);
    mvwprintw(REPLACE_DIALOG, 0, width/2 - 4, "FIND FOR");
    mvwprintw(REPLACE_DIALOG, 2, width/2 - 6, "REPLACE WITH");
    wmove(REPLACE_DIALOG, 1, 1);
    wrefresh(REPLACE_DIALOG);
}

void init_panels() {
    _TITLE_BAR = new_panel(TITLE_BAR);
    _EDITOR = new_panel(EDITOR);
    _MENU = new_panel(MENU);
    _FIND_DIALOG = new_panel(FIND_DIALOG);
    _REPLACE_DIALOG = new_panel(REPLACE_DIALOG);

    set_panel_userptr(_TITLE_BAR, _EDITOR);
    set_panel_userptr(_EDITOR, _MENU);
    set_panel_userptr(_MENU, _FIND_DIALOG);
    set_panel_userptr(_FIND_DIALOG, _REPLACE_DIALOG);
    set_panel_userptr(_REPLACE_DIALOG, _TITLE_BAR);

    top_panel(_EDITOR);
    update_panels();
    doupdate();
}

void init_windows() {
    init_title_bar();
    init_menu();
    init_editor();
    init_find_dialog();
    init_replace_dialog();
}

void init_colors() {
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLUE);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);
    init_pair(3, COLOR_WHITE, COLOR_RED);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(5, COLOR_BLUE, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_BLUE, COLOR_CYAN);
}

void init_curses_config() {
    initscr();
    cbreak();
    noecho();
    set_tabsize(TAB_WIDTH);
    refresh();

    HEIGHT = LINES;
    WIDTH = COLS;
}

void init_slate() {
    init_curses_config();
    init_colors();
    init_windows();
    init_panels();
    keypad(EDITOR, TRUE);
    keypad(FIND_DIALOG, TRUE);
    keypad(REPLACE_DIALOG, TRUE);
    scrollok(EDITOR, TRUE);
    move(1, 0);
    wmove(EDITOR, 0, 0);
}

/**
 * handle_find_replace - Handle functionality for find, find and replace, and find and replace all
 * @param CURRENT_WINDOW
 * @param mode - 0: find, 1: find and replace, or find and replace all
 */
void handle_find_replace(WINDOW* CURRENT_WINDOW, int mode) {
    int ch;
    short int loc_x = 0, loc_y = 0, increment = 0, decrement = 0;
    short int find_x = 0, replace_x = 0;
    short int FIELD, FIND_FIELD = 1, REPLACE_FIELD = 3;  // FIELD: Field flag to determine which field is focused, also used for `y` vertical coordinates in window

    if (mode == MODE_REPLACE) {
        wmove(CURRENT_WINDOW, 3, 1);
        wclrtoeol(CURRENT_WINDOW);
        wmove(CURRENT_WINDOW, 5, 1);
        wclrtoeol(CURRENT_WINDOW);
        mvwprintw(REPLACE_DIALOG, 5, 1, "[ENTER]");
    }

    wmove(CURRENT_WINDOW, 1, 0);
    wclrtoeol(CURRENT_WINDOW);
    wmove(CURRENT_WINDOW, 1, 1);
    update_panels();
    doupdate();

    struct node* FIND = getEmptyList();
    struct node* REPLACE = getEmptyList();
    struct node* CURRENT_FIELD = FIND;
    FIELD = FIND_FIELD;
    while ((ch = wgetch(CURRENT_WINDOW))) {
        loc_y = FIELD;
        switch (ch) {
            case KEY_UP:
            case KEY_DOWN:
                break;
            case KEY_LEFT:
                //TODO: Handle Tabs
                if (loc_x != 0) {
                    loc_x--;
                    CURRENT_FIELD = moveCursor(CURRENT_FIELD, -1);
                }
                break;
            case KEY_RIGHT:
                //TODO: Handle Tabs
                if (loc_x <= line_length(CURRENT_WINDOW, loc_y, 1)) {
                    loc_x++;
                    CURRENT_FIELD = moveCursor(CURRENT_FIELD, 1);
                }
                break;
            case KEY_TAB:
                if (mode == MODE_REPLACE) {
                    if (FIELD == FIND_FIELD) {
                        FIELD = REPLACE_FIELD;
                        CURRENT_FIELD = REPLACE;
                        find_x = loc_x;
                        loc_x = replace_x;
                    }
                    else if (FIELD == REPLACE_FIELD) {
                        FIELD = FIND_FIELD;
                        CURRENT_FIELD = FIND;
                        replace_x = loc_x;
                        loc_x = find_x;
                    }
                    CURRENT_FIELD = moveCursor(CURRENT_FIELD, loc_x);
                    loc_y = FIELD;
                }
                break;
            case KEY_ENTER:    //ENTER
                if (mode == MODE_REPLACE) {
                    wattron(CURRENT_WINDOW, COLOR_PAIR(7));
                    mvwhline(CURRENT_WINDOW, 5, 0, ' ', (int) (WIDTH * 0.6));
                    mvwprintw(CURRENT_WINDOW, 5, 1, "[1]: Replace first | [2]: Replace all");
                    wattroff(CURRENT_WINDOW, COLOR_PAIR(7));
                }
                wmove(CURRENT_WINDOW, loc_y, loc_x + 1);

                FIND_STRING = stringSlice(getContents(FIND), 1, INF);
                if (strlen(FIND_STRING) == 0) {
                    // do nothing, and return to editor window by closing off find/replace window
                }
                else if (mode == MODE_FIND) {
                    find(FIND_STRING, NODE);
                }
                else if (mode == MODE_REPLACE) {
                    REPLACE_STRING = stringSlice(getContents(REPLACE), 1, INF);
                    int temp_ch;
                    temp_ch = wgetch(CURRENT_WINDOW);
                    switch (temp_ch) {
                        case 49: findAndReplace(FIND_STRING, REPLACE_STRING, NODE); break;
                        default: findAndReplaceAll(FIND_STRING, REPLACE_STRING, NODE);
                    }
                }
                top_panel(_EDITOR);
                CURRENT_WINDOW = EDITOR;
                update_panels();
                doupdate();
                break;
            case KEY_ESC:    //ESCAPE
                top_panel(_EDITOR);
                CURRENT_WINDOW = EDITOR;
                update_panels();
                doupdate();
                break;
            case KEY_BKSP:  //BACKSPACE
            case KEY_BKSP_ALT:    //BACKSPACE
                decrement = 1;
                if (loc_x == 0) {
                    break;
                }
                if(loc_x > 0){
                    loc_x -= decrement;
                    if(loc_x < 0){
                        loc_x=0;
                    }
                }
                CURRENT_FIELD = deleteChar(CURRENT_FIELD);
                break;
            default:
                increment = 1;
                loc_x += increment;
                CURRENT_FIELD = insertCharAfter(CURRENT_FIELD, (char) ch);
        }

        if (ch == KEY_ENTER || ch == KEY_ESC) {
            debug(loc_x, loc_y);
            wrefresh(CURRENT_WINDOW);
            break;
        }

        wmove(CURRENT_WINDOW, loc_y, 0);
        wclrtoeol(CURRENT_WINDOW);

        char *contents = getContents(CURRENT_FIELD);
        for (int i = 1; i < strlen(contents); i++) {
            mvwaddch(CURRENT_WINDOW, loc_y, i, (chtype) contents[i]);
        }

        wmove(CURRENT_WINDOW, loc_y, loc_x + 1);
        wrefresh(CURRENT_WINDOW);
    }
}

void keystroke_handler() {
    int ch;
    int x = 0, y = 0, increment = 0, decrement = 0, shift = 0, scroll_offset = 0, position_array_length;
    int current_line_length = 0;
    bool cursor_at_first_row, cursor_at_last_row, cursor_at_first_line, cursor_at_last_line;

    refresh();
    wrefresh(EDITOR);

    WINDOW* CURRENT_WINDOW = EDITOR;
    while ((ch = wgetch(CURRENT_WINDOW))) {
        wrefresh(TITLE_BAR);
        shift = 0;
        switch (ch) {
            case KEY_ESC:    // ^[ ESCAPE
                isFindDirty = false;
                top_panel(_EDITOR);
                CURRENT_WINDOW = EDITOR;
                break;
            case KEY_UP:
                isFindDirty = false;
                cursor_at_first_row = y <= 0;
                cursor_at_first_line = y + scroll_offset == 0;

                if (cursor_at_first_line) {
                    shift -= characters_before_cursor(CURRENT_WINDOW, y, x);
                    NODE = moveCursor(NODE, shift);

                    y = 0;
                    x = 0;
                }
                else {
                    shift--; // 1 for newline
                    if (cursor_at_first_row) {
                        refresh_editor(--scroll_offset);
                        shift -= characters_before_cursor(CURRENT_WINDOW, y, x);
                        shift -= characters_after_cursor(CURRENT_WINDOW, y + 1, x);
                    }
                    else {
                        shift -= characters_before_cursor(CURRENT_WINDOW, y, x);
                        shift -= characters_after_cursor(CURRENT_WINDOW, y - 1, x);
                        y--;
                    }
                    NODE = moveCursor(NODE, shift);

                    current_line_length = line_length(CURRENT_WINDOW, y, 0);
                    if (current_line_length < x) {
                        x = current_line_length;
                    }
                }
                break;
            case KEY_DOWN:
                isFindDirty = false;
                cursor_at_last_row = y + 1 > HEIGHT - 3;
                cursor_at_last_line = y + scroll_offset == LINE_COUNT;

                if (cursor_at_last_line) {    // HEIGHT - 3 because, -1 for last row index (0..HEIGHT-1), -1 for TITLE_BAR, -1 for MENU
                    shift += characters_after_cursor(CURRENT_WINDOW, y, x);
                    NODE = moveCursor(NODE, shift);

                    x = line_length(CURRENT_WINDOW, y, 0);
                }
                /**
                 * LINE_COUNT is from 1..N, y is from 0..HEIGHT.
                 * We're checking if the next line index will NOT be less than the number of lines.
                 * Because when y = 2 at most, we'll have 3 lines, because 0, 1, 2.
                 */
                else {  // If cursor isn't at the last line
                    shift++; // 1 for newline
                    if (cursor_at_last_row) {
                        refresh_editor(++scroll_offset);
                        shift += characters_after_cursor(CURRENT_WINDOW, y - 1, x);
                        shift += characters_before_cursor(CURRENT_WINDOW, y, x);
                    }
                    else {
                        shift += characters_after_cursor(CURRENT_WINDOW, y, x);
                        shift += characters_before_cursor(CURRENT_WINDOW, y + 1, x);
                        y++;
                    }
                    NODE = moveCursor(NODE, shift);

                    current_line_length = line_length(CURRENT_WINDOW, y, 0);
                    if (current_line_length < x) {
                        x = current_line_length;
                    }
                }
                break;
            case KEY_LEFT:
                isFindDirty = false;
                //TODO: Handle Tabs
                if (x != 0) {
                    x = x - 1;
                    NODE = moveCursor(NODE, -1);
                }
                break;
            case KEY_RIGHT:
                isFindDirty = false;
                //TODO: Handle Tabs
                if (x < line_length(CURRENT_WINDOW, y, 0)) {
                    x++;
                    NODE = moveCursor(NODE, 1);
                }
                break;
            case KEY_SLEFT:
                if (!isFindDirty) {
                    break;
                }

                FIND_X = -1;
                FIND_Y = -1;

                position_array_length = positionArrayLength();
                if (position_array_length && FIND_INDEX - 1 == -1) {
                    FIND_INDEX = positionArrayLength() - 1;
                }
                else {
                    FIND_INDEX--;
                }

                refresh_editor(scroll_offset);
                if (FIND_X != -1 && FIND_Y != -1) {
                    scroll_offset = FIND_Y;
                    refresh_editor(scroll_offset);

                    x = FIND_X;
                    y = FIND_Y;
                }
                break;
            case KEY_SRIGHT:
                if (!isFindDirty) {
                    break;
                }

                FIND_X = -1;
                FIND_Y = -1;

                position_array_length = positionArrayLength();
                if (position_array_length && FIND_INDEX + 1 == position_array_length) {
                    FIND_INDEX = 0;
                }
                else {
                    FIND_INDEX++;
                }

                refresh_editor(scroll_offset);
                if (FIND_X != -1 && FIND_Y != -1) {
                    scroll_offset = FIND_Y;
                    refresh_editor(scroll_offset);

                    x = FIND_X;
                    y = FIND_Y;
                }
                break;
            case 21:    // CTRL + U : UNDO
                isFindDirty = false;
                shift = undo(&NODE);
                x += shift;
                wmove(EDITOR, y, x);
                if (shift < 0) {
                    wdelch(EDITOR);
                }
                break;
            case CTRL_F:     // CTRL + F : FIND
                isFindDirty = false;

                FIND_INDEX = 0;
                FIND_X = -1;
                FIND_Y = -1;

                top_panel(_FIND_DIALOG);
                CURRENT_WINDOW = FIND_DIALOG;
                handle_find_replace(CURRENT_WINDOW, MODE_FIND);
                top_panel(_EDITOR);
                CURRENT_WINDOW = EDITOR;

                refresh_editor(scroll_offset);
                if (FIND_X != -1 && FIND_Y != -1) {
                    scroll_offset = FIND_Y;
                    refresh_editor(scroll_offset);

                    x = FIND_X;
                    y = FIND_Y;
                }
                break;
            case CTRL_R:     // CTRL + R : REPLACE/REPLACE ALL
                isFindDirty = false;
                FIND_INDEX = 0;

                top_panel(_REPLACE_DIALOG);
                CURRENT_WINDOW = REPLACE_DIALOG;
                handle_find_replace(CURRENT_WINDOW, MODE_REPLACE);
                top_panel(_EDITOR);
                CURRENT_WINDOW = EDITOR;
                break;
            case CTRL_K:    // CTRL + K : WRITE
            case CTRL_L:    // CTRL + L : WRITE
            case CTRL_W:    // CTRL + W : WRITE
                writeBackToFile(NODE, FILENAME); break;
            case KEY_BKSP:  //BACKSPACE
            case KEY_BKSP_ALT:    //BACKSPACE
                switch ((int) NODE->data) {
                    case KEY_TAB: decrement = gap_before_cursor(CURRENT_WINDOW, WIDTH, y, x); break;   // TAB
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

                    x = line_length(CURRENT_WINDOW, y, 0);
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
                    case KEY_TAB: increment = TAB_WIDTH - x % TAB_WIDTH; break;   // TAB
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

        refresh_editor(scroll_offset);
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

    FILENAME = argv[1];
    NODE = loadFileToList(FILENAME);

    init_slate();
    keystroke_handler();

    finish(0);
    return 0;
}

/**
 * End Code
 */

#pragma clang diagnostic pop
