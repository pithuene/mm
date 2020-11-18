.PHONY: all clear

objects = main.o ssh.o sftp.o knownhost.o list.o

all: mm

clean:
	rm $(objects)

mm: $(objects)
	gcc -Wall -o mm -l ncurses -l ssh $(objects)

main.o: main.c
	gcc -Wall -c main.c

ssh.o: ssh.c ssh.h
	gcc -Wall -c ssh.c

sftp.o: sftp.c sftp.h
	gcc -Wall -c sftp.c

knownhost.o: knownhost.c knownhost.h
	gcc -Wall -c knownhost.c

list.o: list.c list.h
	gcc -Wall -c list.c
