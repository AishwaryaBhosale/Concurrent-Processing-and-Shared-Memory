all: SwimMill.o fish.o pellet.o

SwimMill.o: Swim_mill.c
	gcc -o SwimMill Swim_mill.c -I -lm -pthread

fish.o: fish.c
	gcc -o fish fish.c -lm

pellet.o: pellet.c
	gcc -o pellet pellet.c -I -lm 
