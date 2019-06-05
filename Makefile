default:	messenger

messenger:	
	gcc -o run -g messenger.c -lrt -lpthread

clean:	
	-rm -f run