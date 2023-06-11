journal: journal.c
	gcc journal.c -lncurses -o journal

output: output.c
	gcc output.c -lncurses -o output

getinput: getinput.c
	gcc getinput.c -lncurses -o getinput

getnstr: getnstr.c
	gcc getnstr.c -lncurses -o getnstr