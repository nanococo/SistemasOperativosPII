all:
	gcc initializer.c -o init.out
	./init.out 1234

init:
	gcc initializer.c -o init.out

clean:
	rm *.out