all:
	gcc initializer.c -lpthread -lrt -o init.out
	gcc producer.c -lpthread -lrt -o prod.out
	gcc destroyer.c -lpthread -lrt -o dest.out

init:
	gcc initializer.c -lpthread -lrt -o init.out

prod:
	gcc producer.c -lpthread -lrt -o prod.out

dest:
	gcc destroyer.c -lpthread -lrt -o dest.out

clean:
	rm *.out *.txt