#include <ncurses.h>
#include <string.h>

#define CURSOR_POS (getcury(stdscr) - 1) * COLS + getcurx(stdscr)
#define LENGTH 2000

int main() 
{
    
    char input[LENGTH];
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int length = 0;
    printw("Getting some input %i/%i", length, LENGTH - 1);
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
                int y, x;
                y = getcury(stdscr);
                x = getcurx(stdscr); 
                for (int i = CURSOR_POS - 1; i < length - 1; i++)
                {
                    input[i] = input[i+1];
                }
                length--;
                input[length] = '\0';
                // backspace moves cursor but not anymore with noecho();
                if (x == 0)
                {
                    clear();
                    printw("Getting some input %i/%i", length, LENGTH - 1);
                    move(1,0);
                    printw("%s", input);
                    move(y - 1, COLS - 1);
                }
                else
                {
                    clear();
                    printw("Getting some input %i/%i", length, LENGTH - 1);
                    move(1,0);
                    printw("%s", input);
                    move(y, x - 1);
                }
            }
        }
        else if (ch == KEY_LEFT)
        {
            int y, x;
            y = getcury(stdscr);
            x = getcurx(stdscr);
            if (length > 0 && CURSOR_POS > 0)
            {
                if (x == 0)
                {
                    move(y - 1, COLS - 1);
                }
                else
                {
                    move(y, x - 1);
                }
            }
        }
        else if (ch == KEY_RIGHT)
        {
            int y, x;
            y = getcury(stdscr);
            x = getcurx(stdscr);
            if (CURSOR_POS < length)
            {
                if (x == COLS - 1)
                {
                    move(y + 1, 0);
                }
                else
                {
                    move(y, x + 1);
                }
            }
        }
        else if (ch < KEY_MIN || ch > KEY_MAX)
        {
            int y, x;
            y = getcury(stdscr);
            x = getcurx(stdscr);
            for (int i = length; i > CURSOR_POS; i--)
            {
                input[i] = input[i - 1];
            }
            input[CURSOR_POS] = ch;
            length++;
            input[length] = '\0';
            clear();
            printw("Getting some input %i/%i", length, LENGTH - 1);
            move(1,0);
            printw("%s", input);
            if (x == COLS - 1)
            {
                move(y + 1, 0);
            }
            else
            {
                move(y, x + 1);
            }
        }
    }
    input[length] = '\0';

    endwin();
    printf("Your input was {%s}\n", input);
    return 0;
}