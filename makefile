all: shell_command.c
	gcc shell_command.c

run: all
	./a.out

clean:
	rm *.o
	rm *~
	rm a.out
