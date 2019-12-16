#!/bin/bash
mkdir wrong
i=0
while [ "${i}" != "1000" ]
do
    ./random $RANDOM 255 >tmp
    ./assembler <tmp
    ./dMemWriter <Data
    ./cmp.sh 1>/dev/null 2>/dev/null
    
    size=$(stat --printf="%s" Error.diff)
    
    if [ "${size}" != "0" ]; then
        mv tmp wrong/test_$(date +%m-%d-%H-%M)_$i.wrg
    fi
    i=$(($i+1))
    #echo $i
    #sleep 1
done