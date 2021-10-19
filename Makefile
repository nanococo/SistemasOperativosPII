all:
	gcc initializer.c -o init.out
	gcc destroyer.c -o dest.out

init:
	gcc initializer.c -o init.out

dest:
	gcc destroyer.c -o dest.out

clean:
	rm *.out