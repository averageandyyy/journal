#include <ncurses.h>
#include <string.h>

#define LENGTH 100

int main() 
{
    
    char input[LENGTH];
    initscr();
    cbreak();
    keypad(stdscr, TRUE);

    int length = 0;
    printw("Getting some input");
    move(1,0);
    refresh();

    int ch;
    getnstr(input, LENGTH);
    // input[length] = '\0';

    endwin();
    printf("Your input was {%s}\n", input);

    return 0;
}