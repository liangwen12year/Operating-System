CC=gcc
CFLAGS=-Wall --std=gnu99

a1d:A1Driver.o A1
	$(CC) $(CFLAGS) -o a1d A1Driver.o
A1:Assign1.o 
	$(CC) $(CFLAGS) -o A1 Assign1.o
Assign1.o:Assign1.c
	$(CC) $(CFLAGS) -c Assign1.c -o Assign1.o
A1Driver.o:main.c
	$(CC) $(CFLAGS) -c main.c -o A1Driver.o
clean:
	rm a1d A1 Assign1.o A1Driver.o
