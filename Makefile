CC = gcc
CFLAGS = -O3 -Wall

all:	dealer example_player improved_player

dealer: game.c game.h evalHandTables rng.c rng.h dealer.c
	$(CC) $(CFLAGS) -o $@ game.c rng.c dealer.c

example_player: game.c game.h evalHandTables rng.c rng.h example_player.c handValue/handValue.h handValue/handValue.c
	$(CC) $(CFLAGS) -o $@ game.c rng.c example_player.c handValue/handValue.c

improved_player: game.c game.h evalHandTables rng.c rng.h improved_player.c handValue/handValue.h handValue/handValue.c
	$(CC) $(CFLAGS) -o $@ game.c rng.c improved_player.c handValue/handValue.c

clean:
	rm -f dealer example_player
