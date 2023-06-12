# journal
journal program written in C. input is made such that it mimics the terminal.

Usage: ./journal [flags] [directory]

flags are -w to write a new text file, -r to read an exiting text file and -u to update an existing text file.

directory is optional, but reading and writing will be done in a entries/ directory by default if directory is not specified which will also be created if it does not exist when writing.

file structure is entries/year/month/actualfile.txt
