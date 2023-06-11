#include <ncurses.h>

int main(void)
{
    char input[500];
    
    initscr();
    cbreak();
    keypad(stdscr, TRUE);

    int ch;
    int length = 0;
    move(3, 0);
    while ((ch = getch()) != '\n' && length < 499)
    {
        if ((ch == KEY_BACKSPACE || ch == KEY_LEFT) && length > 0)
        {
            delch();
            length--;
        }
        else
        {
            input[length] = ch;
            length++;
        }
        
    }
    endwin();

    input[length] = '\0';
    printf("Your input was: %s\n", input);
}