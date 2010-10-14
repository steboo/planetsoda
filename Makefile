CC=g++
WARN=-pedantic -Wall

all: MyBot DemoBot

clean:
	rm -rf *.o MyBot DemoBot

MyBot: MyBot.o PlanetWars.o
DemoBot: DemoBot.o PlanetWars.o
