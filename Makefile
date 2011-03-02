CC = gcc
#CFLAGS = -DDEBUG  -g -O3 -Wall
CFLAGS = -O3 -Wall

all:	dealer example_player my_player improved_player

dealer: game.c game.h evalHandTables rng.c rng.h dealer.c
	$(CC) $(CFLAGS) -o $@ game.c rng.c dealer.c


example_player: game.c game.h evalHandTables rng.c rng.h example_player.c handValue/handValue.h handValue/handValue.c
	$(CC) $(CFLAGS) -o $@ game.c rng.c example_player.c handValue/handValue.c

improved_player: game.c game.h evalHandTables rng.c rng.h improved_player.c handValue/handValue.h handValue/handValue.c
	$(CC) $(CFLAGS) -o $@ game.c rng.c improved_player.c handValue/handValue.c

my_player: game.c game.h evalHandTables rng.c rng.h my_player.c opponent_model.c opponent_model.h handValue/handValue.h handValue/handValue.c gameTree/gameTree.h gameTree/gameTree.c
	$(CC) $(CFLAGS) -o $@ game.c rng.c my_player.c opponent_model.c handValue/handValue.c gameTree/gameTree.c

clean:
	rm -f dealer example_player improved_player my_player
