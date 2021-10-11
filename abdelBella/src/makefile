all: serveur1 serveur2 serveur3

serveur1 : serveur1.o
	gcc serveur1.o -o ../bin/serveur1-abdelBella
serveur1.o : serveur1-abdelBella.c
	gcc -c serveur1-abdelBella.c -o serveur1.o

serveur2 : serveur2.o
	gcc serveur2.o -o ../bin/serveur2-abdelBella
serveur2.o : serveur2-abdelBella.c
	gcc -c serveur2-abdelBella.c -o serveur2.o

serveur3 : serveur3.o
	gcc serveur3.o -o ../bin/serveur3-abdelBella
serveur3.o : serveur3-abdelBella.c
	gcc -c serveur3-abdelBella.c -o serveur3.o

clean: 
	rm -rf *.o *.~ ../bin/serveur1-abdelBella ../bin/serveur2-abdelBella ../bin/serveur3-abdelBella