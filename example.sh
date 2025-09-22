#!/bin/bash

for i in "aa" "ab" "ab#ba" "ab#" "#ba"
do
    ./run -p the_language_of_machines/chapter_1/exercice_1_6_1/program_c_3ucm.m -i ${i} -o Accept -d Yes
    if [ ! "$?" = "5" ]
    then
        echo "Unexpected output"
    fi
done

for i in "#" "a#a" "aa#aa" "b#b" "bb#bb" "ab#ab" "abbaaba#abbaaba"
do
    ./run -p the_language_of_machines/chapter_1/exercice_1_6_1/program_c_3ucm.m -i ${i} -o Accept -d Yes
    if [ ! "$?" = "0" ]
    then
        echo "Unexpected output"
    fi
done
