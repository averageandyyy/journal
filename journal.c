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


#define MAX_LENGTH 2048
#define NUM_YEARS 24
#define NUM_MONTHS 12
#define JOURNAL_FOLDER "entries"

void get_entry(char *input, int max_length, char *folder);
void get_date(char *date, size_t buffer_size);
void get_year(char *year, size_t buffer_size);
void get_month(char *month, size_t buffer_size);
void remove_new_line(char *str);
bool create_directories(char *directory);
bool file_exists(char *filename);
int file_count(char *filepath);
int get_year_files(char *folder, char years[][8]);
int get_month_files(char *folder,char months[][24]);
int get_read_files(char *folder,char readfiles[][100]);
void read_file(FILE *to_read);
void update_entry(char *input, int max_length, char *folder);


int main(int argc, char* argv[])
{
    char *options = "rwh";
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
        printf("Usage: ./journal [flag] [directory]\nOtherwise, default folder is entries/\n-r to read and -w to write and -h for help\n");
        return 3;
    }

    char date[80];
    get_date(date, sizeof(date));
    printf("Current date: %s\n", date);
    char year[8];
    get_year(year, sizeof(year));
    char month[24];
    get_month(month, sizeof(month));
    char folder[80];
    (argc == 3) ? strcpy(folder,argv[2]) : strcpy(folder,JOURNAL_FOLDER);

    char input[MAX_LENGTH];

    switch (option)
    {
        case 'h':
            printf("Usage: ./journal [flag] [directory]\nOtherwise, default folder is entries/\n-r to read and -w to write and -h for help\n");
            return 0;


        case 'w':
            char file[200]; 
            char directory[100];
            do
            {
                // reset file string on every iteration
                memset(file, 0, sizeof(file));
                memset(directory, 0, sizeof(directory));
                // copy folder unto file
                (argc == 3) ? strcpy(file,argv[2]) : strcpy(file,JOURNAL_FOLDER);
                // adding a / char
                strcat(file, "/");
                strcat(directory, file);


                get_year(year, sizeof(year));
                strcat(file, strcat(year, "/"));
                strcat(directory, year);


                get_month(month, sizeof(month));
                strcat(directory, month);
                strcat(file, strcat(month, "/"));


                printf("%s\n", directory);
                char title[50];
                printf("Give us a title: ");
                fgets(title, 50, stdin);
                remove_new_line(title);
                if (strlen(title) == 0)
                {
                    int filecount = file_count(file);
                    sprintf(title, "%03i", filecount);
                }
                strcat(title, ".txt");
                strcat(file, title);
            } while (file_exists(file));
            
            printf("Creating directories in %s.\n", directory);
            create_directories(directory);
            printf("Writing into %s\n", file);
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
        
        
        
        
        
        case 'r':
            char readfile[100]; 
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
                    printf("%i. %s\n", i+1, years[i]);
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
                    printf("%i. %s\n", i+1, months[i]);
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
                    printf("%i. %s\n", i+1, readfiles[i]);
                }
                scanf("%i", &file_option);
                file_option -= 1;
            }
            printf("You chose %s\n", readfiles[file_option]);
            strcat(readfile, "/");
            strcat(readfile, readfiles[file_option]);
            printf("%s\n", readfile);
            printf("Opening %s...\n", readfile);
            FILE *to_read = fopen(readfile, "r+");
            if (to_read == NULL)
            {
                printf("Failed to open %s.\n", readfile);
                return 1;
            }
            int read_or_update = -1;
            while (read_or_update < 0 || read_or_update > 1)
            {
                printf("1 for read, 2 for update: ");
                scanf("%i", &read_or_update);
                read_or_update -= 1;
            }
            if (read_or_update == 0)
            {
                read_file(to_read);
            }
            else if (read_or_update == 1)
            {
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
            break;
    }
    return 0;
}



void get_entry(char *input, int max_length, char *folder)
{
    initscr(); // initialize ncurses
    cbreak();  // disable line buffering, receive input character by character
    keypad(stdscr, TRUE); // enable keypad for arrow key input


    mvprintw(0, 0, "writing to %s", folder);
    mvprintw(1, 0, "Enter a string (maximum %d characters)", max_length - 1);
    mvprintw(2, 0, "Character count: 0/%d", max_length - 1);
    refresh();

    int length = 0;
    int cursorPos = 0;
    int ch;
    while((ch = getch()) != '\n' && length < max_length - 1)
    {
        if (ch == ERR)
        {
            continue;
        }

        if (ch == KEY_BACKSPACE || ch == 127 || ch == KEY_DC) // handle backspace
        {
            if (cursorPos > 0)
            {
                // deletes character before cursorPos and moves it there
                mvdelch(3, cursorPos - 1);
                // cursorPos variable yet to be updated
                // thus we can use it to update the input array
                // chars before cursorPos at updated to become those one place after it
                for (int i = cursorPos - 1; i < length - 1; i++)
                {
                    input[i] = input[i+1];
                    mvprintw(3, i, "%c", input[i]);
                }
                // however input[length - 1] is not updated?
                // but because the length variable decrements later
                // that variable will be set to \0 so no worries!
                // while there are characters left behind
                // the length variable 'truncates' it so it appears as normal
                mvprintw(3, length - 1, " "); // clear last character
                length--;
                cursorPos--;
                mvprintw(2, 17, "       "); // clear character count
                mvprintw(2, 17, "%d/%d", length, max_length - 1); // print updated count
                move(3, cursorPos);
                refresh();
            }
        }
        else if (ch == KEY_LEFT) // handle left arrow key
        {
            // nesting it like this seems to prevent the keys from being entered
            if (cursorPos > 0)
            {
                cursorPos--;
                move(3, cursorPos);
                refresh();
            }
        }
        else if (ch == KEY_RIGHT)
        {
            // nesting it like this seems to prevent the keys from being entered
            if (cursorPos < length)
            {
                cursorPos++;
                move(3, cursorPos);
                refresh();   
            }
        }
        else
        {
            // Insert character at cursorPos
            if (cursorPos == length)
            {
                input[length] = ch;
                mvprintw(3, cursorPos, "%c", ch);
                length++;
                cursorPos++;
                mvprintw(2, 17, "       "); // clear character count
                mvprintw(2, 17, "%d/%d", length, max_length - 1); // print updated count
                move(3, cursorPos);
                refresh();
            }
            else
            {
                for (int i = length; i > cursorPos; i--)
                {
                    input[i] = input[i - 1];
                    mvprintw(3, i, "%c", input[i]);
                    mvprintw(2, 17, "       "); // clear character count
                    mvprintw(2, 17, "%d/%d", length, max_length - 1); // print updated count
                }
                input[cursorPos] = ch;
                length++;
                cursorPos++;
                mvprintw(2, 17, "       "); // clear character count
                mvprintw(2, 17, "%d/%d", length, max_length - 1); // print updated count
                move(3, cursorPos);
                refresh();
            }

        }
    }

    input[length] = '\0';

    endwin();
}

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




void remove_new_line(char *str)
{
    size_t length = strcspn(str, "\n");
    str[length] = '\0';
}

bool file_exists(char *filename)
{
    if (access(filename, F_OK) != -1)
    {
        printf("%s exists!\n", filename);
        return true;
    }
    
    return false;
}

int file_count(char *filepath)
{
    DIR* dir = opendir(filepath);

    if (dir == NULL)
    {
        printf("Failed to open directory\n");
        return 0;
    }
    int filecount = 0;
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            filecount++;
        }
    }

    closedir(dir);

    return filecount;
}

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

int get_year_files(char *folder,char years[][8])
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
    while((entry = readdir(directory)) != NULL)
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

int get_month_files(char *folder,char months[][24])
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
    while((entry = readdir(directory)) != NULL)
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


int get_read_files(char *folder,char readfiles[][100])
{
    // years[NUM_YEARS][8];
    DIR *directory = opendir(folder);
    if (directory == NULL)
    {
        printf("Failed to open directory.\n");
        return -1;
    }

    int filecount = 0;
    struct dirent *entry;
    while((entry = readdir(directory)) != NULL)
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



void update_entry(char *input, int max_length, char *folder)
{
    initscr(); // initialize ncurses
    cbreak();  // disable line buffering, receive input character by character
    keypad(stdscr, TRUE); // enable keypad for arrow key input
    int length = strlen(input);
    int cursorPos = strlen(input);


    mvprintw(0, 0, "writing to %s", folder);
    mvprintw(1, 0, "Enter a string (maximum %d characters)", max_length - 1);
    mvprintw(2, 0, "Character count: %d/%d", length, max_length - 1);
    mvprintw(3, 0, "%s", input);
    move(3, cursorPos);
    refresh();

    
    int ch;
    while((ch = getch()) != '\n' && length < max_length - 1)
    {
        if (ch == ERR)
        {
            continue;
        }

        if (ch == KEY_BACKSPACE || ch == 127 || ch == KEY_DC) // handle backspace
        {
            if (cursorPos > 0)
            {
                // deletes character before cursorPos and moves it there
                mvdelch(3, cursorPos - 1);
                // cursorPos variable yet to be updated
                // thus we can use it to update the input array
                // chars before cursorPos at updated to become those one place after it
                for (int i = cursorPos - 1; i < length - 1; i++)
                {
                    input[i] = input[i+1];
                    mvprintw(3, i, "%c", input[i]);
                }
                // however input[length - 1] is not updated?
                // but because the length variable decrements later
                // that variable will be set to \0 so no worries!
                // while there are characters left behind
                // the length variable 'truncates' it so it appears as normal
                mvprintw(3, length - 1, " "); // clear last character
                length--;
                cursorPos--;
                mvprintw(2, 17, "       "); // clear character count
                mvprintw(2, 17, "%d/%d", length, max_length - 1); // print updated count
                move(3, cursorPos);
                refresh();
            }
        }
        else if (ch == KEY_LEFT) // handle left arrow key
        {
            // nesting it like this seems to prevent the keys from being entered
            if (cursorPos > 0)
            {
                cursorPos--;
                move(3, cursorPos);
                refresh();
            }
        }
        else if (ch == KEY_RIGHT)
        {
            // nesting it like this seems to prevent the keys from being entered
            if (cursorPos < length)
            {
                cursorPos++;
                move(3, cursorPos);
                refresh();   
            }
        }
        else
        {
            // Insert character at cursorPos
            if (cursorPos == length)
            {
                input[length] = ch;
                mvprintw(3, cursorPos, "%c", ch);
                length++;
                cursorPos++;
                mvprintw(2, 17, "       "); // clear character count
                mvprintw(2, 17, "%d/%d", length, max_length - 1); // print updated count
                move(3, cursorPos);
                refresh();
            }
            else
            {
                for (int i = length; i > cursorPos; i--)
                {
                    input[i] = input[i - 1];
                    mvprintw(3, i, "%c", input[i]);
                    mvprintw(2, 17, "       "); // clear character count
                    mvprintw(2, 17, "%d/%d", length, max_length - 1); // print updated count
                }
                input[cursorPos] = ch;
                length++;
                cursorPos++;
                mvprintw(2, 17, "       "); // clear character count
                mvprintw(2, 17, "%d/%d", length, max_length - 1); // print updated count
                move(3, cursorPos);
                refresh();
            }

        }
    }

    input[length] = '\0';

    endwin();
}