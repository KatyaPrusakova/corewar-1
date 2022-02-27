#!/bin/bash

# Executes multiple champions with our test_multiplayer_diff.sh and test_single_player.sh scripts
our_champ=valid_players/zork_beater.s
singleplayer=test_single_player_diff.sh
multiplayer=test_multiplayer_diff.sh

echo -e "\\n\033[1;34mRun single player tests with all of our own test champions:\033[0m"
for f in test_instructions/*.s; do
    bash $singleplayer $f
    bash $singleplayer $f 50
    bash $singleplayer $f 1000
    bash $singleplayer $f 10000
    bash $singleplayer $f 100000
done

echo -e "\\n\033[1;34mRun multi_player tests with subject champions:\033[0m"
for f in subject_reference/champs/*.s; do
    bash $multiplayer $f $our_champ
    bash $multiplayer $f $our_champ 50
    bash $multiplayer $f $our_champ 1000
    bash $multiplayer $f $our_champ 10000
    bash $multiplayer $f $our_champ 100000
done