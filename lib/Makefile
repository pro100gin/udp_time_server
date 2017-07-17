CFLAGS = -g -O2 -Wall -pedantic -ansi 
LFLAGS = 

vpath %.h ../include/
		
chat.o: chat.c chat.h
		gcc -c $< $(CFLAGS) $(LFLAGS) 

list.o: list.c list.h
		gcc -c $< $(CFLAGS) $(LFLAGS)
clean:
		rm -rf *.o
