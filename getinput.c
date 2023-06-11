#include <ncurses.h>
#include <string.h>

int main() {
    // Initialize ncurses
    initscr();

    // Read the original string from a text file
    FILE *file = fopen("file.txt", "r");
    char originalString[100];
    fgets(originalString, sizeof(originalString), file);
    fclose(file);

    // Create a copy of the original string
    char updatedString[100];
    strcpy(updatedString, originalString);

    // Update the string
    mvprintw(0, 0, "Original string: %s", originalString);
    mvprintw(1, 0, "Enter a new string: ");
    refresh();

    // Get user input and update the string
    getstr(updatedString);

    // Display the updated string
    clear();
    mvprintw(0, 0, "Original string: %s", originalString);
    mvprintw(1, 0, "Updated string: %s", updatedString);
    refresh();

    // Wait for user input
    getch();

    // Cleanup
    endwin();

    return 0;
}