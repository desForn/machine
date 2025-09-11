#!/bin/bash
result=0
for i in $(cat tests_list)
do
    o=$(bash run.sh ${i})
    if [ ! "${o}" = "All tests succeded" ]
    then
        result=1
        echo "Test ${i} failed:"
        printf "${o}\n"
    fi 
done

if [ $result = 0 ]
then
    echo "All tests succeded"
else
    echo "Some tests failed"
fi

