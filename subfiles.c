#include <stdio.h>
#include <dirent.h>
#include <string.h>

int main() {
    const char* directoryPath = "entries";

    DIR* directory = opendir(directoryPath);
    if (directory == NULL) {
        printf("Failed to open directory: %s\n", directoryPath);
        return 1;
    }

    printf("Subdirectories in %s:\n", directoryPath);

    struct dirent* entry;
    while ((entry = readdir(directory)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                printf("%s\n", entry->d_name);
            }
        }
    }

    closedir(directory);

    return 0;
}