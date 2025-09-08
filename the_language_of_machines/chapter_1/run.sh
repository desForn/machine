#!/bin/bash
if [ ! -d $1 ]
then
    echo "Invalid argument"
fi

cd $1

tested=0
result=0

if [ -e "test_accept" ]
then
    for i in $(cat test_accept)
    do
        tested=1
        ../../../run -p program -i ${i} -o Accept
        if [ ! "$?" = 0 ]
        then
            result=1
            echo "The following test did not succeed:"
            echo "../../../run -p program -i ${i} -o Accept"
        fi
    done
fi

if [ -e "test_reject" ]
then
    for i in $(cat test_reject)
    do
        tested=1
        ../../../run -p program -i ${i} -o Reject
        if [ ! "$?" = 0 ]
        then
            result=1
            echo "The following test did not succeed:"
            echo "../../../run -p program -i ${i} -o Reject"
        fi
    done
fi

if [ -e "test_block" ]
then
    for i in $(cat test_block)
    do
        tested=1
        ../../../run -p program -i ${i} -o Accept
        if [ ! "$?" = 5 ]
        then
            result=1
            echo "The following test did not succeed (failure was expected):"
            echo "../../../run -p program -i ${i} -o Accept"
        fi
    done
fi

if [ -e "test_input" ] && [ -e "test_output" ]
then
    ni=$(wc -l test_input | awk '{print $1}')
    no=$(wc -l test_output | awk '{print $1}')
    if [[ ${ni} -ne ${no} ]]
    then
        echo "Incongruent input and output files"
    else
        for (( i=1; i <= ni; ++i ))
        do
            tested=1
            input=$(sed "${i}q;d" test_input)
            output=$(sed "${i}q;d" test_output)

            ../../../run -p program -i ${input} -o ${output}
            if [ ! "$?" = 0 ]
            then
                result=1
                echo "The following test did not succeed:"
                echo "../../../run -p program -i ${input} -o ${output}"
            fi
        done
    fi
fi

if [ $tested = 0 ]
then
    echo "Not performed any test"
else
    if [ $result = 0 ]
    then
        echo "All tests succeded"
    else
        echo "Some tests did not succeed"
    fi
fi

