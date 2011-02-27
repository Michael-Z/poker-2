#!/usr/bin/perl

system("rm foo* -f;");
system("rm output.txt -f;");
system("./play_match.pl foo wvholdem.limit.2p.game ".$ARGV[0]." ".$ARGV[1]." IHS_player improved_player.wvlimit.2p.sh Evans benchmark_player.wvlimit.2p.sh;");
system("./play_match.pl foo wvholdem.limit.2p.game ".$ARGV[0]." ".($ARGV[1]+1+"")." IHS_player improved_player.wvlimit.2p.sh Evans benchmark_player.wvlimit.2p.sh;");
system("./play_match.pl foo wvholdem.limit.2p.game ".$ARGV[0]." ".($ARGV[1]+2+"")." IHS_player improved_player.wvlimit.2p.sh Evans benchmark_player.wvlimit.2p.sh;");

