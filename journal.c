#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>


#define MAX_LENGTH 2000
#define NUM_YEARS 24
#define NUM_MONTHS 12
#define JOURNAL_FOLDER "entries"
#define CURSOR_POS (getcury(stdscr) - 1) * COLS + getcurx(stdscr)


void get_entry(char *input, int max_length, char *folder);
void get_date(char *date, size_t buffer_size);
void get_year(char *year, size_t buffer_size);
void get_month(char *month, size_t buffer_size);
void remove_new_line(char *str);
bool create_directories(char *directory);
bool file_exists(char *filename);
int file_count(char *filepath);
int get_year_files(char *folder, char years[][8]);
int get_month_files(char *folder, char months[][24]);
int get_read_files(char *folder, char readfiles[][100]);
void read_file(FILE *to_read);
void update_entry(char *input, int max_length, char *folder);
int get_read_files_complete(char *readfile, char *folder);


int main(int argc, char *argv[])
{
    char *options = "urwh";
    char option = getopt(argc, argv, options);

    if (option == '?')
    {
        printf("Invalid option. Valid options are: -r to read and -w to write.\n");
        return 1;
    }

    if (getopt(argc, argv, options) != -1)
    {
        printf("Only one filter allowed.\n");
        return 2;
    }

    if (argc != 2 && argc != 3)
    {
        printf("Usage: ./journal [flag] [directory]\nOtherwise, default folder is entries/\n-r to read, -w to write, -u to update and -h for help\n");
        return 3;
    }

    // showing current date once
    char date[80];
    get_date(date, sizeof(date));
    printf("Current date: %s\n", date);
    char year[8];
    get_year(year, sizeof(year));
    char month[24];
    get_month(month, sizeof(month));
    char folder[80];
    (argc == 3) ? strcpy(folder, argv[2]) : strcpy(folder, JOURNAL_FOLDER);
    char input[MAX_LENGTH];
    char readfile[100];
    FILE *to_read = NULL;

    switch (option)
    {
        // help flag
        case 'h':
            printf("Usage: ./journal [flag] [directory]\nOtherwise, default folder is entries/\n-r to read, -w to write, -u to update and -h for help\n");
            return 0;


        // write flag
        case 'w':
            char file[200]; 
            char directory[100];
            do
            {
                // reset file string on every iteration
                memset(file, 0, sizeof(file));
                memset(directory, 0, sizeof(directory));
                // copy folder unto file
                (argc == 3) ? strcpy(file, argv[2]) : strcpy(file, JOURNAL_FOLDER);
                strcat(file, "/");
                strcat(directory, file);


                // getting current year
                get_year(year, sizeof(year));
                strcat(file, strcat(year, "/"));
                strcat(directory, year);


                // getting current month
                get_month(month, sizeof(month));
                strcat(directory, month);
                strcat(file, strcat(month, "/"));


                // directory does not have .txt
                printf("%s\n", directory);


                // getting title
                char title[100];
                printf("Give us a title: ");
                fgets(title, 100, stdin);
                // remove '\n'
                remove_new_line(title);
                if (strlen(title) == 0)
                {
                    int filecount = file_count(file);
                    sprintf(title, "%03i", filecount);
                }
                strcat(title, ".txt");
                strcat(file, title);
            }
            while (file_exists(file));
            // file must not exist
            

            // create directories if not present
            printf("Creating directories in %s.\n", directory);
            create_directories(directory);
            printf("Writing into %s\n", file);

            
            // getting valid entry and writing into file
            get_entry(input, MAX_LENGTH, file);
            if (strlen(input) == 0)
            {
                printf("Empty entry!\n");
                return 1;
            }
            printf("Your entry was: %s\n", input);
            printf("Writing into %s\n", file);
            FILE *output = fopen(file, "w");
            if (output == NULL)
            {
                printf("Could not open file %s\n", file);
                return 1;
            }
            fputs(input, output);
            fclose(output);
            printf("Writing complete! Check %s!\n", file);
            break;
        
        
        // read flag
        case 'r': 
            if (get_read_files_complete(readfile, folder) != 0)
            {
                return 1;
            }
            to_read = fopen(readfile, "r");
            if (to_read == NULL)
            {
                printf("Failed to open %s.\n", readfile);
                return 1;
            }
            read_file(to_read);
            break;
        

        // update flag
        case 'u': 
            if (get_read_files_complete(readfile, folder) != 0)
            {
                return 1;
            }
            to_read = fopen(readfile, "r");
            if (to_read == NULL)
            {
                printf("Failed to open %s.\n", readfile);
                return 1;
            }
            size_t bytes_read = fread(input, sizeof(char), MAX_LENGTH - 1, to_read);
            input[bytes_read] = '\0';
            printf("Input length %li\n", strlen(input));
            update_entry(input, MAX_LENGTH, readfile);
            printf("Your entry was: %s\n", input);
            fclose(to_read);
            FILE *to_update = fopen(readfile, "w");
            if (to_update == NULL)
            {
                printf("Something went wrong!\n");
                return 1;
            }
            fputs(input, to_update);
            printf("Writing complete! Check %s!\n", readfile);
            fclose(to_update);
    }


    return 0;
}


// function to get input in a fashion similar to the terminal
void get_entry(char *input, int max_length, char *folder)
{
    initscr();
    cbreak();
    // noecho allows for full control over input and its effects. changes made to screen/window can be controlled.
    noecho();
    keypad(stdscr, TRUE);

    // initialization
    int length = 0;
    printw("Writing into %s. Getting some input %i/%i", folder, length, MAX_LENGTH - 1);
    move(1, 0);
    refresh();

    // auto quits when length == MAX_LENGTH - 2 as input[MAX_LENGTH - 1] then must be '\0'
    int ch;
    while ((ch = getch()) != '\n' && length < MAX_LENGTH - 1)
    {
        if (ch == KEY_BACKSPACE)
        {
            // backspace interactions should only take place when cursor is not at start of string
            if (length > 0 && CURSOR_POS > 0)
            {
                int y, x;
                y = getcury(stdscr);
                x = getcurx(stdscr);
                // updating characters, essentially shifting them down left by 1
                for (int i = CURSOR_POS - 1; i < length - 1; i++)
                {
                    input[i] = input[i + 1];
                }
                length--;
                input[length] = '\0';
                // backspace moves cursor but not anymore with noecho();
                // start of row shift back one row and to end of said row
                // clearing and reprinting input helps with display management, meaning we only need to manage where the cursor should be as input is always correct.
                if (x == 0)
                {
                    clear();
                    printw("Writing into %s. Getting some input %i/%i", folder, length, MAX_LENGTH - 1);
                    move(1, 0);
                    printw("%s", input);
                    move(y - 1, COLS - 1);
                }
                // otherwise just shift back once
                else
                {
                    clear();
                    printw("Writing into %s. Getting some input %i/%i", folder, length, MAX_LENGTH - 1);
                    move(1, 0);
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
            // left key interactions control only cursor. input display unaffected.
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
        // only care about character inputs
        else if (ch < KEY_MIN || ch > KEY_MAX)
        {
            int y, x;
            y = getcury(stdscr);
            x = getcurx(stdscr);
            // updating based on cursor position. characters are shifted down right by 1.
            for (int i = length; i > CURSOR_POS; i--)
            {
                input[i] = input[i - 1];
            }
            // update
            input[CURSOR_POS] = ch;
            length++;
            input[length] = '\0';
            // as usual, clearing and reprinting input allows us to manage cursor position.
            clear();
            printw("Writing into %s. Getting some input %i/%i", folder, length, MAX_LENGTH - 1);
            move(1, 0);
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
}


// function to get date string
void get_date(char *date, size_t buffer_size)
{
    memset(date, 0, buffer_size);
    time_t rawtime;
    struct tm *timeinfo; // pointer to tm struct defined in time.h

    time(&rawtime); // probably read time into rawtime
    timeinfo = localtime(&rawtime); // get localtime from values stored in rawtime
    
    // format and writing timeinfo into buffer
    strftime(date, buffer_size, "%Y-%B-%d", timeinfo);
}


// function to get year string
void get_year(char *year, size_t buffer_size)
{
    memset(year, 0, buffer_size);
    time_t rawtime;
    struct tm *timeinfo; // pointer to tm struct defined in time.h

    time(&rawtime); // probably read time into rawtime
    timeinfo = localtime(&rawtime); // get localtime from values stored in rawtime
    
    // format and writing timeinfo into buffer
    strftime(year, buffer_size, "%Y", timeinfo);
}


// function to get month string
void get_month(char *month, size_t buffer_size)
{
    memset(month, 0, buffer_size);
    time_t rawtime;
    struct tm *timeinfo; // pointer to tm struct defined in time.h

    time(&rawtime); // probably read time into rawtime
    timeinfo = localtime(&rawtime); // get localtime from values stored in rawtime
    
    // format and writing timeinfo into buffer
    strftime(month, buffer_size, "%B", timeinfo);
}


// function to remove new line from fgets
void remove_new_line(char *str)
{
    size_t length = strcspn(str, "\n");
    str[length] = '\0';
}


// function to check if a file exists
bool file_exists(char *filename)
{
    if (access(filename, F_OK) != -1)
    {
        printf("%s exists!\n", filename);
        return true;
    }
    
    return false;
}


// function to count files to use as file title in the event a title is not provided
int file_count(char *filepath)
{
    DIR *dir = opendir(filepath);

    if (dir == NULL)
    {
        printf("Failed to open directory\n");
        return 0;
    }
    int filecount = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            filecount++;
        }
    }

    closedir(dir);

    return filecount;
}


// function to create directories by accessing the command line
bool create_directories(char *directory)
{
    char command[100];

    snprintf(command, sizeof(command), "mkdir -p %s", directory);
    int status = system(command);

    if (status == 0)
    {
        printf("Directories in %s created.\n", directory);
        return true;
    }
    else
    {
        printf("Directories not created.\n");
        return false;
    }
}


// function to get year folders
int get_year_files(char *folder, char years[][8])
{
    // years[NUM_YEARS][8];
    DIR *directory = opendir(folder);
    if (directory == NULL)
    {
        printf("Failed to open directory.\n");
        return -1;
    }

    int foldercount = 0;
    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL)
    {
        if (entry->d_type == DT_DIR)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                strcpy(years[foldercount], entry->d_name);
                foldercount++;
            }
        }
    }

    closedir(directory);
    return foldercount;
}


// function to get month folders in a particular year
int get_month_files(char *folder, char months[][24])
{
    // char months[NUM_MONTHS][24];
    DIR *directory = opendir(folder);
    if (directory == NULL)
    {
        printf("Failed to open directory.\n");
        return -1;
    }

    int foldercount = 0;
    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL)
    {
        if (entry->d_type == DT_DIR)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                strcpy(months[foldercount], entry->d_name);
                foldercount++;
            }
        }
    }

    closedir(directory);
    return foldercount;
}


// function to get files in a particular month
int get_read_files(char *folder, char readfiles[][100])
{
    // char readfiles[100][100];
    DIR *directory = opendir(folder);
    if (directory == NULL)
    {
        printf("Failed to open directory.\n");
        return -1;
    }

    int filecount = 0;
    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            strcpy(readfiles[filecount], entry->d_name);
            filecount++;
        }
    }

    closedir(directory);
    return filecount;
}


// function to output text in file
void read_file(FILE *to_read)
{
    int ch;

    while (!feof(to_read))
    {
        ch = fgetc(to_read);
        if (ch != EOF)
        {
            putchar(ch);
        }
    }

    printf("\nOutput complete.\n");
    fclose(to_read);
}



// function to update an existing entry. similar to get_entry except for an initial print of the text and length initialization
void update_entry(char *input, int max_length, char *folder)
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int length = strlen(input);
    printw("Writing into %s. Getting some input %i/%i", folder, length, MAX_LENGTH - 1);
    move(1, 0);
    printw("%s", input);
    refresh();

    

    int ch;
    while ((ch = getch()) != '\n' && length < MAX_LENGTH - 1)
    {
        if (ch == KEY_BACKSPACE)
        {
            if (length > 0 && CURSOR_POS > 0)
            {
                int y, x;
                y = getcury(stdscr);
                x = getcurx(stdscr); 
                for (int i = CURSOR_POS - 1; i < length - 1; i++)
                {
                    input[i] = input[i + 1];
                }
                length--;
                input[length] = '\0';
                // backspace moves cursor but not anymore with noecho();
                if (x == 0)
                {
                    clear();
                    printw("Writing into %s. Getting some input %i/%i", folder, length, MAX_LENGTH - 1);
                    move(1, 0);
                    printw("%s", input);
                    move(y - 1, COLS - 1);
                }
                else
                {
                    clear();
                    printw("Writing into %s. Getting some input %i/%i", folder, length, MAX_LENGTH - 1);
                    move(1, 0);
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
            printw("Writing into %s. Getting some input %i/%i", folder, length, MAX_LENGTH - 1);
            move(1, 0);
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
}

// function that combines the whole reading of files
int get_read_files_complete(char *readfile, char *folder)
{
    char years[NUM_YEARS][8];
    int year_files = get_year_files(folder, years);
    if (year_files <= 0)
    {
        printf("No entries found in %s.\n", folder);
        return 1;
    }
    strcpy(readfile, folder);
    int year_option = -1;
    while (year_option < 0 || year_option > year_files - 1)
    {
        printf("Choose which year you would like to view.\n");
        for (int i = 0; i < year_files; i++)
        {
            printf("%i. %s\n", i + 1, years[i]);
        }
        scanf("%i", &year_option);
        year_option -= 1;
    }
    printf("You chose %s\n", years[year_option]);
    strcat(readfile, "/");
    strcat(readfile, years[year_option]);
    printf("%s\n", readfile);


    char months[NUM_MONTHS][24];
    int month_files = get_month_files(readfile, months);
    if (month_files <= 0)
    {
        printf("No entries found in %s.\n", readfile);
        return 1;
    }
    int month_option = -1;
    while (month_option < 0 || month_option > month_files - 1)
    {
        printf("Choose which month you would like to view.\n");
        for (int i = 0; i < month_files; i++)
        {
            printf("%i. %s\n", i + 1, months[i]);
        }
        scanf("%i", &month_option);
        month_option -= 1;
    }
    printf("You chose %s\n", months[month_option]);
    strcat(readfile, "/");
    strcat(readfile, months[month_option]);
    printf("%s\n", readfile);


    char readfiles[100][100];
    int filecount = get_read_files(readfile, readfiles);
    if (filecount <= 0)
    {
        printf("No files found in %s.\n", readfile);
        return 1;
    }
    int file_option = -1;
    while (file_option < 0 || file_option > filecount - 1)
    {
        printf("Choose which file you would like to view.\n");
        for (int i = 0; i < filecount; i++)
        {
            printf("%i. %s\n", i + 1, readfiles[i]);
        }
        scanf("%i", &file_option);
        file_option -= 1;
    }
    printf("You chose %s\n", readfiles[file_option]);
    strcat(readfile, "/");
    strcat(readfile, readfiles[file_option]);
    printf("%s\n", readfile);
    return 0;
}