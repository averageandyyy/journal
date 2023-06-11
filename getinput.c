#include <ncurses.h>
#include <string.h>

#define LENGTH 100

int main() 
{
    
    char input[LENGTH];
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int length = 0;
    printw("Getting some input");
    move(1,0);
    // printw("%i", getcurx(stdscr));
    refresh();

    

    int ch;
    while ((ch = getch()) != '\n' && length < LENGTH - 1)
    {
        if (ch == KEY_BACKSPACE)
        {
            if (length > 0)
            {
                // backspace moves cursor
                if (getcurx(stdscr) == 0)
                {
                    move(getcury(stdscr) - 1, COLS - 1);
                    delch();
                    length--;
                }
                else
                {
                    mvdelch(getcury(stdscr), getcurx(stdscr) - 1);
                    length--;
                }
            }
        }
        else if (ch < KEY_MIN || ch > KEY_MAX)
        {
            input[length] = ch;
            addch(ch);
            length++;
        }
    }
    input[length] = '\0';

    endwin();
    printf("Your input was {%s}\n", input);
    return 0;
}