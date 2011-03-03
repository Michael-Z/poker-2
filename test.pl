#!/usr/bin/perl

system("rm foo* -f;");
system("rm output.txt -f;");
system("./play_match.pl foo wvholdem.limit.2p.game ".$ARGV[0]." ".$ARGV[1]." my_player my_player.wvlimit.2p.sh Random example_player.wvlimit.2p.sh;");
system("./play_match.pl foo wvholdem.limit.2p.game ".$ARGV[0]." ".($ARGV[1]+1+"")." my_player my_player.wvlimit.2p.sh Random example_player.wvlimit.2p.sh;");
system("./play_match.pl foo wvholdem.limit.2p.game ".$ARGV[0]." ".($ARGV[1]+2+"")." my_player my_player.wvlimit.2p.sh Random example_player.wvlimit.2p.sh;");

